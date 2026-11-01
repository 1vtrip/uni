#include "service/ReportGenerator.h"
#include "accounts/PasswordPolicy.h"
#include "exceptions/Exceptions.h"
#include "service/FileSystem.h"
#include <sstream>
#include <utility>
#include <vector>

ReportGenerator::ReportGenerator(const Timestamp& from, const Timestamp& to) : from_(from), to_(to) {
    if (to < from) throw InvalidOperationException("начало периода отчёта позже конца");
}

void ReportGenerator::setPeriod(const Timestamp& from, const Timestamp& to) {
    if (to < from) throw InvalidOperationException("начало периода отчёта позже конца");
    from_ = from;
    to_ = to;
}

const Timestamp& ReportGenerator::getFrom() const { return from_; }
const Timestamp& ReportGenerator::getTo() const { return to_; }

bool ReportGenerator::inPeriod(const Timestamp& t) const { return from_ <= t && t <= to_; }

std::string ReportGenerator::nameOf(const FileSystem& fs, int userId) {
    if (userId == 0) return "неизвестный";
    try {
        return fs.account(userId).getLogin() + " (#" + std::to_string(userId) + ")";
    } catch (const EntryNotFoundException&) {
        return "#" + std::to_string(userId);
    }
}

std::string ReportGenerator::pad(const std::string& s, int width) {
    int letters = PasswordPolicy::letterCount(s);
    return letters >= width ? s + " " : s + std::string(width - letters, ' ');
}

/**
 * @brief Папки сортируются выбором по убыванию размера: параллельные векторы логинов, размеров и квот
 * меняются местами вместе, как в SearchResult.
 */
std::string ReportGenerator::diskUsage(const FileSystem& fs) const {
    std::vector<std::string> logins;
    std::vector<DataSize> inTree;
    std::vector<std::string> quotas;
    for (size_t i = 0; i < fs.getUsers().size(); ++i) {
        const UserAccount& u = fs.getUsers()[i];
        std::ostringstream q; q << u.getQuota();
        logins.push_back(u.getLogin()); inTree.push_back(fs.directorySize(u.getHomeDirId())); quotas.push_back(q.str());
    }
    for (size_t i = 0; i < fs.getGuests().size(); ++i) {
        const GuestAccount& g = fs.getGuests()[i];
        std::ostringstream q; q << g.getQuota();
        logins.push_back(g.getLogin()); inTree.push_back(fs.directorySize(g.getHomeDirId())); quotas.push_back(q.str());
    }
    for (size_t i = 0; i < inTree.size(); ++i) {
        size_t best = i;
        for (size_t j = i + 1; j < inTree.size(); ++j)
            if (inTree[best] < inTree[j]) best = j;
        std::swap(inTree[i], inTree[best]);
        std::swap(logins[i], logins[best]);
        std::swap(quotas[i], quotas[best]);
    }
    std::ostringstream out;
    out << "=== Занятое место ===\n";
    out << "Устройство: " << fs.getDevice() << "\n";
    out << "Домашние папки (в дереве | по квоте):\n";
    for (size_t i = 0; i < logins.size(); ++i)
        out << "  " << pad(logins[i], 12) << pad(inTree[i].toString(), 10) << "| " << quotas[i] << "\n";
    out << "Корзина: " << fs.getTrash().count() << " эл., " << fs.getTrash().size() << "\n";
    out << "Резервные копии: " << fs.getBackups().count() << ", занимают " << fs.getBackups().storageUsed() << "\n";
    out << "Размер в дереве и квота расходятся: файлы в корзине квоту занимают, но в дереве их нет,\n"
        << "а сама домашняя папка (4 KB) создана системой и квоту не занимает.";
    return out.str();
}

std::string ReportGenerator::quotaReport(const FileSystem& fs) const {
    std::vector<std::string> names;
    std::vector<Quota> quotas;
    for (size_t i = 0; i < fs.getUsers().size(); ++i) {
        names.push_back(fs.getUsers()[i].getLogin() + " (" + fs.getUsers()[i].role() + ")");
        quotas.push_back(fs.getUsers()[i].getQuota());
    }
    for (size_t i = 0; i < fs.getGuests().size(); ++i) {
        names.push_back(fs.getGuests()[i].getLogin() + " (" + fs.getGuests()[i].role() + ")");
        quotas.push_back(fs.getGuests()[i].getQuota());
    }
    for (size_t i = 0; i < quotas.size(); ++i) {
        size_t best = i;
        for (size_t j = i + 1; j < quotas.size(); ++j)
            if (quotas[best].usagePercent() < quotas[j].usagePercent()) best = j;
        std::swap(quotas[i], quotas[best]);
        std::swap(names[i], names[best]);
    }
    std::ostringstream out;
    out << "=== Квоты ===\n";
    int near = 0;
    for (size_t i = 0; i < quotas.size(); ++i) {
        out << "  " << pad(names[i], 26) << quotas[i] << "\n";
        if (quotas[i].isNearLimit()) ++near;
    }
    out << "Почти заполнены: " << near << " из " << quotas.size();
    return out.str();
}

/** @brief Сводка по действиям — два параллельных вектора (действие и счётчик), как в AuditLog::suspiciousUsers. */
std::string ReportGenerator::activity(const FileSystem& fs, int userId) const {
    std::vector<AuditRecord> all = fs.getAudit().byUser(userId);
    std::vector<std::string> actions;
    std::vector<int> counts;
    std::ostringstream lines;
    int total = 0;
    for (size_t i = 0; i < all.size(); ++i) {
        if (!inPeriod(all[i].getTime())) continue;
        ++total;
        lines << "\n  " << all[i];
        size_t j = 0;
        while (j < actions.size() && actions[j] != all[i].getAction()) ++j;
        if (j == actions.size()) { actions.push_back(all[i].getAction()); counts.push_back(1); }
        else ++counts[j];
    }
    std::ostringstream out;
    out << "=== Действия " << nameOf(fs, userId) << " за " << from_ << " — " << to_ << " ===\n";
    if (total == 0) return out.str() + "Действий нет.";
    out << "Всего " << total << ":";
    for (size_t j = 0; j < actions.size(); ++j) out << (j == 0 ? " " : ", ") << actions[j] << ' ' << counts[j];
    out << lines.str();
    return out.str();
}

std::string ReportGenerator::securityReport(const FileSystem& fs, int threshold) const {
    const std::vector<AuditRecord>& records = fs.getAudit().getRecords();
    std::vector<int> userIds, failures, failedLogins;
    int totalFailures = 0, unknownLogins = 0;
    for (size_t i = 0; i < records.size(); ++i) {
        const AuditRecord& r = records[i];
        if (!r.isFailure() || !inPeriod(r.getTime())) continue;
        ++totalFailures;
        if (r.getUserId() == 0) {
            if (r.getAction() == "login") ++unknownLogins;
            continue;
        }
        size_t j = 0;
        while (j < userIds.size() && userIds[j] != r.getUserId()) ++j;
        if (j == userIds.size()) { userIds.push_back(r.getUserId()); failures.push_back(0); failedLogins.push_back(0); }
        ++failures[j];
        if (r.getAction() == "login") ++failedLogins[j];
    }
    std::ostringstream out;
    out << "=== Безопасность за " << from_ << " — " << to_ << " ===\n";
    out << "Неудачных действий: " << totalFailures << "\n";
    out << "Неудачные входы:";
    bool any = false;
    for (size_t j = 0; j < userIds.size(); ++j)
        if (failedLogins[j] > 0) { out << "\n  " << pad(nameOf(fs, userIds[j]), 16) << failedLogins[j]; any = true; }
    if (!any) out << " нет";
    out << "\nПопыток войти под несуществующим логином: " << unknownLogins << "\n";
    out << "Подозрительные (неудач больше " << threshold << "):";
    any = false;
    for (size_t j = 0; j < userIds.size(); ++j)
        if (failures[j] > threshold) { out << "\n  " << pad(nameOf(fs, userIds[j]), 16) << failures[j] << " неудач"; any = true; }
    if (!any) out << " нет";
    out << "\nЗаблокированы:";
    any = false;
    for (size_t i = 0; i < fs.getUsers().size(); ++i)
        if (fs.getUsers()[i].isBlocked()) { out << ' ' << fs.getUsers()[i].getLogin(); any = true; }
    for (size_t i = 0; i < fs.getGuests().size(); ++i)
        if (fs.getGuests()[i].isBlocked()) { out << ' ' << fs.getGuests()[i].getLogin(); any = true; }
    for (size_t i = 0; i < fs.getAdmins().size(); ++i)
        if (fs.getAdmins()[i].isBlocked()) { out << ' ' << fs.getAdmins()[i].getLogin(); any = true; }
    if (!any) out << " нет";
    return out.str();
}

std::ostream& operator<<(std::ostream& os, const ReportGenerator& r) {
    return os << "Отчёты за " << r.from_ << " — " << r.to_;
}
