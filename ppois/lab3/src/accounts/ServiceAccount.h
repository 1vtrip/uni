#pragma once
#include <string>
#include <vector>
#include "accounts/Account.h"

/**
 * @brief Служебная учётная запись для программ (например, резервного копирования).
 * Права задаются списком разрешённых операций; программа подтверждает себя API-ключом.
 */
class ServiceAccount final : public Account {
public:
    /** @brief Начальный ключ получается так же, как в rotateKey. @throws InvalidOperationException пустое имя сервиса. */
    ServiceAccount(int id, const std::string& login, const std::string& displayName,
                   const Credentials& credentials, const Timestamp& created, const std::string& serviceName);

    std::string role() const override;
    /** @brief Строка Account::print, имя сервиса и разрешённые операции. */
    void print(std::ostream& os) const override;
    /** @brief 1440 минут (сутки): резервное копирование может идти долго. */
    int maxSessionMinutes() const override;
    /** @brief Читать любые файлы можно, только если разрешена операция "backup". */
    bool canReadAnyFile() const override;

    bool canPerform(const std::string& op) const;
    /** @brief Разрешить операцию; повтор ничего не меняет. @throws InvalidOperationException пустое имя. */
    void allow(const std::string& op);
    /** @brief Запретить операцию; false, если её и так не было. */
    bool disallow(const std::string& op);
    /**
     * @brief Новый ключ = Checksum::of(serviceName_ + старый ключ + seed).toHex().
     * Старый ключ входит в хеш, поэтому каждая смена даёт новый ключ, даже с тем же seed.
     */
    void rotateKey(int seed);
    /** @brief Совпадает ли ключ с текущим; пустой ключ не подходит никогда. */
    bool verifyKey(const std::string& key) const;

    const std::string& getServiceName() const;
    const std::vector<std::string>& getAllowedOperations() const;
    const std::string& getApiKey() const;

private:
    std::string serviceName_;
    std::vector<std::string> allowedOperations_;
    std::string apiKey_;
};
