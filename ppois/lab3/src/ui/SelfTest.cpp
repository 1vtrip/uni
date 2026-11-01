#include "ui/SelfTest.h"
#include <iostream>
#include <sstream>
#include <type_traits>
#include "accounts/SuperUser.h"
#include "core/Checksum.h"
#include "core/Path.h"
#include "exceptions/Exceptions.h"
#include "fs/ArchiveFile.h"
#include "fs/Directory.h"
#include "fs/ImageFile.h"
#include "fs/TextFile.h"
#include "ops/LockManager.h"
#include "ops/SearchQuery.h"
#include "service/ReportGenerator.h"
#include "storage/Quota.h"
#include "storage/RecycleBin.h"
#include "storage/VersionHistory.h"
#include "storage/Volume.h"
#include "ui/DemoData.h"

using std::cout;
using A = Permission::Access;

static int passed = 0;
static int failed = 0;

/** @brief Одна проверка: вывести результат и посчитать его. */
static void check(const std::string& name, bool ok) {
    cout << (ok ? "  [OK]   " : "  [FAIL] ") << name << "\n";
    if (ok) ++passed; else ++failed;
}

/** @brief Заголовок группы проверок. */
static void section(const std::string& title) { cout << "\n" << title << "\n"; }

/** @brief Строка «как ls -l» и preview: вызывается через const FsEntry&, поэтому у каждого типа своя версия. */
static void showEntry(const FsEntry& e) { cout << "    " << e << "  | " << e.preview(30) << "\n"; }

/** @brief Роль, длина сессии и права — через const Account&, тоже виртуально. */
static void showRights(const Account& a) {
    cout << "    " << a.role() << ": сессия " << a.maxSessionMinutes() << " мин, читать всё — "
         << (a.canReadAnyFile() ? "да" : "нет") << "\n";
}

/** @brief Вывод любого IPrintable в строку. */
static std::string str(const IPrintable& p) {
    std::ostringstream out;
    out << p;
    return out.str();
}

static void testCore() {
    section("Этап 1. core и исключения");
    check("DataSize::fromMB(1.5) выводится как 1.5 MB", DataSize::fromMB(1.5).toString() == "1.5 MB");
    Path p = Path::parse("/home/./ivan/../ivan/a.TXT");
    check("Path::parse(\"/home/./ivan/../ivan/a.TXT\") = /home/ivan/a.TXT", p.toString() == "/home/ivan/a.TXT");
    check("расширение a.TXT — txt", p.extension() == "txt");
    check("две одинаковые строки дают одинаковый Checksum", Checksum::of("abc") == Checksum::of("abc"));
    bool thrown = false;
    try { Path::parse("/.."); } catch (const InvalidPathException&) { thrown = true; }
    check("\"..\" выше корня -> InvalidPathException", thrown);
}

static void testMeta() {
    section("Этап 2. meta");
    check("Permission::fromString(\"rwxr-x---\").toOctal() == 0750", Permission::fromString("rwxr-x---").toOctal() == 0750);
    check("гостю нельзя читать файл с правами forPrivate()", !Permission::forPrivate().allows(A::Read, false, false));
    check("MimeType::fromExtension(\"PNG\") — изображение", MimeType::fromExtension("PNG").isImage());
}

static void testFs() {
    section("Этап 3. fs: полиморфизм через const FsEntry&");
    Timestamp t(1, 11, 2026, 9, 0);
    TextFile text(10, "notes.txt", 2, 7, Permission::forFile(), t, "hello world");
    ImageFile image(11, "pic.jpg", 2, 7, Permission::forFile(), t, 1920, 1080, "jpg");
    ArchiveFile archive(12, "a.zip", 2, 7, Permission::forFile(), t);
    archive.addEntry(ArchiveEntry("notes.txt", DataSize(1000), DataSize(300)), t);
    Directory dir(13, "docs", 1, 7, Permission::forDirectory(), t);
    showEntry(text);
    showEntry(image);
    showEntry(archive);
    showEntry(dir);
    check("у текста, картинки, архива и папки разный size()",
          text.size() != image.size() && image.size() != archive.size() && archive.size() != dir.size());
    check("FsEntry и File абстрактные (создать нельзя)", std::is_abstract<FsEntry>::value && std::is_abstract<File>::value);
}

static void testStorage() {
    section("Этап 4. storage, versions, trash");
    Timestamp t(1, 11, 2026, 9, 0);
    Quota q(7, DataSize::fromMB(100));
    q.consume(DataSize::fromMB(60));
    bool thrown = false;
    try { q.consume(DataSize::fromMB(60)); } catch (const QuotaExceededException&) { thrown = true; }
    check("квота 100 МБ: второй раз по 60 МБ -> QuotaExceededException", thrown && q.getUsed() == DataSize::fromMB(60));
    VersionHistory h(1);
    h.addVersion("v1", 7, t, ""); h.addVersion("v2", 7, t, ""); h.addVersion("v3", 7, t, "");
    check("три версии и откат ко второй", h.count() == 3 && h.rollbackTo(2) == "v2");
    RecycleBin bin(30);
    bin.add(TrashItem(5, 2, "/a.txt", 7, DataSize(10), t, 30));
    std::vector<int> purged = bin.purgeExpired(t.addDays(31));
    check("файл в корзине со сроком 30 дней пропадает через 31 день", purged.size() == 1 && bin.isEmpty());
}

static void testAccounts() {
    section("Этап 5. accounts и security: права через const Account&");
    Timestamp t(1, 11, 2026, 9, 0);
    UserAccount u(1, "ivan", "Иван", Credentials::create("ivan", "Secret123"), t, 3);
    GuestAccount g(2, "guest", "Гость", Credentials::create("guest", "Visit1234"), t, 4, t.addDays(7));
    AdminAccount a(3, "root", "Админ", Credentials::create("root", "Admin1234"), t, 2);
    ServiceAccount s(4, "backup", "Сервис", Credentials::create("backup", "x"), t, "backup");
    s.allow("backup");
    showRights(u);
    showRights(g);
    showRights(a);
    showRights(s);
    check("сессия: пользователь 60, гость 30, админ 240, служебная 1440",
          u.maxSessionMinutes() == 60 && g.maxSessionMinutes() == 30 && a.maxSessionMinutes() == 240 && s.maxSessionMinutes() == 1440);
    for (int i = 0; i < 5; ++i) u.checkPassword("wrong");
    check("пять неверных паролей блокируют учётную запись", u.isBlocked() && !u.checkPassword("Secret123"));
    bool thrown = false;
    try { PasswordPolicy().validate("12345", "ivan"); } catch (const WeakPasswordException&) { thrown = true; }
    check("слабый пароль \"12345\" не проходит политику", thrown);
    g.resetQuota();
    check("квота гостя после resetQuota() — 50 MB (виртуальный вызов после конструктора)",
          g.getQuota().getLimit() == DataSize::fromMB(50));
}

static void testOps() {
    section("Этап 6. блокировки, поиск, ссылки");
    Timestamp t(1, 11, 2026, 9, 0);
    LockManager lm;
    lm.acquire(12, 1, FileLock::LockMode::Shared, t, 30);
    lm.acquire(12, 2, FileLock::LockMode::Shared, t, 30);
    bool thrown = false;
    try { lm.acquire(12, 3, FileLock::LockMode::Exclusive, t, 30); } catch (const FileLockedException&) { thrown = true; }
    check("двое берут Shared — ок, третий Exclusive — FileLockedException", thrown);
    check("wildcardMatch(\"*.tx?\", \"notes.txt\")", SearchQuery::wildcardMatch("*.tx?", "notes.txt"));
    ShareLink link("t", 12, 1, A::Read, t, 7, 2);
    link.use(t); link.use(t);
    thrown = false;
    try { link.use(t); } catch (const ShareLinkExpiredException&) { thrown = true; }
    check("ссылка с maxUses = 2 на третьем использовании -> ShareLinkExpiredException", thrown);
}

/** @brief Сценарий из раздела «Предметная область» плана, через методы FileSystem. */
static void testScenario() {
    section("Этап 7. основной сценарий через FileSystem");
    FileSystem fs = createDemoFileSystem();
    fs.login("ivan", "Secret123");
    fs.createDirectory("/home/ivan", "lab");
    fs.createTextFile("/home/ivan/lab", "task.txt", "версия 1");
    fs.writeText("/home/ivan/lab/task.txt", "версия 2");
    fs.rollback("/home/ivan/lab/task.txt", 1);
    check("создать папку и файл, изменить, откатить", fs.readText("/home/ivan/lab/task.txt") == "версия 1");
    std::string token = fs.share("/home/ivan/lab/task.txt", 7, 0);
    fs.logout();
    check("дать ссылку и открыть её без входа", fs.readShared(token) == "версия 1");
    fs.login("ivan", "Secret123");
    SearchQuery q; q.byName("*.txt");
    check("найти по *.txt", fs.search(q).count() >= 3);
    fs.remove("/home/ivan/lab/task.txt");
    int id = fs.getTrash().getItems().back().getEntryId();
    fs.restore(id);
    check("удалить в корзину и восстановить", fs.pathOf(id) == "/home/ivan/lab/task.txt");
    fs.login("root", "Admin1234");
    int backups = fs.getBackups().count();
    fs.runBackup();
    check("резервная копия", fs.getBackups().count() == backups + 1 || fs.getBackups().count() == fs.getBackups().getPolicy().getKeepCount());
    ReportGenerator r(fs.now().addDays(-1), fs.now());
    check("отчёт о занятом месте не пустой", r.diskUsage(fs).find("Домашние папки") != std::string::npos);
    fs.login("guest", "Visit1234");
    bool denied = false, quota = false;
    try { fs.createTextFile("/home/ivan", "hack.txt", "x"); } catch (const AccessDeniedException&) { denied = true; }
    try { fs.createTextFile("/home/guest", "big", std::string(51 * 1024 * 1024, 'x')); } catch (const QuotaExceededException&) { quota = true; }
    check("гость пишет в чужую папку -> AccessDeniedException", denied);
    check("превышение квоты гостя -> QuotaExceededException", quota);
}

static void testDiamond() {
    section("Этап 8. ромб и интерфейсы");
    Timestamp t(1, 11, 2026, 9, 0);
    SuperUser su(9, "boss", "Начальник", Credentials::create("boss", "Chief12345"), t, 20);
    su.resetQuota();
    const UserAccount& asUser = su;
    const AdminAccount& asAdmin = su;
    check("SuperUser через const UserAccount& и const AdminAccount&: getLogin() без неоднозначности",
          asUser.getLogin() == "boss" && asAdmin.getLogin() == "boss");
    const Account& viaUser = asUser;
    const Account& viaAdmin = asAdmin;
    check("Account в SuperUser один (виртуальное наследование)", &viaUser == &viaAdmin);
    check("квота 100 GB и уровень 3", su.getQuota().getLimit() == DataSize::fromGB(100) && su.getAdminLevel() == 3);
    check("operator<< из IPrintable печатает версию SuperUser", str(su).find("Суперпользователь") != std::string::npos);
    ReportGenerator r(t, t.addMinutes(60));
    Session s(1, 9, t, 30);
    ShareLink link("abc", 1, 9, A::Read, t, 7, 0);
    check("expiryStatus(сессия) — истёк, expiryStatus(ссылка) — действует",
          r.expiryStatus(s, "s", t.addMinutes(60)).find("истёк") != std::string::npos
              && r.expiryStatus(link, "l", t.addMinutes(60)).find("действует") != std::string::npos);
}

/** @brief Каждое из 18 исключений должно бросаться хотя бы в одном месте (требование перед сдачей). */
static void testAllExceptions() {
    section("Все 18 исключений бросаются");
    FileSystem fs = createDemoFileSystem();
    int count = 0;
    try { Permission::fromString("rw"); } catch (const FileSystemException&) { ++count; }
    try { fs.account(999999); } catch (const EntryNotFoundException&) { ++count; }
    try { fs.resolve("/нет"); } catch (const InvalidPathException&) { ++count; }
    try { fs.addUser("ivan", "x", "Secret123"); } catch (const EntryAlreadyExistsException&) { ++count; }
    fs.login("ivan", "Secret123");
    try { fs.remove("/home/ivan/docs"); } catch (const DirectoryNotEmptyException&) { ++count; }
    try { fs.move("/home/ivan/docs", "/home/ivan/docs"); } catch (const InvalidOperationException&) { ++count; }
    try { fs.rollback("/home/ivan/docs/notes.txt", 999); } catch (const VersionNotFoundException&) { ++count; }
    std::string token = fs.share("/home/ivan/docs/notes.txt", 1, 1);
    fs.readShared(token);
    try { fs.readShared(token); } catch (const ShareLinkExpiredException&) { ++count; }
    fs.simulateCorruption("/home/ivan/docs/report.md");
    try { fs.verifyIntegrity("/home/ivan/docs/report.md"); } catch (const CorruptedFileException&) { ++count; }
    fs.createTextFile("/shared", "draft.txt", "x");
    fs.lock("/shared/draft.txt");
    fs.login("guest", "Visit1234");
    try { fs.createTextFile("/home/ivan", "x", "x"); } catch (const AccessDeniedException&) { ++count; }
    try { fs.createTextFile("/home/guest", "big", std::string(51 * 1024 * 1024, 'x')); } catch (const QuotaExceededException&) { ++count; }
    try { fs.readText("/shared/draft.txt"); } catch (const FileLockedException&) { ++count; }
    fs.advanceTime(31);
    try { fs.currentUserId(); } catch (const SessionExpiredException&) { ++count; }
    try { fs.login("ivan", "wrong"); } catch (const AuthenticationException&) { ++count; }
    try { fs.addUser("newbie", "x", "12345"); } catch (const WeakPasswordException&) { ++count; }
    StorageDevice small(9, "flash", "SSD", DataSize::fromMB(1));
    try { small.allocate(DataSize::fromMB(2)); } catch (const InsufficientStorageException&) { ++count; }
    try { Volume("ro", "/mnt", 9, true).checkWritable(); } catch (const ReadOnlyVolumeException&) { ++count; }
    try { BackupManager().latest(); } catch (const BackupFailedException&) { ++count; }
    check("поймано " + std::to_string(count) + " из 18", count == 18);
}

int runSelfTest() {
    passed = failed = 0;
    cout << "Самопроверка лабораторной №3 «Система управления файлами»\n";
    try {
        testCore();
        testMeta();
        testFs();
        testStorage();
        testAccounts();
        testOps();
        testScenario();
        testDiamond();
        testAllExceptions();
    } catch (const std::exception& e) {
        check(std::string("неожиданное исключение: ") + e.what(), false);
    }
    cout << "\nИтог: пройдено " << passed << ", не пройдено " << failed << "\n";
    return failed;
}
