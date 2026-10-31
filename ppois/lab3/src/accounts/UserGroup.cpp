#include "accounts/UserGroup.h"
#include "exceptions/Exceptions.h"
#include "meta/Tag.h"

UserGroup::UserGroup(int id, const std::string& name) : id_(id), name_(name), memberIds_() {
    checkName(name);
}

void UserGroup::checkName(const std::string& name) {
    if (!Tag::isValidName(name)) throw InvalidOperationException("недопустимое имя группы \"" + name + "\"");
}

bool UserGroup::addMember(int userId) {
    if (contains(userId)) return false;
    memberIds_.push_back(userId);
    return true;
}

bool UserGroup::removeMember(int userId) {
    for (size_t i = 0; i < memberIds_.size(); ++i) {
        if (memberIds_[i] == userId) {
            memberIds_.erase(memberIds_.begin() + i);
            return true;
        }
    }
    return false;
}

bool UserGroup::contains(int userId) const {
    for (size_t i = 0; i < memberIds_.size(); ++i)
        if (memberIds_[i] == userId) return true;
    return false;
}

int UserGroup::size() const { return (int)memberIds_.size(); }
bool UserGroup::isEmpty() const { return memberIds_.empty(); }

void UserGroup::rename(const std::string& newName) {
    checkName(newName);
    name_ = newName;
}

/** @brief То же правило, что у Directory: 11–14 — «участников», иначе по последней цифре. */
std::string UserGroup::membersWord(int n) {
    int lastTwo = n % 100;
    if (lastTwo >= 11 && lastTwo <= 14) return "участников";
    int last = n % 10;
    if (last == 1) return "участник";
    if (last >= 2 && last <= 4) return "участника";
    return "участников";
}

int UserGroup::getId() const { return id_; }
const std::string& UserGroup::getName() const { return name_; }
const std::vector<int>& UserGroup::getMemberIds() const { return memberIds_; }

bool UserGroup::operator==(const UserGroup& o) const { return id_ == o.id_; }
bool UserGroup::operator!=(const UserGroup& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const UserGroup& g) {
    return os << g.name_ << " (#" << g.id_ << "): " << g.size() << " " << UserGroup::membersWord(g.size());
}
