#include <iostream>
#include <stdexcept>
#include "ui/DemoData.h"
#include "ui/ConsoleMenu.h"

/** @brief Программа запускается с готовой демо-фирмой; ввод закрыт (Ctrl+D) -> аккуратный выход. */
int main() {
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
