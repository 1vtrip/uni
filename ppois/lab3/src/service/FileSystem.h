#pragma once
#include <string>
#include <vector>
#include "accounts/AccessControlList.h"
#include "accounts/AdminAccount.h"
#include "accounts/GuestAccount.h"
#include "accounts/ServiceAccount.h"
#include "accounts/SuperUser.h"
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
    /**
     * @brief Суперпользователь: домашняя папка, квота 100 GB и права администратора уровня 3 (ромб).
     * Исключения как у addUser.
     */
    int addSuperUser(const std::string& login, const std::string& name, const std::string& password);
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
    /**
     * @brief Создать папку (rwxr-xr-x), занимает 4 KB квоты. Общий порядок создания: все проверки,
     * потом место, потом push_back и addChild (папка берётся заново после push_back).
     * @throws InvalidPathException / AccessDeniedException / EntryAlreadyExistsException /
     * QuotaExceededException / InsufficientStorageException / InvalidOperationException (папка заполнена).
     */
    int createDirectory(const std::string& parentPath, const std::string& name);
    /** @brief Создать текстовый файл (rw-r--r--) с первой версией "Создан". Исключения как у createDirectory. */
    int createTextFile(const std::string& dirPath, const std::string& name, const std::string& content);
    /** @brief Создать картинку; объект создаётся до занятия места, его конструктор проверяет размеры. */
    int createImage(const std::string& dirPath, const std::string& name, int w, int h, const std::string& format);
    /**
     * @brief Прочитать текст; ссылки раскрываются, как у cat. Нельзя, если чужая Exclusive-блокировка.
     * @throws AccessDeniedException / FileLockedException / InvalidOperationException (не текстовый файл).
     */
    std::string readText(const std::string& path);
    /**
     * @brief Записать текст и создать версию. Разница в размере списывается с квоты владельца файла.
     * @throws AccessDeniedException / FileLockedException (чужая блокировка) / QuotaExceededException.
     */
    void writeText(const std::string& path, const std::string& content);
    /**
     * @brief Откат: содержимое версии записывается как новая версия "Откат к vN"; новые версии не удаляются.
     * @throws VersionNotFoundException / CorruptedFileException и исключения writeText.
     */
    void rollback(const std::string& path, int version);
    /** @brief История версий текстового файла (нужно право чтения). */
    const VersionHistory& versionsOf(const std::string& path);
    /**
     * @brief Переименовать (нужна запись в папку-родителя). Пути ссылок на элемент обновляются.
     * @throws InvalidOperationException системная папка. @throws InvalidPathException / EntryAlreadyExistsException /
     * AccessDeniedException / FileLockedException.
     */
    void rename(const std::string& path, const std::string& newName);
    /**
     * @brief Переместить в другую папку (нужна запись в старую и новую). Пути ссылок обновляются.
     * @throws InvalidOperationException системная папка или перенос папки внутрь самой себя;
     * а также InvalidPathException / EntryAlreadyExistsException / AccessDeniedException / FileLockedException.
     */
    void move(const std::string& path, const std::string& newParentPath);
    /**
     * @brief Скопировать файл (ссылка раскрывается, как у cp); владелец копии и плательщик квоты — копирующий.
     * Занятое имя -> "a (копия).txt", "a (копия 2).txt"... @throws InvalidOperationException не файл.
     */
    int copy(const std::string& path, const std::string& newParentPath);
    /** @brief Сменить права ("rwxr-x---"): владелец или администратор. @throws AccessDeniedException / FileSystemException. */
    void chmod(const std::string& path, const std::string& rwx);
    /**
     * @brief Удалить в корзину (нужна запись в папку-родителя). Квота остаётся занятой до окончательного удаления.
     * Вытесненные из корзины элементы удаляются окончательно; элемент больше всей корзины — сразу, мимо неё.
     * @throws InvalidOperationException системная папка. @throws DirectoryNotEmptyException / AccessDeniedException /
     * FileLockedException.
     */
    void remove(const std::string& path);
    /**
     * @brief Вернуть из корзины: может тот, кто удалил, владелец или администратор. Если исходной папки
     * уже нет или она сама в корзине — в домашнюю папку владельца (иначе в корень). Занятое имя -> "a (восстановлен).txt".
     * @throws EntryNotFoundException элемента нет в корзине. @throws AccessDeniedException.
     */
    void restore(int entryId);
    /**
     * @brief Окончательно удалить из корзины: по умолчанию только истёкшие; onlyExpired = false — все свои
     * (у администратора — все). Вернуть, сколько удалено.
     */
    int emptyTrash(bool onlyExpired = true);
    /**
     * @brief Сжать файлы в новый архив. Архив сначала собирается отдельным объектом (compress принимает
     * const File&, поэтому подходит любой файл), и только потом занимает место и попадает в папку.
     * @throws InvalidOperationException пустой список или не файл. @throws EntryAlreadyExistsException два файла
     * с одним именем. И исключения createTextFile.
     */
    int archive(const std::vector<std::string>& paths, const std::string& dirPath, const std::string& name);
    /**
     * @brief Заблокировать файл на 30 минут: Exclusive требует права записи, Shared — чтения.
     * @throws FileLockedException мешает чужая блокировка.
     */
    void lock(const std::string& path, bool exclusive = true);
    /** @brief Снять свою блокировку. @throws InvalidOperationException её нет. */
    void unlock(const std::string& path);
    /**
     * @brief Ссылка общего доступа на файл; вернуть токен. Пользователю и гостю решает виртуальный canShare(),
     * админу можно, служебной записи нельзя. Нужно то право, которое отдаётся.
     * @throws AccessDeniedException / InvalidOperationException (не файл или неверные параметры ссылки).
     */
    std::string share(const std::string& path, int days, int maxUses, bool allowWrite = false);
    /**
     * @brief Перейти по ссылке (вход не нужен) и вернуть ID файла; создателю уходит уведомление.
     * @throws EntryNotFoundException нет такой ссылки. @throws ShareLinkExpiredException ссылка не действует.
     */
    int openShared(const std::string& token);
    /** @brief Открыть по ссылке и вернуть текст (у не текстового файла — preview). */
    std::string readShared(const std::string& token);
    /** @brief Записать текст по ссылке с правом записи; квоту платит владелец. @throws AccessDeniedException ссылка только для чтения. */
    void writeShared(const std::string& token, const std::string& content);
    /**
     * @brief Поиск по всем элементам. Показывается только то, что стоит в дереве (не в корзине) и что
     * пользователь может читать: иначе поиск выдавал бы имена файлов из чужих закрытых папок.
     */
    SearchResult search(const SearchQuery& q);
    /**
     * @brief Резервная копия всех файлов по политике; запускать может тот, кто читает всё (админ уровня 2+, backup).
     * @throws AccessDeniedException / BackupFailedException (сумма больше предела).
     */
    int runBackup();
    /**
     * @brief Проверить текстовый файл: сумма совпадает с последней версией и все версии целы.
     * @throws CorruptedFileException / InvalidOperationException (не текстовый файл).
     */
    void verifyIntegrity(const std::string& path);
    /** @brief Демонстрация сбоя диска: испортить последнюю версию файла, чтобы verifyIntegrity нашёл повреждение. */
    void simulateCorruption(const std::string& path);

    // доступ для отчётов и меню
    const std::string& getName() const;
    const std::vector<UserAccount>& getUsers() const;
    const std::vector<GuestAccount>& getGuests() const;
    const std::vector<AdminAccount>& getAdmins() const;
    const std::vector<ServiceAccount>& getServices() const;
    const std::vector<SuperUser>& getSuperUsers() const;
    const std::vector<UserGroup>& getGroups() const;
    const AuditLog& getAudit() const;
    const RecycleBin& getTrash() const;
    const NotificationService& getNotifications() const;
    const StorageDevice& getDevice() const;
    const AuthService& getAuth() const;
    const LockManager& getLocks() const;
    const ShareManager& getShares() const;
    const BackupManager& getBackups() const;

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
    std::vector<SuperUser> superUsers_;
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
    /** @brief Почему нельзя (например, "открыть папку /home/ivan"); пусто — можно. Без аудита, поэтому const. */
    std::string denialReason(int entryId, Permission::Access a, int userId) const;
    /** @brief Файл любого типа через ссылку на File (для Compressor). @throws InvalidOperationException не файл. */
    const File& file(int id) const;
    /** @brief Добавить e в результат, если он подходит, стоит в дереве и пользователь может его читать. */
    void addIfVisible(SearchResult& result, const SearchQuery& q, const FsEntry& e, int userId) const;
    /** @brief Положить в ids и sizes элемент e, если он стоит в дереве и политика копирования его берёт. */
    void addIfBackedUp(std::vector<int>& ids, std::vector<DataSize>& sizes, const FsEntry& e) const;
    /**
     * @brief Порядок: canReadAnyFile для чтения -> право x на каждой папке по пути -> ACL -> rwx.
     * Отказ пишется в аудит. @throws AccessDeniedException.
     */
    void checkAccess(int entryId, Permission::Access a, int userId);
    /** @brief Есть ли у учётной записи квота (у пользователя и гостя есть, у админа и служебной нет). */
    bool hasQuota(int userId) const;
    /** @brief Пользователь или гость с квотой, через ссылку на UserAccount. @throws InvalidOperationException квоты нет. */
    UserAccount& quotaOwner(int userId);
    /**
     * @brief Занять место: квота, потом устройство. Если устройство откажет, квота возвращается в catch
     * и исключение бросается дальше. @throws QuotaExceededException / InsufficientStorageException.
     */
    void reserveSpace(int userId, const DataSize& size);
    void freeSpace(int ownerId, const DataSize& size);
    /** @brief Проверки перед созданием элемента name в папке dirId: папка, право записи, имя, повтор, заполненность. */
    void checkCanCreate(int dirId, const std::string& name, int userId);
    /** @brief Путь будущего элемента: "/home/ivan" + "a.txt", у корня без двойного слеша. */
    std::string childPath(int dirId, const std::string& name) const;
    /** @brief ID того, кто держит живую чужую блокировку (только Exclusive, если exclusiveOnly), 0 — нет. */
    int foreignLockOwner(int entryId, int userId, bool exclusiveOnly) const;
    /** @brief Общая часть writeText и rollback: блокировка, квота владельца, запись, версия, аудит. */
    void writeContent(int fileId, const std::string& content, int userId, const std::string& comment);
    /** @brief История версий файла. @throws InvalidOperationException истории нет. */
    VersionHistory& historyOf(int fileId);
    /** @brief Лежит ли id внутри rootId (или это он сам): подъём по parentId. Нужно для move. */
    bool isInsideSubtree(int id, int rootId) const;
    /** @brief "/", "/home" и домашние папки нельзя переименовать, переместить или удалить. */
    bool isProtected(int id) const;
    /** @brief Обновить путь у ссылок, чья цель — movedId или что-то внутри него (после rename и move). */
    void refreshLinks(int movedId);
    /** @brief Свободное имя: name, иначе "a (mark).txt", "a (mark 2).txt"... перед расширением (mark — "копия", "восстановлен"). */
    std::string freeName(int dirId, const std::string& name, const std::string& mark) const;
    /** @brief Стоит ли элемент на своём месте в дереве: каждый предок существует и числит его среди детей. */
    bool isInTree(int id) const;
    /** @brief Удалить навсегда: из вектора, место владельца, версии, ACL, блокировки; ссылки на него становятся битыми. */
    void deleteForever(int id);
};
