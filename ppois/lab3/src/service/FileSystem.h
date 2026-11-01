#pragma once
#include <string>
#include <vector>
#include "accounts/AccessControlList.h"
#include "accounts/AdminAccount.h"
#include "accounts/GuestAccount.h"
#include "accounts/ServiceAccount.h"
#include "accounts/UserAccount.h"
#include "accounts/UserGroup.h"
#include "fs/ArchiveFile.h"
#include "fs/Directory.h"
#include "fs/ExecutableFile.h"
#include "fs/ImageFile.h"
#include "fs/Symlink.h"
#include "fs/TextFile.h"
#include "ops/AuditLog.h"
#include "ops/BackupManager.h"
#include "ops/Compressor.h"
#include "ops/LockManager.h"
#include "ops/SearchQuery.h"
#include "ops/SearchResult.h"
#include "ops/ShareManager.h"
#include "service/AuthService.h"
#include "service/NotificationService.h"
#include "storage/RecycleBin.h"
#include "storage/StorageDevice.h"
#include "storage/VersionHistory.h"

/**
 * @brief Файловая система: владеет всеми объектами и связывает их по ID.
 * Элементы дерева лежат в отдельном векторе на каждый конечный тип, а наружу отдаются
 * ссылкой на базовый класс FsEntry — полиморфизм без указателей.
 */
class FileSystem {
public:
    static const int ROOT_ID = 1;   // корень "/"
    static const int HOME_ID = 2;   // папка "/home"

    /** @brief Создаёт "/", "/home" и служебную учётную запись backup (с операцией "backup"). */
    FileSystem(const std::string& name, const StorageDevice& device, const Timestamp& now);

    // время (модельные часы для демонстрации)
    const Timestamp& now() const;
    /** @brief Перемотать часы вперёд. @throws InvalidOperationException minutes не положительный. */
    void advanceTime(int minutes);

    // учётные записи и вход
    /**
     * @brief Новый пользователь с папкой /home/<login> (rwx------) и квотой; вернуть его ID.
     * Сначала всё проверяется, потом создаётся: при ошибке ничего не остаётся.
     * @throws InvalidPathException логин не годится как имя папки. @throws EntryAlreadyExistsException логин занят.
     * @throws WeakPasswordException пароль не проходит политику.
     */
    int addUser(const std::string& login, const std::string& name, const std::string& password);
    /** @brief Гость на days дней, с папкой и квотой 50 MB. Исключения как у addUser, плюс days <= 0. */
    int addGuest(const std::string& login, const std::string& password, int days);
    /** @brief Администратор без домашней папки. Исключения как у addUser, плюс уровень вне 1–3. */
    int addAdmin(const std::string& login, const std::string& name, const std::string& password, int level);
    /** @brief Учётная запись по ID через ссылку на базовый класс. @throws EntryNotFoundException такой нет. */
    const Account& account(int id) const;
    /** @brief ID учётной записи по логину, 0 — нет такой. */
    int findAccount(const std::string& login) const;
    /**
     * @brief Войти (предыдущая сессия закрывается). Неудача пишется в аудит, исключение бросается дальше.
     * Неизвестный логин даёт то же сообщение, что неверный пароль: по ответу нельзя узнать, какие логины есть.
     * @throws AuthenticationException неверный логин или пароль, запись заблокирована, гостевой срок истёк.
     */
    void login(const std::string& login, const std::string& password);
    /** @brief Выйти; если никто не вошёл, ничего не делает. */
    void logout();
    /** @brief ID вошедшего пользователя, заодно отмечает активность. @throws SessionExpiredException не вошли или сессия истекла. */
    int currentUserId();
    bool isLoggedIn() const;

    // дерево
    /** @brief Элемент по ID как ссылка на базовый класс. @throws EntryNotFoundException такого нет. */
    const FsEntry& entry(int id) const;
    FsEntry& entry(int id);
    /**
     * @brief ID элемента по абсолютному пути. Ссылки в пути не раскрываются (для этого followLink).
     * @throws InvalidPathException путь неверный, относительный, не найден или в середине стоит не папка.
     */
    int resolve(const std::string& path) const;
    /**
     * @brief Пройти цепочку ссылок до настоящего элемента; не ссылка возвращается как есть.
     * @throws EntryNotFoundException ссылка битая. @throws InvalidOperationException цикл ссылок.
     */
    int followLink(int id) const;
    /** @brief Полный путь элемента: "/home/ivan/a.txt". */
    std::string pathOf(int id) const;
    /** @brief ID элементов папки (нужно право чтения папки и x на папках по пути). */
    std::vector<int> list(const std::string& dirPath);
    /** @brief Размер папки со всем содержимым: рекурсия по ID. */
    DataSize directorySize(int dirId) const;

    // операции (проверяют сессию, права, блокировки, квоту и пишут аудит)
    int createDirectory(const std::string& parentPath, const std::string& name);
    int createTextFile(const std::string& dirPath, const std::string& name, const std::string& content);
    int createImage(const std::string& dirPath, const std::string& name, int w, int h, const std::string& format);
    std::string readText(const std::string& path);
    void writeText(const std::string& path, const std::string& content);
    void rollback(const std::string& path, int version);
    void rename(const std::string& path, const std::string& newName);
    void move(const std::string& path, const std::string& newParentPath);
    int copy(const std::string& path, const std::string& newParentPath);
    void chmod(const std::string& path, const std::string& rwx);
    void remove(const std::string& path);
    void restore(int entryId);
    int emptyTrash();
    int archive(const std::vector<std::string>& paths, const std::string& dirPath, const std::string& name);
    void lock(const std::string& path);
    void unlock(const std::string& path);
    std::string share(const std::string& path, int days, int maxUses);
    int openShared(const std::string& token);
    SearchResult search(const SearchQuery& q) const;
    int runBackup();
    void verifyIntegrity(const std::string& path) const;

    // доступ для отчётов и меню
    const std::string& getName() const;
    const std::vector<UserAccount>& getUsers() const;
    const std::vector<GuestAccount>& getGuests() const;
    const std::vector<AdminAccount>& getAdmins() const;
    const std::vector<ServiceAccount>& getServices() const;
    const std::vector<UserGroup>& getGroups() const;
    const AuditLog& getAudit() const;
    const RecycleBin& getTrash() const;
    const NotificationService& getNotifications() const;
    const StorageDevice& getDevice() const;
    const AuthService& getAuth() const;

private:
    std::string name_;
    Timestamp now_;
    // элементы дерева — по вектору на каждый конечный тип
    std::vector<Directory> directories_;
    std::vector<TextFile> textFiles_;
    std::vector<ImageFile> images_;
    std::vector<ExecutableFile> executables_;
    std::vector<ArchiveFile> archives_;
    std::vector<Symlink> symlinks_;
    // учётные записи
    std::vector<UserAccount> users_;
    std::vector<GuestAccount> guests_;
    std::vector<AdminAccount> admins_;
    std::vector<ServiceAccount> services_;
    std::vector<UserGroup> groups_;
    // подсистемы
    StorageDevice device_;
    std::vector<AccessControlList> acls_;
    std::vector<VersionHistory> versions_;
    RecycleBin trash_;
    LockManager locks_;
    AuditLog audit_;
    ShareManager shares_;
    BackupManager backups_;
    AuthService auth_;
    NotificationService notifier_;
    Compressor compressor_;
    int nextEntryId_;
    int nextAccountId_;
    int sessionId_;                                  // 0 = никто не вошёл

    // вспомогательные: учётные записи
    /** @brief Неконстантная учётная запись по ID (для входа: checkPassword меняет счётчик). */
    Account& accountRef(int id);
    /**
     * @brief Проверить логин новой учётной записи: годится как имя папки и свободен.
     * @throws InvalidPathException / EntryAlreadyExistsException.
     */
    void checkNewLogin(const std::string& login) const;
    /** @brief Создать /home/<login> с правами rwx------ и вернуть её ID. */
    int createHome(const std::string& login, int ownerId);

    // вспомогательные: дерево
    /** @brief Папка по ID. @throws InvalidPathException это не папка или такого элемента нет. */
    Directory& directory(int id);
    const Directory& directory(int id) const;
    TextFile& textFile(int id);
    /** @brief ID элемента с именем name в папке dirId, 0 — нет такого. */
    int findChild(int dirId, const std::string& name) const;
    const Symlink& symlink(int id) const;
    /** @brief Есть ли у пользователя общая группа с владельцем: так понимается «группа» в правах rwx. */
    bool sharesGroup(int userId, int ownerId) const;
    /** @brief Решение без админских прав: ACL (запрет важнее всего), иначе rwx. */
    bool allowedByRules(int entryId, Permission::Access a, int userId) const;
    /**
     * @brief Порядок: canReadAnyFile для чтения -> право x на каждой папке по пути -> ACL -> rwx.
     * Отказ пишется в аудит. @throws AccessDeniedException.
     */
    void checkAccess(int entryId, Permission::Access a, int userId);
    void reserveSpace(int userId, const DataSize& size);             // квота + устройство
    void freeSpace(int ownerId, const DataSize& size);
    VersionHistory& historyOf(int fileId);
    bool isInsideSubtree(int id, int rootId) const;                  // для move
};
