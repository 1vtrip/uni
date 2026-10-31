#include "fs/Symlink.h"
#include "core/Path.h"
#include "exceptions/Exceptions.h"

Symlink::Symlink(int id, const std::string& name, int parentId, int ownerId,
                 const Permission& permission, const Timestamp& now,
                 const std::string& targetPath, int targetId)
    : FsEntry(id, name, parentId, ownerId, permission, now),
      targetPath_(Path::parse(targetPath).toString()), targetId_(targetId) {
    if (targetId < 0) throw InvalidOperationException("ID цели ссылки не может быть отрицательным");
}

std::string Symlink::typeName() const { return "Ссылка"; }
DataSize Symlink::size() const { return DataSize((long long)targetPath_.size()); }
char Symlink::typeChar() const { return 'l'; }

std::string Symlink::preview(int) const {
    return "→ " + targetPath_ + (isBroken() ? " (битая)" : "");
}

/** @brief Битая ссылка бросает исключение, а не возвращает 0: иначе FileSystem стал бы искать элемент #0. */
int Symlink::resolve() const {
    if (isBroken()) throw EntryNotFoundException(targetPath_ + " (ссылка " + name_ + " битая)");
    return targetId_;
}

bool Symlink::isBroken() const { return targetId_ == 0; }
void Symlink::markBroken() { targetId_ = 0; }

/** @brief Путь разбирается до изменения полей: при ошибке ссылка остаётся прежней. */
void Symlink::retarget(int id, const std::string& path, const Timestamp& now) {
    if (id <= 0) throw InvalidOperationException("ID новой цели ссылки должен быть положительным");
    std::string normalized = Path::parse(path).toString();
    targetId_ = id;
    targetPath_ = normalized;
    touch(now);
}

const std::string& Symlink::getTargetPath() const { return targetPath_; }
int Symlink::getTargetId() const { return targetId_; }
