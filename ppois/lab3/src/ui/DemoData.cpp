#include "ui/DemoData.h"
#include "exceptions/Exceptions.h"
#include <string>
#include <vector>

/**
 * @brief Всё создаётся через обычные методы FileSystem от имени настоящих пользователей,
 * поэтому в журнале аудита сразу есть записи, а квоты посчитаны честно.
 */
FileSystem createDemoFileSystem() {
    FileSystem fs("fms", StorageDevice(1, "disk0", "SSD", DataSize::fromGB(10)), Timestamp(1, 11, 2026, 9, 0));
    fs.addUser("ivan", "Иван Петров", "Secret123");
    fs.addUser("petr", "Пётр Сидоров", "Pass12345");
    fs.addGuest("guest", "Visit1234", 7);
    fs.addAdmin("root", "Администратор", "Admin1234", 2);
    fs.addSuperUser("boss", "Начальник", "Chief12345");

    // администратор: группа и общая папка. Писать в "/" может только система, поэтому, как через sudo,
    // администратор на время открывает "/" на запись, создаёт /shared и закрывает обратно.
    fs.login("root", "Admin1234");
    fs.createGroup("students");
    fs.addToGroup("students", "ivan");
    fs.addToGroup("students", "petr");
    fs.chmod("/", "rwxrwxrwx");
    fs.createDirectory("/", "shared");
    fs.chmod("/", "rwxr-xr-x");
    fs.chmod("/shared", "rwxrwxrwx");
    fs.createTextFile("/shared", "readme.txt", "Общая папка: сюда может писать каждый.\nНе удаляйте чужие файлы.");

    // ivan: текст с несколькими версиями, отчёт, картинка, программа, ссылка, архив
    fs.login("ivan", "Secret123");
    fs.chmod("/home/ivan", "rwxr-x---");                 // группа students может заходить и читать
    fs.createDirectory("/home/ivan", "docs");
    fs.createTextFile("/home/ivan/docs", "notes.txt", "Список дел:\n- сдать лабу");
    fs.advanceTime(30);
    fs.writeText("/home/ivan/docs/notes.txt", "Список дел:\n- сдать лабу\n- подготовиться к защите");
    fs.advanceTime(30);
    fs.writeText("/home/ivan/docs/notes.txt", "Список дел:\n- сдать лабу\n- подготовиться к защите\n- купить кофе");
    fs.createTextFile("/home/ivan/docs", "report.md", "# Отчёт\nСистема управления файлами: 56 классов, 18 исключений.");
    fs.createImage("/home/ivan", "photo.jpg", 1920, 1080, "jpg");
    fs.createExecutable("/home/ivan", "backup-tool", "macOS", "1.10.2", DataSize::fromMB(3), true);
    fs.createSymlink("/home/ivan", "notes-link", "/home/ivan/docs/notes.txt");
    std::vector<std::string> toArchive;
    toArchive.push_back("/home/ivan/docs/notes.txt");
    toArchive.push_back("/home/ivan/docs/report.md");
    fs.archive(toArchive, "/home/ivan", "docs.zip");
    fs.createTextFile("/home/ivan", "old.txt", "Черновик, который больше не нужен");
    fs.remove("/home/ivan/old.txt");                      // в корзине: можно показать восстановление
    fs.setAclRule("/home/ivan/docs/notes.txt", "petr", false, Permission::Access::Write, true);
    fs.share("/home/ivan/docs/report.md", 7, 3);

    // petr: план, картинка, программа для Linux
    fs.login("petr", "Pass12345");
    fs.createTextFile("/home/petr", "plan.txt", "План на неделю:\n1. Лекции\n2. Лабораторные");
    fs.createImage("/home/petr", "avatar.png", 256, 256, "png");
    fs.createExecutable("/home/petr", "game", "Linux", "0.9.1", DataSize::fromMB(12), false);
    fs.createTextFile("/shared", "petr-notes.txt", "Заметки Петра в общей папке");

    // guest: один файл
    fs.login("guest", "Visit1234");
    fs.createTextFile("/home/guest", "hello.txt", "Привет! Я гость.");

    // boss: свой файл и резервная копия всей системы
    fs.login("boss", "Chief12345");
    fs.createTextFile("/home/boss", "budget.txt", "Бюджет отдела: 1000 у.е.");
    fs.runBackup();
    fs.logout();

    // одна неудачная попытка входа — чтобы отчёт о безопасности был не пустым
    try {
        fs.login("petr", "wrong-password");
    } catch (const FileSystemException&) {
    }
    fs.advanceTime(60);
    return fs;
}

std::string demoCredentials() {
    return "Демо-пользователи (логин / пароль):\n"
           "  ivan   / Secret123    — пользователь (группа students)\n"
           "  petr   / Pass12345    — пользователь (группа students)\n"
           "  guest  / Visit1234    — гость на 7 дней, квота 50 MB\n"
           "  root   / Admin1234    — администратор уровня 2\n"
           "  boss   / Chief12345   — суперпользователь (ромб)\n"
           "  backup / Backup2026!  — служебная учётная запись\n";
}
