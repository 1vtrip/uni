#pragma once
#include <stdexcept>
#include <string>

/** @brief База всех исключений системы: их можно поймать одним catch (const FileSystemException&). */
class FileSystemException : public std::runtime_error {
public:
    explicit FileSystemException(const std::string& msg) : std::runtime_error(msg) {}
};

/** @brief Файла, папки или другого объекта с таким путём или ID нет. */
class EntryNotFoundException : public FileSystemException {
public:
    explicit EntryNotFoundException(const std::string& what)
        : FileSystemException("Не найдено: " + what) {}
};

/** @brief Путь пустой, содержит недопустимое имя или ".." выходит выше корня. */
class InvalidPathException : public FileSystemException {
public:
    explicit InvalidPathException(const std::string& path)
        : FileSystemException("Недопустимый путь: " + path) {}
};

/** @brief В папке уже есть элемент с таким именем. */
class EntryAlreadyExistsException : public FileSystemException {
public:
    explicit EntryAlreadyExistsException(const std::string& name)
        : FileSystemException("Уже существует: " + name) {}
};

/** @brief Нельзя удалить папку, в которой что-то лежит. */
class DirectoryNotEmptyException : public FileSystemException {
public:
    explicit DirectoryNotEmptyException(const std::string& path)
        : FileSystemException("Папка не пуста: " + path) {}
};

/** @brief Операция бессмысленна: перенос папки в саму себя, цикл ссылок, неверный размер и т. п. */
class InvalidOperationException : public FileSystemException {
public:
    explicit InvalidOperationException(const std::string& reason)
        : FileSystemException("Недопустимая операция: " + reason) {}
};

/** @brief У пользователя нет прав на действие. */
class AccessDeniedException : public FileSystemException {
public:
    AccessDeniedException(const std::string& user, const std::string& action)
        : FileSystemException("Доступ запрещён: пользователь " + user + " не может " + action) {}
};

/** @brief Неверный логин или пароль. */
class AuthenticationException : public FileSystemException {
public:
    explicit AuthenticationException(const std::string& login)
        : FileSystemException("Не удалось войти как " + login + ": неверный логин или пароль") {}
};

/** @brief Пароль не проходит требования PasswordPolicy. */
class WeakPasswordException : public FileSystemException {
public:
    explicit WeakPasswordException(const std::string& reason)
        : FileSystemException("Слабый пароль: " + reason) {}
};

/** @brief Сессия закончилась или не существует. */
class SessionExpiredException : public FileSystemException {
public:
    explicit SessionExpiredException(int sessionId)
        : FileSystemException("Сессия #" + std::to_string(sessionId) + " истекла") {}
};

/** @brief У пользователя не хватает квоты под новые данные. */
class QuotaExceededException : public FileSystemException {
public:
    QuotaExceededException(const std::string& user, const std::string& need, const std::string& left)
        : FileSystemException("Квота пользователя " + user + " превышена: нужно "
              + need + ", свободно " + left) {}
};

/** @brief На накопителе нет места или он отключён. */
class InsufficientStorageException : public FileSystemException {
public:
    explicit InsufficientStorageException(const std::string& device)
        : FileSystemException("Недостаточно места на устройстве " + device) {}
};

/** @brief Запись в том, смонтированный только для чтения. */
class ReadOnlyVolumeException : public FileSystemException {
public:
    explicit ReadOnlyVolumeException(const std::string& volume)
        : FileSystemException("Том " + volume + " доступен только для чтения") {}
};

/** @brief Файл заблокирован другим пользователем. */
class FileLockedException : public FileSystemException {
public:
    FileLockedException(const std::string& path, int ownerId)
        : FileSystemException("Файл " + path + " заблокирован пользователем #"
              + std::to_string(ownerId)) {}
};

/** @brief В истории файла нет версии с таким номером. */
class VersionNotFoundException : public FileSystemException {
public:
    explicit VersionNotFoundException(int version)
        : FileSystemException("Версия " + std::to_string(version) + " не найдена") {}
};

/** @brief Ссылка общего доступа истекла или исчерпала число переходов. */
class ShareLinkExpiredException : public FileSystemException {
public:
    explicit ShareLinkExpiredException(const std::string& token)
        : FileSystemException("Ссылка " + token + " больше не действует") {}
};

/** @brief Контрольная сумма файла не совпадает с сохранённой. */
class CorruptedFileException : public FileSystemException {
public:
    explicit CorruptedFileException(const std::string& path)
        : FileSystemException("Файл повреждён: " + path) {}
};

/** @brief Резервное копирование или восстановление не удалось. */
class BackupFailedException : public FileSystemException {
public:
    explicit BackupFailedException(const std::string& reason)
        : FileSystemException("Ошибка резервного копирования: " + reason) {}
};
