#pragma once
#include <string>

/**
 * @brief Безопасный ввод с клавиатуры для меню. Всё читается целой строкой через getline и уже потом
 * разбирается: так в потоке не остаётся "\n" после чисел, а мусор вместо числа не ломает cin.
 * При неверном вводе функция сообщает об ошибке и спрашивает снова.
 * Если ввод закрыт (Ctrl+D), бросается std::runtime_error.
 */

/** @brief Строка целиком; пустая не принимается, если allowEmpty == false. */
std::string readLine(const std::string& prompt, bool allowEmpty = false);
/** @brief Целое число от min до max включительно. */
int readInt(const std::string& prompt, int min, int max);
/** @brief Дробное число от min до max включительно (запятая тоже принимается как разделитель). */
double readDouble(const std::string& prompt, double min, double max);
/** @brief Ответ да/нет: "д", "да", "y", "yes" или "н", "нет", "n", "no". */
bool readYesNo(const std::string& prompt);
