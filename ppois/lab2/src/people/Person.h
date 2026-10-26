#pragma once
#include <string>
#include <iostream>
#include "core/ContactInfo.h"
#include "core/Date.h"
#include "interfaces/IPrintable.h"

/** @brief Абстрактный человек: клиент или сотрудник. Создать просто Person нельзя. */
class Person : public IPrintable {
public:
    Person(int id, const std::string& name, const ContactInfo& contacts, const Date& birth);
    virtual ~Person() = default;

    /** @brief Роль человека в фирме. Чисто виртуальная: из-за неё класс абстрактный. */
    virtual std::string getRole() const = 0;
    /** @brief Вывод "Роль #id: Имя"; наследники дописывают свои поля. */
    void print(std::ostream& os) const override;

    /** @brief Возраст в полных годах на дату today. */
    int age(const Date& today) const;
    /** @brief Заменить контакты; false и старые остаются, если телефон или e-mail неверные. */
    bool updateContacts(const ContactInfo& contacts);   // TODO(exc): InvalidClientDataException из ContactInfo::validate

    int getId() const;
    const std::string& getName() const;
    const ContactInfo& getContacts() const;
    const Date& getBirthDate() const;

    /** @brief Люди равны, если совпадает id. */
    bool operator==(const Person& other) const;
    bool operator!=(const Person& other) const;

protected:
    int id_;
    std::string name_;
    ContactInfo contacts_;
    Date birthDate_;
};
