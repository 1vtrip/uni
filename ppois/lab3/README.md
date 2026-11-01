# Лабораторная работа №3. Система управления файлами

Формат строки: `Класс | число полей | число методов | связанные классы`.

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| AccessControlEntry | 4 | 3 | Permission |
| AccessControlList | 2 | 3 | AccessControlEntry, Permission |
| Account | 8 | 11 | Credentials, PasswordPolicy, Timestamp |
| AdminAccount | 2 | 11 | Credentials, Timestamp |
| ArchiveEntry | 3 | 4 | DataSize |
| ArchiveFile | 2 | 7 | ArchiveEntry, Permission, Timestamp |
| AuditLog | 3 | 5 | AuditRecord, Timestamp |
| AuditRecord | 7 | 2 | Timestamp |
| AuthService | 3 | 7 | Account, PasswordPolicy, Session, Timestamp |
| Backup | 6 | 8 | DataSize, Timestamp |
| BackupManager | 3 | 8 | Backup, BackupPolicy, DataSize, Timestamp |
| BackupPolicy | 4 | 3 | DataSize, FsEntry, Timestamp |
| Checksum | 1 | 3 | — |
| Compressor | 2 | 5 | DataSize, File, MimeType |
| Credentials | 3 | 4 | Checksum |
| DataSize | 1 | 6 | — |
| Directory | 2 | 10 | Permission, Timestamp |
| ExecutableFile | 3 | 6 | DataSize, Permission, Timestamp |
| File | 3 | 4 | Checksum, DataSize, MimeType, Permission, Timestamp |
| FileLock | 5 | 5 | Timestamp |
| FileSystem | 29 | 90 | AccessControlList, AdminAccount, ArchiveFile, AuditLog, AuthService, BackupManager, Compressor, DataSize, Directory, ExecutableFile, FsEntry, GuestAccount, ImageFile, LockManager, NotificationService, Permission, RecycleBin, SearchQuery, SearchResult, ServiceAccount, ShareManager, StorageDevice, SuperUser, Symlink, TextFile, Timestamp, UserAccount, UserGroup, VersionHistory |
| FileVersion | 6 | 2 | Checksum, Timestamp |
| FsEntry | 8 | 8 | Metadata, Permission, Timestamp |
| GuestAccount | 1 | 8 | Credentials, Timestamp |
| IExpirable | 0 | 0 | Timestamp |
| ImageFile | 4 | 8 | Permission, Timestamp |
| IPrintable | 0 | 0 | — |
| ISizeable | 0 | 0 | — |
| LockManager | 1 | 7 | FileLock, Timestamp |
| Metadata | 3 | 9 | Tag |
| MimeType | 2 | 6 | — |
| Notification | 5 | 1 | Timestamp |
| NotificationService | 1 | 8 | Notification, ShareLink, Timestamp, UserAccount |
| PasswordPolicy | 4 | 7 | — |
| Path | 2 | 9 | — |
| Permission | 1 | 10 | — |
| Quota | 4 | 7 | DataSize |
| RecycleBin | 3 | 8 | DataSize, Timestamp, TrashItem |
| ReportGenerator | 2 | 10 | FileSystem, IExpirable, ISizeable, Timestamp |
| SearchQuery | 8 | 9 | DataSize, FsEntry, Timestamp |
| SearchResult | 3 | 5 | DataSize, FsEntry |
| ServiceAccount | 3 | 9 | Credentials, Timestamp |
| Session | 6 | 4 | Timestamp |
| ShareLink | 9 | 5 | Permission, Timestamp |
| ShareManager | 2 | 8 | Permission, ShareLink, Timestamp |
| StorageDevice | 6 | 5 | DataSize |
| SuperUser | 0 | 5 | Credentials, Timestamp |
| Symlink | 2 | 7 | Permission, Timestamp |
| Tag | 2 | 4 | — |
| TextFile | 1 | 9 | Permission, Timestamp |
| Timestamp | 5 | 9 | — |
| TrashItem | 7 | 2 | DataSize, Timestamp |
| UserAccount | 2 | 6 | Credentials, Quota, Timestamp |
| UserGroup | 3 | 6 | — |
| VersionHistory | 3 | 9 | FileVersion, Timestamp |
| Volume | 5 | 2 | Path |

### Исключения (18)

- FileSystemException
- EntryNotFoundException
- InvalidPathException
- EntryAlreadyExistsException
- DirectoryNotEmptyException
- InvalidOperationException
- AccessDeniedException
- AuthenticationException
- WeakPasswordException
- SessionExpiredException
- QuotaExceededException
- InsufficientStorageException
- ReadOnlyVolumeException
- FileLockedException
- VersionNotFoundException
- ShareLinkExpiredException
- CorruptedFileException
- BackupFailedException

### Итоговая статистика

| Показатель | Значение |
|---|---|
| Поля | 210 |
| Поведения | 417 |
| Ассоциации | 126 |
| Исключения | 18 |
