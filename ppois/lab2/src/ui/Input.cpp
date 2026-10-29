#include "ui/Input.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "exceptions/Exceptions.h"

/** @brief Одна строка из cin; закрытый ввод -> std::runtime_error, чтобы циклы повтора не стали вечными. */
static std::string nextLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) throw std::runtime_error("Ввод прерван");
    return line;
}

/** @brief Убрать пробелы по краям строки. */
static std::string trim(const std::string& s) {
    size_t begin = s.find_first_not_of(" \t\r");
    if (begin == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r");
    return s.substr(begin, end - begin + 1);
}

std::string readLine(const std::string& prompt, bool allowEmpty) {
    while (true) {
        std::string line = trim(nextLine(prompt));
        if (!line.empty() || allowEmpty) return line;
        std::cout << "  Строка не может быть пустой.\n";
    }
}

/** @brief Число принимается, только если строка целиком — число: "12abc" отклоняется. */
int readInt(const std::string& prompt, int min, int max) {
    while (true) {
        std::istringstream in(trim(nextLine(prompt)));
        int value;
        char extra;
        if (!(in >> value) || in >> extra)
            std::cout << "  Введите целое число.\n";
        else if (value < min || value > max)
            std::cout << "  Число должно быть от " << min << " до " << max << ".\n";
        else
            return value;
    }
}

double readDouble(const std::string& prompt, double min, double max) {
    while (true) {
        std::string line = trim(nextLine(prompt));
        for (size_t i = 0; i < line.size(); ++i)
            if (line[i] == ',') line[i] = '.';
        std::istringstream in(line);
        double value;
        char extra;
        if (!(in >> value) || in >> extra)
            std::cout << "  Введите число (например, 1.5).\n";
        else if (value < min || value > max)
            std::cout << "  Число должно быть от " << min << " до " << max << ".\n";
        else
            return value;
    }
}

/** @brief Формат разбирается здесь, а существование даты проверяет конструктор Date (InvalidDateException). */
Date readDate(const std::string& prompt) {
    while (true) {
        std::istringstream in(trim(nextLine(prompt)));
        int d, m, y;
        char dot1, dot2, extra;
        if (!(in >> d >> dot1 >> m >> dot2 >> y) || dot1 != '.' || dot2 != '.' || in >> extra) {
            std::cout << "  Формат даты: ДД.ММ.ГГГГ, например 28.10.2026.\n";
            continue;
        }
        try {
            return Date(d, m, y);
        } catch (const InvalidDateException& e) {
            std::cout << "  " << e.what() << "\n";
        }
    }
}

bool readYesNo(const std::string& prompt) {
    while (true) {
        std::string a = trim(nextLine(prompt));
        if (a == "д" || a == "да" || a == "Д" || a == "Да" || a == "y" || a == "yes" || a == "Y") return true;
        if (a == "н" || a == "нет" || a == "Н" || a == "Нет" || a == "n" || a == "no" || a == "N") return false;
        std::cout << "  Ответьте «д» или «н».\n";
    }
}
