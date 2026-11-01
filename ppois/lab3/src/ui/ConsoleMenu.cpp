#include "ui/ConsoleMenu.h"
#include <iostream>
#include <sstream>
#include "core/Path.h"
#include "exceptions/Exceptions.h"
#include "service/ReportGenerator.h"
#include "storage/Volume.h"
#include "ui/Input.h"

using std::cout;
using A = Permission::Access;

/** @brief Строка о результате одного исключения в демонстрации. */
static void showCaught(int n, const std::string& type, const std::exception& e) {
    cout << "  " << (n < 10 ? " " : "") << n << ". " << type << "\n      " << e.what() << "\n";
}

/** @brief "\n", набранное в одной строке, превращается в настоящий перевод строки. */
static std::string unescape(const std::string& s) {
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size() && s[i + 1] == 'n') { result += '\n'; ++i; }
        else result += s[i];
    }
    return result;
}

/** @brief Выбор действия: 1 чтение, 2 запись, 3 выполнение. */
static A readAccess() {
    int a = readInt("Действие (1 — чтение, 2 — запись, 3 — выполнение): ", 1, 3);
    return a == 1 ? A::Read : a == 2 ? A::Write : A::Execute;
}

ConsoleMenu::ConsoleMenu(FileSystem& fs) : fs_(fs), cwd_("/") {}

/**
 * @brief Номер пункта читается вне try: если ввод закрыт, исключение выходит из run() и программа завершается.
 * catch идут от частного к общему: иначе общий FileSystemException перехватил бы всё раньше частных.
 */
void ConsoleMenu::run() {
    while (true) {
        showMainMenu();
        int choice = readInt("Выбор: ", 0, 16);
        if (choice == 0) break;
        try {
            switch (choice) {
                case 1:  accountMenu(); break;
                case 2:  browseMenu(); break;
                case 3:  createMenu(); break;
                case 4:  openDialog(); break;
                case 5:  textMenu(); break;
                case 6:  manageMenu(); break;
                case 7:  trashMenu(); break;
                case 8:  lockMenu(); break;
                case 9:  shareMenu(); break;
                case 10: searchDialog(); break;
                case 11: archiveMenu(); break;
                case 12: adminMenu(); break;
                case 13: reportsMenu(); break;
                case 14: notificationsMenu(); break;
                case 15: timeDialog(); break;
                case 16: exceptionsDemo(); break;
            }
        } catch (const SessionExpiredException& e) {
            cout << "[Сессия] " << e.what() << "\nВойдите заново: пункт 1.\n";
            cwd_ = "/";
        } catch (const AccessDeniedException& e) {
            cout << "[Доступ] " << e.what() << "\n";
        } catch (const QuotaExceededException& e) {
            cout << "[Квота] " << e.what() << "\nУдалите лишнее или очистите корзину: пункт 7.\n";
        } catch (const FileSystemException& e) {
            cout << "[Ошибка] " << e.what() << "\n";
        } catch (const std::exception& e) {
            cout << "[Системная ошибка] " << e.what() << "\n";
        }
    }
}

std::string ConsoleMenu::homeOf(int userId) const {
    for (size_t i = 0; i < fs_.getUsers().size(); ++i)
        if (fs_.getUsers()[i].getId() == userId) return fs_.pathOf(fs_.getUsers()[i].getHomeDirId());
    for (size_t i = 0; i < fs_.getGuests().size(); ++i)
        if (fs_.getGuests()[i].getId() == userId) return fs_.pathOf(fs_.getGuests()[i].getHomeDirId());
    for (size_t i = 0; i < fs_.getSuperUsers().size(); ++i)
        if (fs_.getSuperUsers()[i].getId() == userId) return fs_.pathOf(fs_.getSuperUsers()[i].getHomeDirId());
    return "/";
}

/** @brief peekUserId не отмечает активность: перерисовка меню не должна продлевать сессию. */
std::string ConsoleMenu::promptLine() const {
    std::ostringstream out;
    int userId = fs_.peekUserId();
    if (userId == 0) {
        out << "(вход не выполнен)@" << fs_.getName() << ":" << cwd_ << "  [" << fs_.now() << "]";
        return out.str();
    }
    out << fs_.account(userId).getLogin() << "@" << fs_.getName() << ":" << cwd_ << "  [";
    bool quotaShown = false;
    for (size_t i = 0; i < fs_.getUsers().size() && !quotaShown; ++i)
        if (fs_.getUsers()[i].getId() == userId) { out << "квота " << (int)fs_.getUsers()[i].getQuota().usagePercent() << "% | "; quotaShown = true; }
    for (size_t i = 0; i < fs_.getGuests().size() && !quotaShown; ++i)
        if (fs_.getGuests()[i].getId() == userId) { out << "квота " << (int)fs_.getGuests()[i].getQuota().usagePercent() << "% | "; quotaShown = true; }
    for (size_t i = 0; i < fs_.getSuperUsers().size() && !quotaShown; ++i)
        if (fs_.getSuperUsers()[i].getId() == userId) { out << "квота " << (int)fs_.getSuperUsers()[i].getQuota().usagePercent() << "% | "; quotaShown = true; }
    int unread = (int)fs_.getNotifications().unreadFor(userId).size();
    if (unread > 0) out << "уведомлений " << unread << " | ";
    out << fs_.now() << "]";
    return out.str();
}

void ConsoleMenu::showMainMenu() const {
    cout << "\n" << promptLine() << "\n"
         << " 1. Вход / выход / пароль        9. Ссылки общего доступа\n"
         << " 2. Папки: ls, cd, размер       10. Поиск\n"
         << " 3. Создать                     11. Архив и резервные копии\n"
         << " 4. Открыть: свойства, preview  12. Администрирование\n"
         << " 5. Текст, версии, откат        13. Отчёты\n"
         << " 6. Переименовать, права, ACL   14. Уведомления\n"
         << " 7. Корзина                     15. Перемотать время\n"
         << " 8. Блокировки                  16. Демонстрация исключений\n"
         << " 0. Выход\n";
}

std::string ConsoleMenu::absPath(const std::string& input) const {
    if (input.empty()) return cwd_;
    std::string raw = input[0] == '/' ? input : (cwd_ == "/" ? "/" + input : cwd_ + "/" + input);
    return Path::parse(raw).toString();
}

std::string ConsoleMenu::askPath(const std::string& prompt) const { return absPath(readLine(prompt, true)); }

void ConsoleMenu::printList(const std::vector<int>& ids) const {
    if (ids.empty()) { cout << "  (пусто)\n"; return; }
    for (size_t i = 0; i < ids.size(); ++i) cout << "  " << fs_.entry(ids[i]) << "\n";
}

// ===================== 1. Вход =====================

void ConsoleMenu::accountMenu() {
    cout << "1. Войти   2. Выйти   3. Сменить пароль   0. Назад\n";
    int c = readInt("Выбор: ", 0, 3);
    if (c == 1) {
        std::string login = readLine("Логин: ");
        std::string password = readLine("Пароль: ");
        fs_.login(login, password);
        int userId = fs_.currentUserId();
        cwd_ = homeOf(userId);
        cout << "Добро пожаловать, " << fs_.account(userId).getDisplayName() << " (" << fs_.account(userId).role()
             << "). Сессия: " << fs_.account(userId).maxSessionMinutes() << " мин без активности.\n";
        int unread = (int)fs_.getNotifications().unreadFor(userId).size();
        if (unread > 0) cout << "Непрочитанных уведомлений: " << unread << " (пункт 14).\n";
    } else if (c == 2) {
        fs_.logout();
        cwd_ = "/";
        cout << "Вы вышли.\n";
    } else if (c == 3) {
        std::string oldPassword = readLine("Старый пароль: ");
        std::string newPassword = readLine("Новый пароль: ");
        fs_.changePassword(oldPassword, newPassword);
        cout << "Пароль изменён.\n";
    }
}

// ===================== 2. Папки =====================

void ConsoleMenu::browseMenu() {
    cout << "1. Содержимое папки (ls)   2. Перейти (cd)   3. Размер папки со всем содержимым   0. Назад\n";
    int c = readInt("Выбор: ", 0, 3);
    if (c == 1) {
        std::string path = askPath("Папка (пусто — текущая): ");
        cout << path << ":\n";
        printList(fs_.list(path));
    } else if (c == 2) {
        std::string path = askPath("Куда (.. — вверх, / — корень): ");
        if (!fs_.stat(path).isDirectory()) throw InvalidPathException(path + " (не папка)");
        fs_.list(path);   // проверка, что в папку можно войти и прочитать её
        cwd_ = path;
    } else if (c == 3) {
        std::string path = askPath("Папка (пусто — текущая): ");
        fs_.list(path);
        cout << path << ": " << fs_.directorySize(fs_.resolve(path)) << " (рекурсия по ID, у каждого — свой size())\n";
    }
}

// ===================== 3. Создать =====================

void ConsoleMenu::createMenu() {
    cout << "1. Папка   2. Текстовый файл   3. Изображение   4. Программа   5. Ссылка   0. Назад\n";
    int c = readInt("Выбор: ", 0, 5);
    if (c == 0) return;
    std::string dir = askPath("В какой папке (пусто — текущая): ");
    std::string name = readLine("Имя: ");
    int id = 0;
    if (c == 1) {
        id = fs_.createDirectory(dir, name);
    } else if (c == 2) {
        std::string text = unescape(readLine("Текст (\\n — новая строка): ", true));
        id = fs_.createTextFile(dir, name, text);
    } else if (c == 3) {
        int w = readInt("Ширина: ", 1, 20000);
        int h = readInt("Высота: ", 1, 20000);
        std::string format = readLine("Формат (PNG, JPG, GIF): ");
        id = fs_.createImage(dir, name, w, h, format);
    } else if (c == 4) {
        std::string platform = readLine("Платформа (macOS, Linux, Windows): ");
        std::string version = readLine("Версия (например, 1.10.2): ");
        double mb = readDouble("Размер, MB: ", 0.001, 4096);
        bool isSigned = readYesNo("Подписан? (д/н): ");
        id = fs_.createExecutable(dir, name, platform, version, DataSize::fromMB(mb), isSigned);
    } else if (c == 5) {
        std::string target = askPath("На что ссылка: ");
        id = fs_.createSymlink(dir, name, target);
    }
    cout << "Создано:\n  " << fs_.entry(id) << "\n";
}

// ===================== 4. Открыть =====================

/** @brief typeName, size и preview вызываются через const FsEntry& — у каждого типа своя версия. */
void ConsoleMenu::openDialog() {
    std::string path = askPath("Что открыть: ");
    const FsEntry& e = fs_.stat(path);
    cout << "  " << e << "\n"
         << "  Тип:       " << e.typeName() << "\n"
         << "  Размер:    " << e.size() << "\n"
         << "  Владелец:  " << (e.getOwnerId() == 0 ? std::string("система") : fs_.account(e.getOwnerId()).getLogin()) << "\n"
         << "  Создан:    " << e.getCreated() << ", изменён: " << e.getModified() << "\n"
         << "  Метаданные: " << e.getMetadata() << "\n"
         << "  Preview:   " << e.preview(60) << "\n";
    if (e.isDirectory()) cout << "  С содержимым: " << fs_.directorySize(e.getId()) << "\n";
    if (e.typeChar() == 'l') cout << "  Ведёт к:   " << fs_.pathOf(fs_.followLink(e.getId())) << "\n";
    if (e.typeChar() != 'd' && readYesNo("Показать содержимое текста? (д/н): ")) {
        try {
            cout << "----\n" << fs_.readText(path) << "\n----\n";
        } catch (const InvalidOperationException& ex) {
            cout << "  " << ex.what() << "\n";
        }
    }
}

// ===================== 5. Текст =====================

void ConsoleMenu::textMenu() {
    cout << "1. Заменить текст   2. Дописать строку   3. История версий   4. Откатить к версии\n"
         << "5. Проверить целостность   6. Имитировать сбой диска   0. Назад\n";
    int c = readInt("Выбор: ", 0, 6);
    if (c == 0) return;
    std::string path = askPath("Файл: ");
    if (c == 1) {
        fs_.writeText(path, unescape(readLine("Новый текст (\\n — новая строка): ", true)));
        cout << "Сохранено, создана новая версия.\n";
    } else if (c == 2) {
        std::string line = readLine("Строка: ", true);
        fs_.writeText(path, fs_.readText(path) + "\n" + line);
        cout << "Дописано.\n";
    } else if (c == 3) {
        cout << fs_.versionsOf(path) << "\n";
    } else if (c == 4) {
        cout << fs_.versionsOf(path) << "\n";
        int v = readInt("Номер версии: ", 1, 1000000);
        fs_.rollback(path, v);
        cout << "Откат выполнен новой версией; более поздние версии сохранены.\n";
    } else if (c == 5) {
        fs_.verifyIntegrity(path);
        cout << "Файл цел: сумма совпадает с последней версией, все версии в порядке.\n";
    } else if (c == 6) {
        fs_.simulateCorruption(path);
        cout << "Последняя версия испорчена. Проверьте целостность (пункт 5 -> 5).\n";
    }
}

// ===================== 6. Переименовать, права =====================

void ConsoleMenu::manageMenu() {
    cout << "1. Переименовать   2. Переместить   3. Копировать   4. Изменить права (rwx)\n"
         << "5. Правило ACL   6. Показать ACL   7. Убрать правила ACL   0. Назад\n";
    int c = readInt("Выбор: ", 0, 7);
    if (c == 0) return;
    std::string path = askPath("Что: ");
    if (c == 1) {
        fs_.rename(path, readLine("Новое имя: "));
        cout << "Переименовано.\n";
    } else if (c == 2) {
        fs_.move(path, askPath("В какую папку: "));
        cout << "Перемещено.\n";
    } else if (c == 3) {
        int id = fs_.copy(path, askPath("В какую папку (пусто — текущая): "));
        cout << "Копия: " << fs_.pathOf(id) << "\n";
    } else if (c == 4) {
        cout << "Сейчас: " << fs_.stat(path).getPermission() << "\n";
        fs_.chmod(path, readLine("Новые права (например, rwxr-x---): "));
        cout << "Права изменены: " << fs_.stat(path).getPermission() << "\n";
    } else if (c == 5) {
        bool isGroup = readYesNo("Для группы? (д — группа, н — пользователь): ");
        std::string who = readLine(isGroup ? "Имя группы: " : "Логин: ");
        A access = readAccess();
        bool allow = readYesNo("Разрешить? (д — разрешить, н — запретить): ");
        fs_.setAclRule(path, who, isGroup, access, allow);
        cout << "Правило добавлено. Запрет в ACL важнее любого разрешения.\n";
    } else if (c == 6) {
        cout << fs_.aclOf(path) << "\n";
    } else if (c == 7) {
        bool isGroup = readYesNo("Для группы? (д/н): ");
        std::string who = readLine(isGroup ? "Имя группы: " : "Логин: ");
        cout << "Удалено правил: " << fs_.clearAclRules(path, who, isGroup) << "\n";
    }
}

// ===================== 7. Корзина =====================

void ConsoleMenu::trashMenu() {
    cout << "1. Удалить в корзину   2. Показать корзину   3. Восстановить\n"
         << "4. Очистить истёкшие   5. Очистить свои сразу   0. Назад\n";
    int c = readInt("Выбор: ", 0, 5);
    if (c == 1) {
        fs_.remove(askPath("Что удалить: "));
        cout << "Перемещено в корзину на " << fs_.getTrash().getRetentionDays() << " дней. Квота пока занята.\n";
    } else if (c == 2) {
        const std::vector<TrashItem>& items = fs_.getTrash().getItems();
        cout << fs_.getTrash() << "\n";
        for (size_t i = 0; i < items.size(); ++i)
            cout << "  #" << items[i].getEntryId() << ": осталось дней " << items[i].daysLeft(fs_.now()) << "\n";
    } else if (c == 3) {
        cout << fs_.getTrash() << "\n";
        int id = readInt("ID элемента (#...): ", 1, 1000000000);
        fs_.restore(id);
        cout << "Восстановлено: " << fs_.pathOf(id) << "\n";
    } else if (c == 4) {
        cout << "Удалено навсегда: " << fs_.emptyTrash(true) << "\n";
    } else if (c == 5) {
        cout << "Удалено навсегда: " << fs_.emptyTrash(false) << "\n";
    }
}

// ===================== 8. Блокировки =====================

void ConsoleMenu::lockMenu() {
    cout << "1. Монопольная (пишу)   2. Общая (читаю)   3. Снять   4. Мои блокировки   0. Назад\n";
    int c = readInt("Выбор: ", 0, 4);
    if (c == 1 || c == 2) {
        std::string path = askPath("Файл: ");
        fs_.lock(path, c == 1);
        cout << "Заблокировано на 30 минут.\n";
    } else if (c == 3) {
        fs_.unlock(askPath("Файл: "));
        cout << "Блокировка снята.\n";
    } else if (c == 4) {
        std::vector<FileLock> mine = fs_.getLocks().locksOf(fs_.currentUserId());
        if (mine.empty()) cout << "  (нет)\n";
        for (size_t i = 0; i < mine.size(); ++i)
            cout << "  " << fs_.pathOf(mine[i].getEntryId()) << " — " << mine[i] << "\n";
    }
}

// ===================== 9. Ссылки =====================

void ConsoleMenu::shareMenu() {
    cout << "1. Поделиться файлом   2. Открыть по ссылке   3. Записать по ссылке   4. Мои ссылки   0. Назад\n";
    int c = readInt("Выбор: ", 0, 4);
    if (c == 1) {
        std::string path = askPath("Файл: ");
        int days = readInt("На сколько дней: ", 1, 365);
        int uses = readInt("Сколько переходов (0 — без ограничения): ", 0, 1000000);
        bool write = readYesNo("Разрешить запись? (д/н): ");
        cout << "Токен ссылки: " << fs_.share(path, days, uses, write) << "\n";
    } else if (c == 2) {
        std::string token = readLine("Токен: ");
        cout << "----\n" << fs_.readShared(token) << "\n----\n";
    } else if (c == 3) {
        std::string token = readLine("Токен: ");
        fs_.writeShared(token, unescape(readLine("Новый текст (\\n — новая строка): ", true)));
        cout << "Записано по ссылке.\n";
    } else if (c == 4) {
        std::vector<ShareLink> mine = fs_.getShares().activeOf(fs_.currentUserId(), fs_.now());
        if (mine.empty()) cout << "  (нет действующих)\n";
        for (size_t i = 0; i < mine.size(); ++i)
            cout << "  " << mine[i] << "  (" << fs_.pathOf(mine[i].getEntryId()) << ")\n";
    }
}

// ===================== 10. Поиск =====================

void ConsoleMenu::searchDialog() {
    SearchQuery q;
    std::string pattern = readLine("Шаблон имени (* и ?, пусто — любое): ", true);
    if (!pattern.empty()) q.byName(pattern);
    std::string ext = readLine("Расширение (пусто — любое): ", true);
    if (!ext.empty()) q.byExtension(ext);
    double maxKb = readDouble("Размер до, KB (0 — без ограничения): ", 0, 1e9);
    if (maxKb > 0) q.bySize(DataSize::fromKB(readDouble("Размер от, KB: ", 0, maxKb)), DataSize::fromKB(maxKb));
    int days = readInt("Изменённые за последние N дней (0 — любые): ", 0, 100000);
    if (days > 0) q.modifiedSince(fs_.now().addDays(-days));
    q.withHidden(readYesNo("Показывать скрытые? (д/н): "));
    SearchResult result = fs_.search(q);
    result.sortBySizeDesc();
    cout << "Запрос: " << q << "\nНайдено: " << result.count() << ", всего " << result.totalSize()
         << " (только то, что вам можно читать)\n";
    std::vector<int> top = result.top(20);
    for (size_t i = 0; i < top.size(); ++i)
        cout << "  " << fs_.entry(top[i]).size() << "\t" << fs_.pathOf(top[i]) << "\n";
}

// ===================== 11. Архив и резервные копии =====================

void ConsoleMenu::archiveMenu() {
    cout << "1. Архивировать файлы   2. Резервная копия сейчас   3. Список резервных копий   0. Назад\n";
    int c = readInt("Выбор: ", 0, 3);
    if (c == 1) {
        std::string list = readLine("Файлы через запятую: ");
        std::vector<std::string> paths;
        std::istringstream in(list);
        std::string part;
        while (std::getline(in, part, ',')) {
            size_t b = part.find_first_not_of(' '), e = part.find_last_not_of(' ');
            if (b != std::string::npos) paths.push_back(absPath(part.substr(b, e - b + 1)));
        }
        std::string dir = askPath("Куда положить архив (пусто — текущая папка): ");
        int id = fs_.archive(paths, dir, readLine("Имя архива: "));
        cout << "  " << fs_.entry(id) << "\n  Внутри: " << fs_.entry(id).preview(200) << "\n";
    } else if (c == 2) {
        int id = fs_.runBackup();
        cout << "Готово: " << fs_.getBackups().get(id) << "\n";
    } else if (c == 3) {
        cout << fs_.getBackups() << "\n";
    }
}

// ===================== 12. Администрирование =====================

void ConsoleMenu::adminMenu() {
    int me = fs_.currentUserId();
    if (!fs_.account(me).canManageUsers()) {
        cout << "Только для администратора (" << fs_.account(me).role() << " не может управлять пользователями).\n";
        return;
    }
    cout << " 1. Все учётные записи    6. Группы               11. Заблокировать\n"
         << " 2. Добавить пользователя 7. Создать группу       12. Разблокировать\n"
         << " 3. Добавить гостя        8. Добавить в группу    13. Продлить гостя\n"
         << " 4. Добавить админа       9. Убрать из группы     14. Журнал аудита\n"
         << " 5. Добавить суперпольз. 10. Квота                 0. Назад\n";
    int c = readInt("Выбор: ", 0, 14);
    if (c == 1) {
        for (size_t i = 0; i < fs_.getUsers().size(); ++i) cout << "  " << fs_.getUsers()[i] << "\n";
        for (size_t i = 0; i < fs_.getGuests().size(); ++i) cout << "  " << fs_.getGuests()[i] << "\n";
        for (size_t i = 0; i < fs_.getAdmins().size(); ++i) cout << "  " << fs_.getAdmins()[i] << "\n";
        for (size_t i = 0; i < fs_.getSuperUsers().size(); ++i) cout << "  " << fs_.getSuperUsers()[i] << "\n";
        for (size_t i = 0; i < fs_.getServices().size(); ++i) cout << "  " << fs_.getServices()[i] << "\n";
    } else if (c >= 2 && c <= 5) {
        std::string login = readLine("Логин: ");
        std::string password = readLine("Пароль (" + std::string("политика: ") + "не короче 8, цифра, заглавная, без логина): ");
        int id;
        if (c == 3) {
            id = fs_.addGuest(login, password, readInt("На сколько дней: ", 1, 365));
        } else {
            std::string name = readLine("Имя: ");
            if (c == 2) id = fs_.addUser(login, name, password);
            else if (c == 4) id = fs_.addAdmin(login, name, password, readInt("Уровень (1–3): ", 1, 3));
            else id = fs_.addSuperUser(login, name, password);
        }
        cout << "Создано: " << fs_.account(id) << "\n";
    } else if (c == 6) {
        for (size_t i = 0; i < fs_.getGroups().size(); ++i) {
            const UserGroup& g = fs_.getGroups()[i];
            cout << "  " << g << ":";
            for (size_t j = 0; j < g.getMemberIds().size(); ++j) cout << ' ' << fs_.account(g.getMemberIds()[j]).getLogin();
            cout << "\n";
        }
        if (fs_.getGroups().empty()) cout << "  (нет групп)\n";
    } else if (c == 7) {
        fs_.createGroup(readLine("Имя группы: "));
        cout << "Группа создана.\n";
    } else if (c == 8 || c == 9) {
        std::string group = readLine("Группа: ");
        std::string login = readLine("Логин: ");
        if (c == 8) fs_.addToGroup(group, login);
        else fs_.removeFromGroup(group, login);
        cout << "Готово.\n";
    } else if (c == 10) {
        std::string login = readLine("Логин: ");
        fs_.setQuota(login, DataSize::fromMB(readDouble("Новый лимит, MB: ", 1, 1e7)));
        cout << "Квота изменена.\n";
    } else if (c == 11 || c == 12) {
        fs_.setBlocked(readLine("Логин: "), c == 11);
        cout << "Готово.\n";
    } else if (c == 13) {
        std::string login = readLine("Логин гостя: ");
        fs_.extendGuest(login, readInt("На сколько дней: ", 1, 365));
        cout << "Продлено.\n";
    } else if (c == 14) {
        const std::vector<AuditRecord>& records = fs_.getAudit().getRecords();
        size_t from = records.size() > 30 ? records.size() - 30 : 0;
        cout << "Последние записи журнала (" << records.size() - from << " из " << records.size() << "):\n";
        for (size_t i = from; i < records.size(); ++i) cout << "  " << records[i] << "\n";
    }
}

// ===================== 13. Отчёты =====================

void ConsoleMenu::reportsMenu() {
    int days = readInt("Период: последние N дней: ", 1, 3650);
    ReportGenerator r(fs_.now().addDays(-days), fs_.now());
    cout << "1. Занятое место   2. Квоты   3. Мои действия   4. Безопасность\n"
         << "5. Сроки действия (IExpirable)   6. Размеры (ISizeable)   0. Назад\n";
    int c = readInt("Выбор: ", 0, 6);
    if (c == 1) cout << r.diskUsage(fs_) << "\n";
    else if (c == 2) cout << r.quotaReport(fs_) << "\n";
    else if (c == 3) cout << r.activity(fs_, fs_.currentUserId()) << "\n";
    else if (c == 4) {
        if (!fs_.account(fs_.currentUserId()).canManageUsers()) throw AccessDeniedException(fs_.account(fs_.currentUserId()).getLogin(), "смотреть отчёт о безопасности");
        cout << r.securityReport(fs_) << "\n";
    } else if (c == 5) {
        // одна функция expiryStatus для пяти не связанных классов — в этом смысл интерфейса
        const Timestamp& now = fs_.now();
        const std::vector<Session>& sessions = fs_.getAuth().getSessions();
        for (size_t i = 0; i < sessions.size(); ++i)
            cout << r.expiryStatus(sessions[i], "сессия #" + std::to_string(sessions[i].getId()) + " ("
                                   + fs_.account(sessions[i].getUserId()).getLogin() + ")", now) << "\n";
        const std::vector<ShareLink>& links = fs_.getShares().getLinks();
        for (size_t i = 0; i < links.size(); ++i)
            cout << r.expiryStatus(links[i], "ссылка " + links[i].getToken().substr(0, 8), now) << "\n";
        const std::vector<TrashItem>& trash = fs_.getTrash().getItems();
        for (size_t i = 0; i < trash.size(); ++i)
            cout << r.expiryStatus(trash[i], "корзина: " + trash[i].getOriginalPath(), now) << "\n";
        const std::vector<FileLock>& locks = fs_.getLocks().getLocks();
        for (size_t i = 0; i < locks.size(); ++i)
            cout << r.expiryStatus(locks[i], "блокировка " + fs_.pathOf(locks[i].getEntryId()), now) << "\n";
        for (size_t i = 0; i < fs_.getGuests().size(); ++i)
            cout << r.expiryStatus(fs_.getGuests()[i], "гость " + fs_.getGuests()[i].getLogin(), now) << "\n";
    } else if (c == 6) {
        // одна функция sizeLine для файла, корзины и резервной копии
        cout << r.sizeLine(fs_.stat(cwd_), "текущая папка (сама запись)") << "\n"
             << r.sizeLine(fs_.getTrash(), "корзина") << "\n";
        if (fs_.getBackups().count() > 0) {
            try {
                cout << r.sizeLine(fs_.getBackups().latest(), "последняя резервная копия") << "\n";
            } catch (const BackupFailedException& e) {
                cout << "  " << e.what() << "\n";
            }
        }
    }
}

// ===================== 14. Уведомления =====================

void ConsoleMenu::notificationsMenu() {
    int me = fs_.currentUserId();
    std::vector<Notification> all = fs_.getNotifications().allFor(me);
    if (all.empty()) cout << "  (уведомлений нет)\n";
    for (size_t i = 0; i < all.size(); ++i) cout << "  " << all[i] << "\n";
    cout << "* — не прочитано.\n";
}

// ===================== 15. Время =====================

void ConsoleMenu::timeDialog() {
    cout << "Сейчас: " << fs_.now() << "\n1. На N минут   2. На N дней   0. Назад\n";
    int c = readInt("Выбор: ", 0, 2);
    if (c == 0) return;
    int n = readInt("N: ", 1, 100000);
    fs_.advanceTime(c == 1 ? n : n * 1440);
    cout << "Теперь: " << fs_.now() << ". Сессии, ссылки, блокировки и записи корзины могли истечь (пункт 13 -> 5).\n";
}

// ===================== 16. Демонстрация исключений =====================

/**
 * @brief Всё делается на копии: файловая система хранится по значению, поэтому копия независима,
 * и демонстрация ничего не меняет в настоящих данных.
 */
void ConsoleMenu::exceptionsDemo() const {
    FileSystem demo = fs_;
    demo.logout();
    cout << "Демонстрация всех 18 исключений (на копии файловой системы):\n";

    try { Permission::fromString("rw"); }
    catch (const FileSystemException& e) { showCaught(1, "FileSystemException (база)", e); }
    try { demo.account(999999); }
    catch (const EntryNotFoundException& e) { showCaught(2, "EntryNotFoundException", e); }
    try { demo.resolve("/нет/такого/пути"); }
    catch (const InvalidPathException& e) { showCaught(3, "InvalidPathException", e); }
    try { demo.addUser("ivan", "Ещё один", "Secret123"); }
    catch (const EntryAlreadyExistsException& e) { showCaught(4, "EntryAlreadyExistsException", e); }

    demo.login("ivan", "Secret123");
    try { demo.remove("/home/ivan/docs"); }
    catch (const DirectoryNotEmptyException& e) { showCaught(5, "DirectoryNotEmptyException", e); }
    try { demo.move("/home/ivan/docs", "/home/ivan/docs"); }
    catch (const InvalidOperationException& e) { showCaught(6, "InvalidOperationException", e); }
    try { demo.rollback("/home/ivan/docs/notes.txt", 999); }
    catch (const VersionNotFoundException& e) { showCaught(7, "VersionNotFoundException", e); }
    std::string token = demo.share("/home/ivan/docs/notes.txt", 1, 1);
    demo.readShared(token);
    try { demo.readShared(token); }
    catch (const ShareLinkExpiredException& e) { showCaught(8, "ShareLinkExpiredException", e); }
    demo.simulateCorruption("/home/ivan/docs/report.md");
    try { demo.verifyIntegrity("/home/ivan/docs/report.md"); }
    catch (const CorruptedFileException& e) { showCaught(9, "CorruptedFileException", e); }
    demo.createTextFile("/shared", "ivan-draft.txt", "черновик");
    demo.lock("/shared/ivan-draft.txt");

    demo.login("guest", "Visit1234");
    try { demo.createTextFile("/home/ivan", "hack.txt", "x"); }
    catch (const AccessDeniedException& e) { showCaught(10, "AccessDeniedException", e); }
    try { demo.createTextFile("/home/guest", "big.bin", std::string(51 * 1024 * 1024, 'x')); }
    catch (const QuotaExceededException& e) { showCaught(11, "QuotaExceededException", e); }
    try { demo.readText("/shared/ivan-draft.txt"); }
    catch (const FileLockedException& e) { showCaught(12, "FileLockedException", e); }
    demo.advanceTime(31);
    try { demo.currentUserId(); }
    catch (const SessionExpiredException& e) { showCaught(13, "SessionExpiredException", e); }

    try { demo.login("ivan", "неверный"); }
    catch (const AuthenticationException& e) { showCaught(14, "AuthenticationException", e); }
    try { demo.addUser("newbie", "Новичок", "12345"); }
    catch (const WeakPasswordException& e) { showCaught(15, "WeakPasswordException", e); }

    StorageDevice small(9, "flash", "SSD", DataSize::fromMB(1));
    try { small.allocate(DataSize::fromMB(2)); }
    catch (const InsufficientStorageException& e) { showCaught(16, "InsufficientStorageException", e); }
    Volume archiveVolume("archive", "/mnt/archive", 9, true);
    try { archiveVolume.checkWritable(); }
    catch (const ReadOnlyVolumeException& e) { showCaught(17, "ReadOnlyVolumeException", e); }
    BackupManager empty;
    try { empty.latest(); }
    catch (const BackupFailedException& e) { showCaught(18, "BackupFailedException", e); }

    cout << "Все они наследуют FileSystemException, поэтому меню ловит любое одним catch (const FileSystemException&).\n";
}
