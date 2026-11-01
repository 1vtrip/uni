#include <iostream>
#include <stdexcept>
#include <string>
#include "ui/ConsoleMenu.h"
#include "ui/DemoData.h"
#include "ui/SelfTest.h"

/**
 * @brief Точка входа программы «Система управления файлами».
 * Без аргументов — меню с готовой демо-системой; с флагом --test — самопроверка всех этапов
 * (код возврата 1, если хоть одна проверка не прошла). Закрытый ввод (Ctrl+D) — аккуратный выход.
 */
int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--test")
        return runSelfTest() == 0 ? 0 : 1;
    FileSystem fs = createDemoFileSystem();
    std::cout << "Система управления файлами (виртуальная, в памяти)\n" << demoCredentials();
    ConsoleMenu menu(fs);
    try {
        menu.run();
    } catch (const std::exception& e) {
        std::cout << "\n" << e.what() << ". Выход.\n";
    }
    std::cout << "До свидания!\n";
    return 0;
}
