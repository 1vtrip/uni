#include "service/FileSystem.h"
#include "core/Path.h"
#include "exceptions/Exceptions.h"

// ===================== конструктор и время =====================

FileSystem::FileSystem(const std::string& name, const StorageDevice& device, const Timestamp& now)
    : name_(name), now_(now), directories_(), textFiles_(), images_(), executables_(), archives_(), symlinks_(),
      users_(), guests_(), admins_(), services_(), groups_(), device_(device), acls_(), versions_(),
      trash_(30, DataSize::fromGB(1)), locks_(), audit_(1000), shares_(), backups_(BackupPolicy()),
      auth_(PasswordPolicy()), notifier_(), compressor_(), nextEntryId_(HOME_ID + 1), nextAccountId_(1),
      sessionId_(0) {
    // владелец корня и /home — система (ID 0): обычные пользователи туда не пишут
    directories_.push_back(Directory(ROOT_ID, "", 0, 0, Permission::forDirectory(), now));
    directories_.push_back(Directory(HOME_ID, "home", ROOT_ID, 0, Permission::forDirectory(), now));
    directory(ROOT_ID).addChild(HOME_ID);

    int backupId = nextAccountId_++;
    services_.push_back(ServiceAccount(backupId, "backup", "Резервное копирование",
                                       Credentials::create("backup", "Backup2026!"), now, "backup"));
    services_.back().allow("backup");
}

const Timestamp& FileSystem::now() const { return now_; }

void FileSystem::advanceTime(int minutes) {
    if (minutes <= 0) throw InvalidOperationException("время можно перемотать только вперёд");
    now_ = now_.addMinutes(minutes);
}

// ===================== учётные записи =====================

/**
 * @brief Все проверки до любых изменений: если бы папку создали, а потом логин оказался занят,
 * в /home осталась бы папка без хозяина.
 */
void FileSystem::checkNewLogin(const std::string& login) const {
    if (!Path::isValidName(login)) throw InvalidPathException("логин \"" + login + "\" не годится как имя папки");
    if (findAccount(login) != 0) throw EntryAlreadyExistsException("логин " + login);
    if (findChild(HOME_ID, login) != 0) throw EntryAlreadyExistsException("/home/" + login);
    if (directory(HOME_ID).isFull()) throw InvalidOperationException("в /home больше нет места для папок");
}

/**
 * @brief Ловушка с push_back: вектор папок может переехать в памяти, и ссылка на /home,
 * взятая до push_back, станет недействительной. Поэтому /home берётся заново уже после него.
 */
int FileSystem::createHome(const std::string& login, int ownerId) {
    int homeId = nextEntryId_++;
    directories_.push_back(Directory(homeId, login, HOME_ID, ownerId, Permission(0700), now_));
    directory(HOME_ID).addChild(homeId);
    return homeId;
}

/**
 * @brief Квота ставится через resetQuota() уже после push_back: в конструкторе UserAccount
 * виртуальный defaultQuota() не дошёл бы до наследника (это важно для гостя).
 */
int FileSystem::addUser(const std::string& login, const std::string& name, const std::string& password) {
    checkNewLogin(login);
    auth_.getPolicy().validate(password, login);
    int id = nextAccountId_++;
    int homeId = createHome(login, id);
    users_.push_back(UserAccount(id, login, name, Credentials::create(login, password), now_, homeId));
    users_.back().resetQuota();
    return id;
}

int FileSystem::addGuest(const std::string& login, const std::string& password, int days) {
    if (days <= 0) throw InvalidOperationException("гостевой доступ даётся на положительное число дней");
    checkNewLogin(login);
    auth_.getPolicy().validate(password, login);
    int id = nextAccountId_++;
    int homeId = createHome(login, id);
    guests_.push_back(GuestAccount(id, login, "Гость " + login, Credentials::create(login, password), now_,
                                   homeId, now_.addDays(days)));
    guests_.back().resetQuota();   // 50 MB: виртуальный вызов после создания объекта
    return id;
}

/** @brief Объект создаётся до push_back: если уровень неверный, конструктор бросит, а вектор не изменится. */
int FileSystem::addAdmin(const std::string& login, const std::string& name, const std::string& password, int level) {
    checkNewLogin(login);
    auth_.getPolicy().validate(password, login);
    AdminAccount admin(nextAccountId_, login, name, Credentials::create(login, password), now_, level);
    ++nextAccountId_;
    admins_.push_back(admin);
    return admin.getId();
}

/** @brief Тот же приём, что entry(): поиск по всем векторам, наружу — ссылка на базовый класс Account. */
const Account& FileSystem::account(int id) const {
    for (size_t i = 0; i < users_.size(); ++i)    if (users_[i].getId() == id)    return users_[i];
    for (size_t i = 0; i < guests_.size(); ++i)   if (guests_[i].getId() == id)   return guests_[i];
    for (size_t i = 0; i < admins_.size(); ++i)   if (admins_[i].getId() == id)   return admins_[i];
    for (size_t i = 0; i < services_.size(); ++i) if (services_[i].getId() == id) return services_[i];
    throw EntryNotFoundException("учётная запись #" + std::to_string(id));
}

Account& FileSystem::accountRef(int id) {
    for (size_t i = 0; i < users_.size(); ++i)    if (users_[i].getId() == id)    return users_[i];
    for (size_t i = 0; i < guests_.size(); ++i)   if (guests_[i].getId() == id)   return guests_[i];
    for (size_t i = 0; i < admins_.size(); ++i)   if (admins_[i].getId() == id)   return admins_[i];
    for (size_t i = 0; i < services_.size(); ++i) if (services_[i].getId() == id) return services_[i];
    throw EntryNotFoundException("учётная запись #" + std::to_string(id));
}

int FileSystem::findAccount(const std::string& login) const {
    for (size_t i = 0; i < users_.size(); ++i)    if (users_[i].getLogin() == login)    return users_[i].getId();
    for (size_t i = 0; i < guests_.size(); ++i)   if (guests_[i].getLogin() == login)   return guests_[i].getId();
    for (size_t i = 0; i < admins_.size(); ++i)   if (admins_[i].getLogin() == login)   return admins_[i].getId();
    for (size_t i = 0; i < services_.size(); ++i) if (services_[i].getLogin() == login) return services_[i].getId();
    return 0;
}

// ===================== вход и сессия =====================

/**
 * @brief Неудачный вход ловится, пишется в аудит и бросается дальше через throw; — меню поймает его само.
 * Если именно эта попытка заблокировала запись, владельцу уходит предупреждение о безопасности.
 */
void FileSystem::login(const std::string& login, const std::string& password) {
    logout();
    int id = findAccount(login);
    if (id == 0) {
        audit_.record(now_, 0, "login", login, false, "нет такого логина");
        throw AuthenticationException(login);
    }
    Account& acc = accountRef(id);
    bool wasBlocked = acc.isBlocked();
    try {
        sessionId_ = auth_.login(acc, password, now_);
    } catch (const AuthenticationException& e) {
        audit_.record(now_, id, "login", login, false, e.what());
        if (!wasBlocked && acc.isBlocked())
            notifier_.securityAlert(id, "учётная запись заблокирована после "
                                        + std::to_string(Account::MAX_FAILED_LOGINS) + " неверных паролей подряд", now_);
        throw;
    }
    audit_.record(now_, id, "login", login, true);
}

/** @brief Сессия могла уже истечь или быть удалена — выйти всё равно можно, поэтому ошибка не важна. */
void FileSystem::logout() {
    if (sessionId_ == 0) return;
    int userId = 0;
    try {
        userId = auth_.session(sessionId_).getUserId();
        auth_.logout(sessionId_);
    } catch (const SessionExpiredException&) {
    }
    if (userId != 0) audit_.record(now_, userId, "logout", "", true);
    sessionId_ = 0;
}

/** @brief Истёкшая сессия сбрасывается в 0, чтобы меню показало «не выполнен вход». */
int FileSystem::currentUserId() {
    if (sessionId_ == 0) throw SessionExpiredException();
    try {
        return auth_.requireSession(sessionId_, now_);
    } catch (const SessionExpiredException&) {
        sessionId_ = 0;
        throw;
    }
}

/** @brief Только проверка, без отметки активности; удалённая сессия считается истёкшей. */
bool FileSystem::isLoggedIn() const {
    if (sessionId_ == 0) return false;
    try {
        return !auth_.session(sessionId_).isExpired(now_);
    } catch (const SessionExpiredException&) {
        return false;
    }
}

// ===================== дерево: поиск элементов =====================

// Ищем объект по всем векторам и возвращаем ссылку на базовый класс.
// Виртуальные методы через неё вызывают версию настоящего типа — без указателей.
const FsEntry& FileSystem::entry(int id) const {
    for (size_t i = 0; i < directories_.size(); ++i) if (directories_[i].getId() == id) return directories_[i];
    for (size_t i = 0; i < textFiles_.size(); ++i)   if (textFiles_[i].getId() == id)   return textFiles_[i];
    for (size_t i = 0; i < images_.size(); ++i)      if (images_[i].getId() == id)      return images_[i];
    for (size_t i = 0; i < executables_.size(); ++i) if (executables_[i].getId() == id) return executables_[i];
    for (size_t i = 0; i < archives_.size(); ++i)    if (archives_[i].getId() == id)    return archives_[i];
    for (size_t i = 0; i < symlinks_.size(); ++i)    if (symlinks_[i].getId() == id)    return symlinks_[i];
    throw EntryNotFoundException("#" + std::to_string(id));
}

FsEntry& FileSystem::entry(int id) {
    for (size_t i = 0; i < directories_.size(); ++i) if (directories_[i].getId() == id) return directories_[i];
    for (size_t i = 0; i < textFiles_.size(); ++i)   if (textFiles_[i].getId() == id)   return textFiles_[i];
    for (size_t i = 0; i < images_.size(); ++i)      if (images_[i].getId() == id)      return images_[i];
    for (size_t i = 0; i < executables_.size(); ++i) if (executables_[i].getId() == id) return executables_[i];
    for (size_t i = 0; i < archives_.size(); ++i)    if (archives_[i].getId() == id)    return archives_[i];
    for (size_t i = 0; i < symlinks_.size(); ++i)    if (symlinks_[i].getId() == id)    return symlinks_[i];
    throw EntryNotFoundException("#" + std::to_string(id));
}

Directory& FileSystem::directory(int id) {
    for (size_t i = 0; i < directories_.size(); ++i) if (directories_[i].getId() == id) return directories_[i];
    throw InvalidPathException("#" + std::to_string(id) + " не папка");
}

const Directory& FileSystem::directory(int id) const {
    for (size_t i = 0; i < directories_.size(); ++i) if (directories_[i].getId() == id) return directories_[i];
    throw InvalidPathException("#" + std::to_string(id) + " не папка");
}

int FileSystem::findChild(int dirId, const std::string& name) const {
    const std::vector<int>& children = directory(dirId).getChildIds();
    for (size_t i = 0; i < children.size(); ++i)
        if (entry(children[i]).getName() == name) return children[i];
    return 0;
}

// ===================== доступ для отчётов и меню =====================

const std::string& FileSystem::getName() const { return name_; }
const std::vector<UserAccount>& FileSystem::getUsers() const { return users_; }
const std::vector<GuestAccount>& FileSystem::getGuests() const { return guests_; }
const std::vector<AdminAccount>& FileSystem::getAdmins() const { return admins_; }
const std::vector<ServiceAccount>& FileSystem::getServices() const { return services_; }
const std::vector<UserGroup>& FileSystem::getGroups() const { return groups_; }
const AuditLog& FileSystem::getAudit() const { return audit_; }
const RecycleBin& FileSystem::getTrash() const { return trash_; }
const NotificationService& FileSystem::getNotifications() const { return notifier_; }
const StorageDevice& FileSystem::getDevice() const { return device_; }
const AuthService& FileSystem::getAuth() const { return auth_; }
