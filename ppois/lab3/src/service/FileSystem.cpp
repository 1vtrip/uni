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
        // через ссылку нельзя читать то, что лежит в корзине; после восстановления ссылка заработает сама
        if (trash_.contains(current)) throw EntryNotFoundException(pathOf(current) + " (в корзине)");
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

// ===================== место: квота и устройство =====================

bool FileSystem::hasQuota(int userId) const {
    for (size_t i = 0; i < users_.size(); ++i)  if (users_[i].getId() == userId)  return true;
    for (size_t i = 0; i < guests_.size(); ++i) if (guests_[i].getId() == userId) return true;
    return false;
}

/** @brief GuestAccount — наследник UserAccount, поэтому гость возвращается той же ссылкой на базовый класс. */
UserAccount& FileSystem::quotaOwner(int userId) {
    for (size_t i = 0; i < users_.size(); ++i)  if (users_[i].getId() == userId)  return users_[i];
    for (size_t i = 0; i < guests_.size(); ++i) if (guests_[i].getId() == userId) return guests_[i];
    throw InvalidOperationException("у учётной записи #" + std::to_string(userId) + " нет квоты");
}

/** @brief Снимки версий квоту не занимают: иначе 10 версий файла съедали бы место в 10 раз быстрее. */
void FileSystem::reserveSpace(int userId, const DataSize& size) {
    if (size.isZero()) return;
    bool quota = hasQuota(userId);
    if (quota) {
        UserAccount& u = quotaOwner(userId);
        // Quota знает только ID, поэтому сообщение с логином собирается здесь
        if (!u.getQuota().canAllocate(size))
            throw QuotaExceededException(u.getLogin(), size.toString(), u.getQuota().remaining().toString());
        u.getQuota().consume(size);
    }
    try {
        device_.allocate(size);
    } catch (const InsufficientStorageException&) {
        if (quota) quotaOwner(userId).getQuota().release(size);   // вернуть квоту, иначе место «утечёт»
        throw;
    }
}

void FileSystem::freeSpace(int ownerId, const DataSize& size) {
    if (hasQuota(ownerId)) quotaOwner(ownerId).getQuota().release(size);
    device_.free(size);
}

// ===================== создание элементов =====================

std::string FileSystem::childPath(int dirId, const std::string& name) const {
    return dirId == ROOT_ID ? "/" + name : pathOf(dirId) + "/" + name;
}

/** @brief Заполненность проверяется здесь, до занятия места: иначе квота занята, а addChild бросил бы. */
void FileSystem::checkCanCreate(int dirId, const std::string& name, int userId) {
    if (!entry(dirId).isDirectory()) throw InvalidPathException(pathOf(dirId) + " (не папка)");
    checkAccess(dirId, Permission::Access::Write, userId);
    if (!Path::isValidName(name)) throw InvalidPathException("недопустимое имя \"" + name + "\"");
    if (findChild(dirId, name) != 0) throw EntryAlreadyExistsException(childPath(dirId, name));
    if (directory(dirId).isFull()) throw InvalidOperationException("папка " + pathOf(dirId) + " заполнена");
}

/**
 * @brief Ловушка из плана: directories_.push_back может перенести вектор в памяти, и ссылка на
 * родительскую папку, взятая раньше, станет недействительной. Поэтому папка берётся заново после push_back.
 */
int FileSystem::createDirectory(const std::string& parentPath, const std::string& name) {
    int userId = currentUserId();
    int dirId = resolve(parentPath);
    checkCanCreate(dirId, name, userId);
    reserveSpace(userId, DataSize::fromKB(4));
    int id = nextEntryId_++;
    directories_.push_back(Directory(id, name, dirId, userId, Permission::forDirectory(), now_));
    directory(dirId).addChild(id);
    audit_.record(now_, userId, "mkdir", pathOf(id), true);
    return id;
}

int FileSystem::createTextFile(const std::string& dirPath, const std::string& name, const std::string& content) {
    int userId = currentUserId();
    int dirId = resolve(dirPath);
    checkCanCreate(dirId, name, userId);
    reserveSpace(userId, DataSize((long long)content.size()));
    int id = nextEntryId_++;
    textFiles_.push_back(TextFile(id, name, dirId, userId, Permission::forFile(), now_, content));
    directory(dirId).addChild(id);
    versions_.push_back(VersionHistory(id, 10));
    versions_.back().addVersion(content, userId, now_, "Создан");
    audit_.record(now_, userId, "create", pathOf(id), true);
    if (hasQuota(userId)) notifier_.quotaWarning(quotaOwner(userId), now_);
    return id;
}

/** @brief Объект создаётся до занятия места: конструктор проверит размеры, а размер файла станет известен. */
int FileSystem::createImage(const std::string& dirPath, const std::string& name, int w, int h, const std::string& format) {
    int userId = currentUserId();
    int dirId = resolve(dirPath);
    checkCanCreate(dirId, name, userId);
    ImageFile image(nextEntryId_, name, dirId, userId, Permission::forFile(), now_, w, h, format);
    reserveSpace(userId, image.size());
    ++nextEntryId_;
    images_.push_back(image);
    directory(dirId).addChild(image.getId());
    audit_.record(now_, userId, "create", pathOf(image.getId()), true, image.preview(0));
    if (hasQuota(userId)) notifier_.quotaWarning(quotaOwner(userId), now_);
    return image.getId();
}

// ===================== чтение, запись, версии =====================

TextFile& FileSystem::textFile(int id) {
    for (size_t i = 0; i < textFiles_.size(); ++i) if (textFiles_[i].getId() == id) return textFiles_[i];
    throw InvalidOperationException(pathOf(id) + " — не текстовый файл");
}

VersionHistory& FileSystem::historyOf(int fileId) {
    for (size_t i = 0; i < versions_.size(); ++i) if (versions_[i].getFileId() == fileId) return versions_[i];
    throw InvalidOperationException("у " + pathOf(fileId) + " нет истории версий");
}

int FileSystem::foreignLockOwner(int entryId, int userId, bool exclusiveOnly) const {
    const std::vector<FileLock>& all = locks_.getLocks();
    for (size_t i = 0; i < all.size(); ++i) {
        const FileLock& l = all[i];
        if (l.getEntryId() != entryId || l.isHeldBy(userId) || l.isExpired(now_)) continue;
        if (!exclusiveOnly || l.getMode() == FileLock::LockMode::Exclusive) return l.getUserId();
    }
    return 0;
}

/** @brief Exclusive — «пишу, больше никому нельзя», поэтому чужая Exclusive-блокировка запрещает и чтение. */
std::string FileSystem::readText(const std::string& path) {
    int userId = currentUserId();
    int id = followLink(resolve(path));
    checkAccess(id, Permission::Access::Read, userId);
    const std::string& content = textFile(id).getContent();
    int holder = foreignLockOwner(id, userId, true);
    if (holder != 0) throw FileLockedException(pathOf(id), holder);
    audit_.record(now_, userId, "read", pathOf(id), true);
    return content;
}

/**
 * @brief Квоту меняет владелец файла, а не тот, кто пишет: если гость по ACL допишет в файл ivan,
 * растёт место ivan. Ссылка f остаётся верной: между её получением и write векторы файлов не меняются.
 */
void FileSystem::writeContent(int fileId, const std::string& content, int userId, const std::string& comment) {
    int holder = foreignLockOwner(fileId, userId, false);
    if (holder != 0) throw FileLockedException(pathOf(fileId), holder);
    TextFile& f = textFile(fileId);
    int owner = f.getOwnerId();
    DataSize oldSize = f.size();
    DataSize newSize((long long)content.size());
    if (oldSize < newSize) reserveSpace(owner, newSize - oldSize);
    else if (newSize < oldSize) freeSpace(owner, oldSize - newSize);
    f.write(content, now_);
    int version = historyOf(fileId).addVersion(content, userId, now_, comment);
    audit_.record(now_, userId, "write", pathOf(fileId), true, comment + ", v" + std::to_string(version));
    if (hasQuota(owner)) notifier_.quotaWarning(quotaOwner(owner), now_);
}

void FileSystem::writeText(const std::string& path, const std::string& content) {
    int userId = currentUserId();
    int id = followLink(resolve(path));
    checkAccess(id, Permission::Access::Write, userId);
    textFile(id);
    writeContent(id, content, userId, "Изменён");
}

/**
 * @brief Как git revert: старое содержимое становится новой версией, версии после неё остаются.
 * История знает только ID файла, поэтому путь в CorruptedFileException подставляется здесь.
 */
void FileSystem::rollback(const std::string& path, int version) {
    int userId = currentUserId();
    int id = followLink(resolve(path));
    checkAccess(id, Permission::Access::Write, userId);
    textFile(id);
    std::string content;
    try {
        content = historyOf(id).rollbackTo(version);
    } catch (const CorruptedFileException&) {
        audit_.record(now_, userId, "rollback", pathOf(id), false, "версия " + std::to_string(version) + " повреждена");
        throw CorruptedFileException(pathOf(id) + ", версия " + std::to_string(version));
    }
    writeContent(id, content, userId, "Откат к v" + std::to_string(version));
}

const VersionHistory& FileSystem::versionsOf(const std::string& path) {
    int userId = currentUserId();
    int id = followLink(resolve(path));
    checkAccess(id, Permission::Access::Read, userId);
    textFile(id);
    return historyOf(id);
}

// ===================== переименование, перемещение, копирование, права =====================

/** @brief Если идти от id вверх и встретить rootId, id внутри него. Глубина ограничена на случай испорченных данных. */
bool FileSystem::isInsideSubtree(int id, int rootId) const {
    int current = id;
    for (int depth = 0; current != 0 && depth < 1000; ++depth) {
        if (current == rootId) return true;
        current = entry(current).getParentId();
    }
    return false;
}

/** @brief Папки в /home создаёт только система, поэтому каждая из них — чья-то домашняя. */
bool FileSystem::isProtected(int id) const {
    if (id == ROOT_ID || id == HOME_ID) return true;
    const FsEntry& e = entry(id);
    return e.isDirectory() && e.getParentId() == HOME_ID;
}

/**
 * @brief Ссылка хранит и ID цели, и путь строкой. После rename или move ID остаётся верным,
 * а путь устаревает, поэтому он пересчитывается у всех ссылок на элемент или его содержимое.
 */
void FileSystem::refreshLinks(int movedId) {
    for (size_t i = 0; i < symlinks_.size(); ++i) {
        if (symlinks_[i].isBroken()) continue;
        int target = symlinks_[i].getTargetId();
        try {
            if (isInsideSubtree(target, movedId)) symlinks_[i].retarget(target, pathOf(target), now_);
        } catch (const EntryNotFoundException&) {
            symlinks_[i].markBroken();   // цели уже нет
        }
    }
}

void FileSystem::rename(const std::string& path, const std::string& newName) {
    int userId = currentUserId();
    int id = resolve(path);
    if (isProtected(id)) throw InvalidOperationException("системную папку " + path + " нельзя переименовать");
    int parent = entry(id).getParentId();
    checkAccess(parent, Permission::Access::Write, userId);
    if (!Path::isValidName(newName)) throw InvalidPathException("недопустимое имя \"" + newName + "\"");
    if (newName == entry(id).getName()) return;
    if (findChild(parent, newName) != 0) throw EntryAlreadyExistsException(childPath(parent, newName));
    int holder = foreignLockOwner(id, userId, false);
    if (holder != 0) throw FileLockedException(pathOf(id), holder);
    std::string oldPath = pathOf(id);
    entry(id).rename(newName, now_);
    refreshLinks(id);
    audit_.record(now_, userId, "rename", oldPath, true, "-> " + pathOf(id));
}

/**
 * @brief Все проверки до removeChild: иначе при ошибке элемент повис бы «нигде».
 * Папку нельзя положить внутрь неё самой: она стала бы своим потомком, выпала из дерева,
 * а pathOf и directorySize ушли бы в бесконечный цикл.
 */
void FileSystem::move(const std::string& path, const std::string& newParentPath) {
    int userId = currentUserId();
    int id = resolve(path);
    int dest = resolve(newParentPath);
    if (isProtected(id)) throw InvalidOperationException("системную папку " + path + " нельзя переместить");
    if (!entry(dest).isDirectory()) throw InvalidPathException(newParentPath + " (не папка)");
    if (isInsideSubtree(dest, id))
        throw InvalidOperationException("нельзя переместить " + path + " внутрь самой себя");
    int oldParent = entry(id).getParentId();
    if (dest == oldParent) return;
    checkAccess(oldParent, Permission::Access::Write, userId);
    checkAccess(dest, Permission::Access::Write, userId);
    const std::string& name = entry(id).getName();
    if (findChild(dest, name) != 0) throw EntryAlreadyExistsException(childPath(dest, name));
    if (directory(dest).isFull()) throw InvalidOperationException("папка " + newParentPath + " заполнена");
    int holder = foreignLockOwner(id, userId, false);
    if (holder != 0) throw FileLockedException(pathOf(id), holder);
    std::string oldPath = pathOf(id);
    directory(oldParent).removeChild(id);
    directory(dest).addChild(id);
    entry(id).moveTo(dest, now_);
    refreshLinks(id);
    audit_.record(now_, userId, "move", oldPath, true, "-> " + pathOf(id));
}

/** @brief Пометка вставляется перед расширением; у ".bashrc" точка в начале, расширения нет. */
std::string FileSystem::freeName(int dirId, const std::string& name, const std::string& mark) const {
    if (findChild(dirId, name) == 0) return name;
    size_t dot = name.rfind('.');
    std::string base = name, ext;
    if (dot != std::string::npos && dot > 0) {
        base = name.substr(0, dot);
        ext = name.substr(dot);
    }
    for (int n = 1;; ++n) {
        std::string candidate = base + " (" + mark + (n == 1 ? "" : " " + std::to_string(n)) + ")" + ext;
        if (findChild(dirId, candidate) == 0) return candidate;
    }
}

/**
 * @brief У готового объекта нельзя сменить ID и владельца, поэтому для каждого типа файла создаётся
 * новый объект из геттеров исходника. Данные копируются в локальный объект до push_back:
 * после push_back ссылка на элемент того же вектора могла бы стать недействительной.
 */
int FileSystem::copy(const std::string& path, const std::string& newParentPath) {
    int userId = currentUserId();
    int src = followLink(resolve(path));
    if (entry(src).typeChar() != '-') throw InvalidOperationException("копировать можно только файлы, а " + path + " — нет");
    checkAccess(src, Permission::Access::Read, userId);
    int holder = foreignLockOwner(src, userId, true);
    if (holder != 0) throw FileLockedException(pathOf(src), holder);
    int dest = resolve(newParentPath);
    std::string name = freeName(dest, entry(src).getName(), "копия");
    checkCanCreate(dest, name, userId);
    reserveSpace(userId, entry(src).size());
    int id = nextEntryId_++;
    std::string srcPath = pathOf(src);
    const Permission perm = entry(src).getPermission();
    for (size_t i = 0; i < textFiles_.size(); ++i) {
        if (textFiles_[i].getId() != src) continue;
        TextFile c(id, name, dest, userId, perm, now_, textFiles_[i].getContent());
        c.getMetadata() = textFiles_[i].getMetadata();
        textFiles_.push_back(c);
        versions_.push_back(VersionHistory(id, 10));
        versions_.back().addVersion(c.getContent(), userId, now_, "Копия " + srcPath);
        break;
    }
    for (size_t i = 0; i < images_.size(); ++i) {
        if (images_[i].getId() != src) continue;
        const ImageFile& s = images_[i];
        ImageFile c(id, name, dest, userId, perm, now_, s.getWidth(), s.getHeight(), s.getFormat(), s.getColorDepth());
        c.getMetadata() = s.getMetadata();
        images_.push_back(c);
        break;
    }
    for (size_t i = 0; i < executables_.size(); ++i) {
        if (executables_[i].getId() != src) continue;
        const ExecutableFile& s = executables_[i];
        ExecutableFile c(id, name, dest, userId, perm, now_, s.getPlatform(), s.getVersion(), s.size(), s.isSigned());
        c.getMetadata() = s.getMetadata();
        executables_.push_back(c);
        break;
    }
    for (size_t i = 0; i < archives_.size(); ++i) {
        if (archives_[i].getId() != src) continue;
        ArchiveFile c(id, name, dest, userId, perm, now_, archives_[i].getFormat());
        const std::vector<ArchiveEntry>& entries = archives_[i].getEntries();
        for (size_t j = 0; j < entries.size(); ++j) c.addEntry(entries[j], now_);
        c.getMetadata() = archives_[i].getMetadata();
        archives_.push_back(c);
        break;
    }
    directory(dest).addChild(id);
    audit_.record(now_, userId, "copy", pathOf(id), true, "из " + srcPath);
    if (hasQuota(userId)) notifier_.quotaWarning(quotaOwner(userId), now_);
    return id;
}

/** @brief Как в Unix: права меняет владелец или администратор (аналог root). */
void FileSystem::chmod(const std::string& path, const std::string& rwx) {
    int userId = currentUserId();
    int id = resolve(path);
    Permission p = Permission::fromString(rwx);
    const Account& acc = account(userId);
    if (entry(id).getOwnerId() != userId && !acc.canManageUsers()) {
        audit_.record(now_, userId, "chmod", path, false, "не владелец");
        throw AccessDeniedException(acc.getLogin(), "менять права " + path + " (не владелец)");
    }
    std::string oldRights = entry(id).getPermission().toString();
    entry(id).chmod(p);
    audit_.record(now_, userId, "chmod", pathOf(id), true, oldRights + " -> " + p.toString());
}

// ===================== корзина =====================

/** @brief Элемент из корзины убран из childIds родителя, поэтому проверка «родитель числит его ребёнком» его отсекает. */
bool FileSystem::isInTree(int id) const {
    int current = id;
    for (int depth = 0; current != ROOT_ID && depth < 1000; ++depth) {
        int parent;
        try {
            parent = entry(current).getParentId();
            if (parent == 0 || !entry(parent).isDirectory() || !directory(parent).hasChild(current)) return false;
        } catch (const FileSystemException&) {
            return false;
        }
        current = parent;
    }
    return current == ROOT_ID;
}

/**
 * @brief Окончательное удаление: элемент стирается из своего вектора, его место освобождается у владельца,
 * вместе с ним уходят версии, ACL и блокировки. Ссылки на него становятся битыми.
 */
void FileSystem::deleteForever(int id) {
    int owner = entry(id).getOwnerId();
    DataSize size = entry(id).size();
    bool erased = false;
    for (size_t i = 0; i < directories_.size() && !erased; ++i)
        if (directories_[i].getId() == id) { directories_.erase(directories_.begin() + i); erased = true; }
    for (size_t i = 0; i < textFiles_.size() && !erased; ++i)
        if (textFiles_[i].getId() == id) { textFiles_.erase(textFiles_.begin() + i); erased = true; }
    for (size_t i = 0; i < images_.size() && !erased; ++i)
        if (images_[i].getId() == id) { images_.erase(images_.begin() + i); erased = true; }
    for (size_t i = 0; i < executables_.size() && !erased; ++i)
        if (executables_[i].getId() == id) { executables_.erase(executables_.begin() + i); erased = true; }
    for (size_t i = 0; i < archives_.size() && !erased; ++i)
        if (archives_[i].getId() == id) { archives_.erase(archives_.begin() + i); erased = true; }
    for (size_t i = 0; i < symlinks_.size() && !erased; ++i)
        if (symlinks_[i].getId() == id) { symlinks_.erase(symlinks_.begin() + i); erased = true; }
    freeSpace(owner, size);
    for (size_t i = 0; i < versions_.size(); ++i)
        if (versions_[i].getFileId() == id) { versions_.erase(versions_.begin() + i); break; }
    for (size_t i = 0; i < acls_.size(); ++i)
        if (acls_[i].getEntryId() == id) { acls_.erase(acls_.begin() + i); break; }
    locks_.releaseAllFor(id);
    shares_.revokeAllFor(id);
    for (size_t i = 0; i < symlinks_.size(); ++i)
        if (symlinks_[i].getTargetId() == id) symlinks_[i].markBroken();
}

/**
 * @brief Элемент убирается из папки, но остаётся в своём векторе и сохраняет parentId — поэтому его можно вернуть.
 * RecycleBin::add возвращает ID вытесненных записей: без их окончательного удаления место бы «утекло».
 */
void FileSystem::remove(const std::string& path) {
    int userId = currentUserId();
    int id = resolve(path);
    if (isProtected(id)) throw InvalidOperationException("системную папку " + path + " нельзя удалить");
    if (entry(id).isDirectory() && !directory(id).isEmpty()) throw DirectoryNotEmptyException(path);
    int parent = entry(id).getParentId();
    checkAccess(parent, Permission::Access::Write, userId);
    int holder = foreignLockOwner(id, userId, false);
    if (holder != 0) throw FileLockedException(pathOf(id), holder);
    std::string fullPath = pathOf(id);
    TrashItem item(id, parent, fullPath, userId, entry(id).size(), now_, trash_.getRetentionDays());
    directory(parent).removeChild(id);
    shares_.revokeAllFor(id);
    locks_.releaseAllFor(id);
    std::vector<int> evicted;
    try {
        evicted = trash_.add(item);
    } catch (const InvalidOperationException&) {
        deleteForever(id);
        audit_.record(now_, userId, "delete", fullPath, true, "больше корзины, удалён сразу");
        return;
    }
    for (size_t i = 0; i < evicted.size(); ++i) deleteForever(evicted[i]);
    audit_.record(now_, userId, "delete", fullPath, true,
                  evicted.empty() ? "в корзину" : "в корзину, вытеснено старых: " + std::to_string(evicted.size()));
}

/**
 * @brief Все проверки и выбор места — до trash_.restore: иначе запись исчезла бы из корзины,
 * а положить элемент было бы некуда.
 */
void FileSystem::restore(int entryId) {
    int userId = currentUserId();
    const std::vector<TrashItem>& items = trash_.getItems();
    int index = -1;
    for (size_t i = 0; i < items.size(); ++i)
        if (items[i].getEntryId() == entryId) index = (int)i;
    if (index < 0) throw EntryNotFoundException("#" + std::to_string(entryId) + " в корзине");
    std::string originalPath = items[index].getOriginalPath();
    int deletedBy = items[index].getDeletedBy();
    int dest = items[index].getOriginalParentId();
    int owner = entry(entryId).getOwnerId();
    const Account& acc = account(userId);
    if (deletedBy != userId && owner != userId && !acc.canManageUsers()) {
        audit_.record(now_, userId, "restore", originalPath, false, "не удалял и не владелец");
        throw AccessDeniedException(acc.getLogin(), "восстановить " + originalPath);
    }
    if (!isInTree(dest) || directory(dest).isFull()) {
        dest = ROOT_ID;
        if (hasQuota(owner)) {
            int home = quotaOwner(owner).getHomeDirId();
            if (isInTree(home) && !directory(home).isFull()) dest = home;
        }
    }
    std::string name = freeName(dest, entry(entryId).getName(), "восстановлен");
    trash_.restore(entryId);
    directory(dest).addChild(entryId);
    if (entry(entryId).getParentId() != dest) entry(entryId).moveTo(dest, now_);
    if (entry(entryId).getName() != name) entry(entryId).rename(name, now_);
    refreshLinks(entryId);
    audit_.record(now_, userId, "restore", pathOf(entryId), true, "было " + originalPath);
}

/** @brief Записи сначала убираются из корзины, потом сами объекты удаляются навсегда. */
int FileSystem::emptyTrash(bool onlyExpired) {
    int userId = currentUserId();
    std::vector<int> ids;
    if (onlyExpired) {
        ids = trash_.purgeExpired(now_);
    } else {
        bool admin = account(userId).canManageUsers();
        const std::vector<TrashItem>& items = trash_.getItems();
        for (size_t i = 0; i < items.size(); ++i)
            if (admin || items[i].getDeletedBy() == userId) ids.push_back(items[i].getEntryId());
        for (size_t i = 0; i < ids.size(); ++i) trash_.restore(ids[i]);
    }
    for (size_t i = 0; i < ids.size(); ++i) deleteForever(ids[i]);
    audit_.record(now_, userId, "purge", "корзина", true, "удалено навсегда: " + std::to_string(ids.size()));
    return (int)ids.size();
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
