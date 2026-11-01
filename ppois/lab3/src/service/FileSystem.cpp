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

const Symlink& FileSystem::symlink(int id) const {
    for (size_t i = 0; i < symlinks_.size(); ++i) if (symlinks_[i].getId() == id) return symlinks_[i];
    throw InvalidOperationException("#" + std::to_string(id) + " не ссылка");
}

// ===================== дерево: пути и размеры =====================

/**
 * @brief От корня по частям пути через findChild. Перед каждым шагом проверяется, что текущий элемент — папка:
 * в "/home/ivan/a.txt/x" файл a.txt стоит там, где нужна папка.
 */
int FileSystem::resolve(const std::string& path) const {
    Path p = Path::parse(path);
    if (!p.isAbsolute()) throw InvalidPathException(path + " (нужен абсолютный путь)");
    const std::vector<std::string>& parts = p.parts();
    int current = ROOT_ID;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (!entry(current).isDirectory())
            throw InvalidPathException(path + " (" + entry(current).getName() + " — не папка)");
        int next = findChild(current, parts[i]);
        if (next == 0) throw InvalidPathException(path + " (не найдено: " + parts[i] + ")");
        current = next;
    }
    return current;
}

/**
 * @brief Цикл (a -> b -> a) ловится ограничением глубины: настоящая цепочка не бывает длиннее 20 ссылок.
 * Symlink::resolve сам бросает EntryNotFoundException у битой ссылки, а entry() — если цель уже удалили.
 */
int FileSystem::followLink(int id) const {
    const int maxDepth = 20;
    int current = id;
    for (int step = 0; step <= maxDepth; ++step) {
        if (entry(current).typeChar() != 'l') return current;
        current = symlink(current).resolve();
        entry(current);
    }
    throw InvalidOperationException("цикл ссылок или слишком длинная цепочка: " + pathOf(id));
}

/** @brief Подъём по parentId до корня, имена собираются с конца; глубина ограничена на случай испорченных данных. */
std::string FileSystem::pathOf(int id) const {
    if (id == ROOT_ID) return "/";
    std::vector<std::string> names;
    int current = id;
    for (int depth = 0; current != ROOT_ID && current != 0 && depth < 1000; ++depth) {
        const FsEntry& e = entry(current);
        names.push_back(e.getName());
        current = e.getParentId();
    }
    std::string path;
    for (int i = (int)names.size() - 1; i >= 0; --i) path += "/" + names[i];
    return path;
}

std::vector<int> FileSystem::list(const std::string& dirPath) {
    int userId = currentUserId();
    int dirId = resolve(dirPath);
    if (!entry(dirId).isDirectory()) throw InvalidPathException(dirPath + " (не папка)");
    checkAccess(dirId, Permission::Access::Read, userId);
    return directory(dirId).getChildIds();
}

/** @brief Рекурсия по ID: у папки берётся размер содержимого, у остальных — виртуальный size(). */
DataSize FileSystem::directorySize(int dirId) const {
    const Directory& dir = directory(dirId);
    DataSize total = dir.size();
    const std::vector<int>& children = dir.getChildIds();
    for (size_t i = 0; i < children.size(); ++i) {
        const FsEntry& child = entry(children[i]);
        if (child.isDirectory()) total += directorySize(child.getId());   // рекурсия
        else                     total += child.size();                   // виртуальный вызов
    }
    return total;
}

// ===================== проверка прав =====================

/**
 * @brief У нашего FsEntry нет группы-владельца, как в Unix, только ownerId.
 * Поэтому «в группе» значит: у пользователя и владельца есть хотя бы одна общая группа.
 */
bool FileSystem::sharesGroup(int userId, int ownerId) const {
    if (ownerId == 0 || ownerId == userId) return false;
    const std::vector<int>& mine = account(userId).getGroupIds();
    try {
        const Account& owner = account(ownerId);
        for (size_t i = 0; i < mine.size(); ++i)
            if (owner.isInGroup(mine[i])) return true;
    } catch (const EntryNotFoundException&) {
        // владельца уже нет — общей группы тоже нет
    }
    return false;
}

bool FileSystem::allowedByRules(int entryId, Permission::Access a, int userId) const {
    const Account& acc = account(userId);
    for (size_t i = 0; i < acls_.size(); ++i) {
        if (acls_[i].getEntryId() != entryId) continue;
        int decision = acls_[i].check(a, userId, acc.getGroupIds());
        if (decision != 0) return decision > 0;
        break;
    }
    const FsEntry& e = entry(entryId);
    return e.canAccess(a, userId, sharesGroup(userId, e.getOwnerId()));
}

/**
 * @brief Право x нужно на каждой папке по пути, как в Unix: иначе гость прочитал бы файл rw-r--r--
 * из закрытой /home/ivan (rwx------), просто зная путь. Метод не const: отказ пишется в аудит.
 */
void FileSystem::checkAccess(int entryId, Permission::Access a, int userId) {
    const Account& acc = account(userId);
    if (a == Permission::Access::Read && acc.canReadAnyFile()) return;
    std::string what;
    int deniedAt = 0;   // где отказали: папка по пути или сам элемент
    for (int dir = entry(entryId).getParentId(); dir != 0 && deniedAt == 0; dir = entry(dir).getParentId())
        if (!allowedByRules(dir, Permission::Access::Execute, userId)) deniedAt = dir;
    if (deniedAt != 0) {
        what = "открыть папку " + pathOf(deniedAt);
    } else if (!allowedByRules(entryId, a, userId)) {
        deniedAt = entryId;
        const char* verb = a == Permission::Access::Read ? "читать " : a == Permission::Access::Write ? "изменять " : "выполнять ";
        what = verb + pathOf(entryId);
    }
    if (deniedAt == 0) return;
    audit_.record(now_, userId, "access", pathOf(entryId), false, "нельзя " + what);
    throw AccessDeniedException(acc.getLogin(), what);
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
