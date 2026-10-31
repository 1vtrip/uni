#include "fs/TextFile.h"

TextFile::TextFile(int id, const std::string& name, int parentId, int ownerId,
                   const Permission& permission, const Timestamp& now, const std::string& content)
    : File(id, name, parentId, ownerId, permission, now, MimeType::fromExtension("txt")),
      content_(content) {
    updateChecksum(content_);              // protected-метод File
}

std::string TextFile::typeName() const { return "Текстовый файл"; }
DataSize TextFile::size() const { return DataSize((long long)content_.size()); }

std::string TextFile::preview(int maxChars) const {
    if ((int)content_.size() <= maxChars) return content_;
    return content_.substr(0, maxChars) + "...";
}

void TextFile::write(const std::string& text, const Timestamp& now) {
    content_ = text;
    updateChecksum(content_);
    touch(now);
}

void TextFile::append(const std::string& text, const Timestamp& now) { write(content_ + text, now); }

int TextFile::lineCount() const {
    if (content_.empty()) return 0;
    int lines = 1;
    for (size_t i = 0; i < content_.size(); ++i)
        if (content_[i] == '\n') ++lines;
    return lines;
}

/** @brief Слово засчитывается в момент перехода «пробел -> не пробел», поэтому лишние пробелы не мешают. */
int TextFile::wordCount() const {
    int words = 0;
    bool inWord = false;
    for (size_t i = 0; i < content_.size(); ++i) {
        bool space = content_[i] == ' ' || content_[i] == '\n' || content_[i] == '\t';
        if (!space && !inWord) ++words;
        inWord = !space;
    }
    return words;
}

/** @brief После находки поиск продолжается с конца найденного, поэтому вхождения не перекрываются. */
int TextFile::countOccurrences(const std::string& what) const {
    if (what.empty()) return 0;
    int count = 0;
    for (size_t pos = content_.find(what); pos != std::string::npos;
         pos = content_.find(what, pos + what.size()))
        ++count;
    return count;
}

/**
 * @brief Результат собирается в отдельной строке: при замене на месте позиции бы сдвигались,
 * а если to содержит from, поиск мог бы зациклиться.
 */
int TextFile::replaceAll(const std::string& from, const std::string& to, const Timestamp& now) {
    if (from.empty()) return 0;
    std::string result;
    int count = 0;
    size_t start = 0, pos;
    while ((pos = content_.find(from, start)) != std::string::npos) {
        result += content_.substr(start, pos - start) + to;
        start = pos + from.size();
        ++count;
    }
    result += content_.substr(start);
    if (count > 0) write(result, now);
    return count;
}

const std::string& TextFile::getContent() const { return content_; }
