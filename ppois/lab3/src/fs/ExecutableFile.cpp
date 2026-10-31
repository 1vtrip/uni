#include "fs/ExecutableFile.h"
#include "exceptions/Exceptions.h"
#include <cctype>

ExecutableFile::ExecutableFile(int id, const std::string& name, int parentId, int ownerId,
                               const Permission& permission, const Timestamp& now,
                               const std::string& platform, const std::string& version,
                               const DataSize& size, bool isSigned)
    : File(id, name, parentId, ownerId, permission, now, MimeType::fromExtension("exe"), size),
      platform_(platform), signed_(isSigned), version_(version) {
    parseVersion(version_);                 // только проверка: бросит, если версия неверная
}

std::string ExecutableFile::typeName() const { return "Исполняемый файл"; }

std::string ExecutableFile::preview(int) const {
    return "[Исполняемый, " + platform_ + ", " + (signed_ ? "подписан" : "не подписан") + "]";
}

/** @brief Строки равны по длине и посимвольно в нижнем регистре. */
bool ExecutableFile::canRunOn(const std::string& platform) const {
    if (platform.size() != platform_.size()) return false;
    for (size_t i = 0; i < platform_.size(); ++i)
        if (std::tolower((unsigned char)platform[i]) != std::tolower((unsigned char)platform_[i]))
            return false;
    return true;
}

/** @brief permission_ — protected-поле FsEntry, поэтому наследник читает его напрямую. */
bool ExecutableFile::isSafeToRun() const {
    return signed_ && permission_.has(Permission::Scope::Owner, Permission::Access::Execute);
}

void ExecutableFile::sign() { signed_ = true; }

/**
 * @brief Строки сравнивать нельзя: "1.10" < "1.9", потому что '1' < '9'.
 * Поэтому обе версии разбираются на числа и сравниваются по частям; первая отличающаяся решает.
 */
bool ExecutableFile::isNewerThan(const std::string& otherVersion) const {
    std::vector<int> mine = parseVersion(version_);
    std::vector<int> other = parseVersion(otherVersion);
    size_t n = mine.size() > other.size() ? mine.size() : other.size();
    for (size_t i = 0; i < n; ++i) {
        int a = i < mine.size() ? mine[i] : 0;
        int b = i < other.size() ? other[i] : 0;
        if (a != b) return a > b;
    }
    return false;                           // версии равны
}

/** @brief Число копится по цифрам (part * 10 + цифра); точка закрывает часть. Пустая часть ("1..2", "1.") — ошибка. */
std::vector<int> ExecutableFile::parseVersion(const std::string& version) {
    std::vector<int> parts;
    int part = 0;
    bool hasDigit = false;
    for (size_t i = 0; i <= version.size(); ++i) {
        if (i == version.size() || version[i] == '.') {
            if (!hasDigit) throw InvalidOperationException("неверная версия \"" + version + "\"");
            parts.push_back(part);
            part = 0;
            hasDigit = false;
        } else if (std::isdigit((unsigned char)version[i])) {
            if (part > 100000) throw InvalidOperationException("слишком большое число в версии \"" + version + "\"");
            part = part * 10 + (version[i] - '0');
            hasDigit = true;
        } else {
            throw InvalidOperationException("неверная версия \"" + version + "\"");
        }
    }
    return parts;
}

const std::string& ExecutableFile::getPlatform() const { return platform_; }
const std::string& ExecutableFile::getVersion() const { return version_; }
bool ExecutableFile::isSigned() const { return signed_; }
