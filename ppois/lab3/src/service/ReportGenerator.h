#pragma once
#include <iostream>
#include <string>
#include "core/Timestamp.h"
#include "interfaces/IExpirable.h"
#include "interfaces/ISizeable.h"

class FileSystem;   // нужен только как const& в параметрах: FileSystem про отчёты не знает, цикла include нет

/**
 * @brief Текстовые отчёты за период [from_, to_]: занятое место, квоты, действия пользователя, безопасность.
 * Методы возвращают строки — печатает меню, в модели нет cout.
 */
class ReportGenerator {
public:
    /** @brief @throws InvalidOperationException from позже to. */
    ReportGenerator(const Timestamp& from, const Timestamp& to);

    /**
     * @brief Устройство, домашние папки (по убыванию размера), корзина и резервные копии.
     * У папок два числа: размер в дереве и занятая квота. Они расходятся: файлы в корзине квоту занимают,
     * но в дереве их уже нет, а сама домашняя папка (4 KB) создана системой и квоту не занимает.
     */
    std::string diskUsage(const FileSystem& fs) const;
    /** @brief Квоты пользователей и гостей по убыванию заполненности, почти заполненные отмечены. */
    std::string quotaReport(const FileSystem& fs) const;
    /** @brief Действия пользователя за период: сводка по видам действий и сами записи. */
    std::string activity(const FileSystem& fs, int userId) const;
    /**
     * @brief Неудачные действия за период, неудачные входы по пользователям, попытки под несуществующими
     * логинами, подозрительные (неудач больше threshold) и заблокированные учётные записи.
     * Подозрительные считаются здесь, с обеими границами периода: AuditLog::suspiciousUsers знает только начало.
     */
    std::string securityReport(const FileSystem& fs, int threshold = 3) const;

    /**
     * @brief Строка о сроке для любого IExpirable: сессии, ссылки, записи корзины, блокировки, гостя.
     * Одна функция работает с пятью не связанными классами — в этом смысл интерфейса.
     */
    std::string expiryStatus(const IExpirable& item, const std::string& label, const Timestamp& now) const;
    /** @brief Строка о размере для любого ISizeable: файла или папки, корзины, резервной копии. */
    std::string sizeLine(const ISizeable& item, const std::string& label) const;

    /** @brief @throws InvalidOperationException from позже to. */
    void setPeriod(const Timestamp& from, const Timestamp& to);
    const Timestamp& getFrom() const;
    const Timestamp& getTo() const;

    /** @brief "Отчёты за 22.10.2026 10:15 — 29.10.2026 10:15". */
    friend std::ostream& operator<<(std::ostream& os, const ReportGenerator& r);

private:
    bool inPeriod(const Timestamp& t) const;
    /** @brief "petr (#5)", "неизвестный" для ID 0, "#9" если учётной записи уже нет. */
    static std::string nameOf(const FileSystem& fs, int userId);
    /** @brief Дополнить пробелами до width букв: setw считает байты, а русская буква в UTF-8 — два байта. */
    static std::string pad(const std::string& s, int width);

    Timestamp from_;
    Timestamp to_;
};
