#include "people/Person.h"

Person::Person(int id, const std::string& name, const ContactInfo& contacts, const Date& birth)
    : id_(id), name_(name), contacts_(contacts), birthDate_(birth) {}

void Person::print(std::ostream& os) const {
    os << getRole() << " #" << id_ << ": " << name_;
}

int Person::age(const Date& today) const { return birthDate_.monthsTo(today) / 12; }

bool Person::updateContacts(const ContactInfo& contacts) {
    if (!contacts.validate()) return false;   // TODO(exc): исключение придёт из ContactInfo::validate
    contacts_ = contacts;
    return true;
}

int Person::getId() const { return id_; }
const std::string& Person::getName() const { return name_; }
const ContactInfo& Person::getContacts() const { return contacts_; }
const Date& Person::getBirthDate() const { return birthDate_; }

bool Person::operator==(const Person& other) const { return id_ == other.id_; }
bool Person::operator!=(const Person& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& os, const Person& p) {
    p.print(os);
    return os;
}
