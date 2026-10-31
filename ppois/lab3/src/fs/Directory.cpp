#include "fs/Directory.h"
#include "exceptions/Exceptions.h"

Directory::Directory(int id, const std::string& name, int parentId, int ownerId,
                     const Permission& permission, const Timestamp& now, int maxChildren)
    : FsEntry(id, name, parentId, ownerId, permission, now), childIds_(), maxChildren_(maxChildren) {
    if (maxChildren <= 0) throw InvalidOperationException("в папке должно помещаться хотя бы 1 элемент");
}

std::string Directory::typeName() const { return "Папка"; }
DataSize Directory::size() const { return DataSize::fromKB(4); }
bool Directory::isDirectory() const { return true; }
char Directory::typeChar() const { return 'd'; }

std::string Directory::preview(int) const {
    return "[Папка, " + std::to_string(childCount()) + " " + itemsWord(childCount()) + "]";
}

/**
 * @brief 11–14 всегда «элементов» (одиннадцать, двенадцать...), поэтому они проверяются первыми.
 * Иначе решает последняя цифра: 1 — «элемент», 2–4 — «элемента», остальное — «элементов».
 */
std::string Directory::itemsWord(int n) {
    int lastTwo = n % 100;
    if (lastTwo >= 11 && lastTwo <= 14) return "элементов";
    int last = n % 10;
    if (last == 1) return "элемент";
    if (last >= 2 && last <= 4) return "элемента";
    return "элементов";
}

void Directory::addChild(int id) {
    if (hasChild(id)) throw EntryAlreadyExistsException("#" + std::to_string(id) + " в папке " + name_);
    if (isFull()) throw InvalidOperationException("папка " + name_ + " заполнена ("
                                                  + std::to_string(maxChildren_) + " " + itemsWord(maxChildren_) + ")");
    childIds_.push_back(id);
}

bool Directory::removeChild(int id) {
    for (size_t i = 0; i < childIds_.size(); ++i) {
        if (childIds_[i] == id) {
            childIds_.erase(childIds_.begin() + i);
            return true;
        }
    }
    return false;
}

bool Directory::hasChild(int id) const {
    for (size_t i = 0; i < childIds_.size(); ++i)
        if (childIds_[i] == id) return true;
    return false;
}

int Directory::childCount() const { return (int)childIds_.size(); }
bool Directory::isEmpty() const { return childIds_.empty(); }
bool Directory::isFull() const { return childCount() >= maxChildren_; }
const std::vector<int>& Directory::getChildIds() const { return childIds_; }
int Directory::getMaxChildren() const { return maxChildren_; }
