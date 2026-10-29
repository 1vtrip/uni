#include <iostream>
#include <stdexcept>
#include <string>
#include "ui/DemoData.h"
#include "ui/ConsoleMenu.h"
#include "ui/SelfTest.h"

/**
 * @brief Без аргументов — меню с готовой демо-фирмой; с флагом --test — самопроверка всех этапов
 * (код возврата 1, если хоть одна проверка не прошла). Закрытый ввод (Ctrl+D) — аккуратный выход.
 */
int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--test")
        return runSelfTest() == 0 ? 0 : 1;
    RepairFirm firm = createDemoFirm();
    ConsoleMenu menu(firm);
    try {
        menu.run();
    } catch (const std::exception& e) {
        std::cout << "\n" << e.what() << ". Выход.\n";
    }
    std::cout << "До свидания!\n";
    return 0;
}
