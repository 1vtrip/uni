#pragma once
#include <string>
#include <vector>
#include "fs/File.h"

/** @brief Исполняемый файл: платформа, версия и цифровая подпись. */
class ExecutableFile final : public File {
public:
    /** @brief @throws InvalidOperationException версия не вида "1.10.2" (числа через точку). */
    ExecutableFile(int id, const std::string& name, int parentId, int ownerId,
                   const Permission& permission, const Timestamp& now,
                   const std::string& platform, const std::string& version,
                   const DataSize& size, bool isSigned = false);

    std::string typeName() const override;
    /** @brief "[Исполняемый, macOS, подписан]". */
    std::string preview(int maxChars) const override;

    /** @brief Подходит ли платформа; регистр не важен: "macos" == "macOS". */
    bool canRunOn(const std::string& platform) const;
    /** @brief Безопасно ли запускать: файл подписан и у владельца есть право на выполнение. */
    bool isSafeToRun() const;
    /** @brief Подписать файл. */
    void sign();
    /**
     * @brief Новее ли эта версия, чем otherVersion. Сравнение по числам слева направо,
     * недостающие части считаются нулями: "1.10" новее "1.9", "1.2" и "1.2.0" равны.
     * @throws InvalidOperationException otherVersion не вида "1.10.2".
     */
    bool isNewerThan(const std::string& otherVersion) const;

    const std::string& getPlatform() const;
    const std::string& getVersion() const;
    bool isSigned() const;

private:
    /** @brief "1.10.2" -> {1, 10, 2}. @throws InvalidOperationException пустая часть или не цифры. */
    static std::vector<int> parseVersion(const std::string& version);

    std::string platform_;
    bool signed_;            // "signed" без подчёркивания — ключевое слово C++
    std::string version_;
};
