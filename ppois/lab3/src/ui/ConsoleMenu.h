#pragma once
#include <string>
#include <vector>
#include "service/FileSystem.h"

/**
 * @brief Консольное меню в виде простого файлового менеджера. Весь ввод-вывод программы — здесь и в Input,
 * модель ничего не печатает. Меню не владеет файловой системой (поле-ссылка), а работает с той, что создана в main.
 * Хранит текущую папку cwd_, как терминал: относительные пути дописываются к ней.
 */
class ConsoleMenu {
public:
    explicit ConsoleMenu(FileSystem& fs);

    /**
     * @brief Главный цикл. Ошибки любого пункта ловятся здесь, от частного к общему:
     * SessionExpired -> AccessDenied -> QuotaExceeded -> FileSystemException -> std::exception.
     */
    void run();

private:
    void showMainMenu() const;
    /** @brief "ivan@fms:/home/ivan  [квота 12% | 01.11.2026 10:15]"; сессию не продлевает. */
    std::string promptLine() const;
    /** @brief Абсолютный нормализованный путь: пустой — текущая папка, относительный — от неё. */
    std::string absPath(const std::string& input) const;
    /** @brief Спросить путь и сразу превратить его в абсолютный. */
    std::string askPath(const std::string& prompt) const;
    /** @brief Домашняя папка вошедшего пользователя или "/", если её нет. */
    std::string homeOf(int userId) const;
    /** @brief Вывести элементы списком в стиле ls -l. */
    void printList(const std::vector<int>& ids) const;

    /** @brief 1. Войти, выйти, сменить пароль. */
    void accountMenu();
    /** @brief 2. ls, cd, размер папки. */
    void browseMenu();
    /** @brief 3. Создать папку, текст, изображение, программу, ссылку. */
    void createMenu();
    /** @brief 4. Свойства и preview (полиморфизм), содержимое текста. */
    void openDialog();
    /** @brief 5. Изменить текст, история версий, откат, проверка целостности. */
    void textMenu();
    /** @brief 6. Переименовать, переместить, копировать, права и ACL. */
    void manageMenu();
    /** @brief 7. Корзина. */
    void trashMenu();
    /** @brief 8. Блокировки. */
    void lockMenu();
    /** @brief 9. Ссылки общего доступа. */
    void shareMenu();
    /** @brief 10. Поиск по шаблону, расширению, размеру и дате. */
    void searchDialog();
    /** @brief 11. Архив и резервные копии. */
    void archiveMenu();
    /** @brief 12. Администрирование (только canManageUsers). */
    void adminMenu();
    /** @brief 13. Отчёты, в том числе сроки через IExpirable и размеры через ISizeable. */
    void reportsMenu();
    /** @brief 14. Уведомления. */
    void notificationsMenu();
    /** @brief 15. Перемотать время. */
    void timeDialog();
    /** @brief 16. Вызвать каждое из 18 исключений и вывести what(). Работает на копии файловой системы. */
    void exceptionsDemo() const;

    FileSystem& fs_;
    std::string cwd_;
};
