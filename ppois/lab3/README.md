# Лабораторная работа №3 по ППОИС: «Система управления файлами»

**Вариант:** система управления файлами (C++17).  
**Автор:** ____________________, **группа:** ________

## 1. Предметная область

Программа моделирует **виртуальную файловую систему** в памяти: на диск ничего не пишется, «файл» — это объект с именем, размером и содержимым. Система умеет:

- дерево папок и файлов разных типов (текст, изображение, исполняемый, архив) и символические ссылки;
- создание, переименование, перемещение, копирование, удаление в корзину и восстановление;
- пользователей, гостей, администраторов, суперпользователя и служебную учётную запись, группы, вход по паролю и сессии;
- права доступа в стиле Unix (`rwxr-x---`) и списки доступа (ACL), где запрет важнее разрешения;
- квоты пользователей и учёт места на устройстве;
- версии текстовых файлов с откатом и проверкой целостности, блокировки Shared/Exclusive;
- ссылки общего доступа со сроком и лимитом переходов, поиск по шаблону `*`/`?`, архивацию и резервное копирование;
- журнал аудита, уведомления и отчёты о месте, квотах, действиях и безопасности.

## 2. Как хранится дерево без указателей

- У каждого элемента есть уникальный `id_` и `parentId_` — ID папки, в которой он лежит. У корня `/` `id_ = 1`, `parentId_ = 0`.
- У папки есть `std::vector<int> childIds_` — ID её элементов. Имён и размеров детей папка не видит.
- Все объекты живут в `FileSystem` в отдельных векторах по типу: `std::vector<Directory>`, `std::vector<TextFile>`, `std::vector<ImageFile>` и т. д.
- `const FsEntry& FileSystem::entry(int id)` ищет объект по всем векторам и возвращает **ссылку на базовый класс**: виртуальные `size()`, `preview()`, `print()` вызывают версию настоящего типа. Это полиморфизм без указателей.
- `FileSystem::resolve(path)` разбирает путь классом `Path` и идёт от корня по `childIds_`; `pathOf(id)` поднимается по `parentId_` до корня.
- Размер папки со всем содержимым считает `FileSystem::directorySize(id)` рекурсией по ID.
- Срезки нет: `FsEntry` и `File` абстрактные и по значению нигде не хранятся, в векторах лежат только конечные типы.
- Учётные записи хранятся так же: по вектору на тип, наружу — `const Account&`.

## 3. Сборка и запуск

```bash
cmake -B build && cmake --build build
./build/fms          # меню с готовой демо-системой
./build/fms --test   # самопроверка всех этапов: [OK]/[FAIL] и итог
```

Демо-пользователи (пароли печатаются и при запуске):

| Логин | Пароль | Роль |
| --- | --- | --- |
| `ivan` | `Secret123` | пользователь, группа students |
| `petr` | `Pass12345` | пользователь, группа students |
| `guest` | `Visit1234` | гость на 7 дней, квота 50 MB |
| `root` | `Admin1234` | администратор уровня 2 |
| `boss` | `Chief12345` | суперпользователь (ромб) |
| `backup` | `Backup2026!` | служебная учётная запись |

Меню выглядит как простой файловый менеджер: в заголовке `ivan@fms:/home/ivan  [квота 0% | 01.11.2026 11:00]`, относительные пути считаются от текущей папки (`cd docs`, `cd ..`). Пункты: вход и пароль, `ls`/`cd`, создание, свойства и preview, текст и версии, переименование и права (rwx, ACL), корзина, блокировки, ссылки, поиск, архив и резервные копии, администрирование, отчёты (в том числе сроки через `IExpirable` и размеры через `ISizeable`), уведомления, перемотка времени, демонстрация всех 18 исключений (на копии системы).

## 4. Иерархии наследования

### Учётные записи (ромб)
```
IPrintable (интерфейс)
└── Account (абстрактный)
    ├── UserAccount               : virtual Account
    │   ├── GuestAccount          [final]  + IExpirable
    │   └──────────────┐
    ├── AdminAccount              : virtual Account
    │   └──────────────┤
    │                  ▼
    │          SuperUser          : UserAccount, AdminAccount  [final]
    └── ServiceAccount            [final]
```

Благодаря `virtual` в `SuperUser` один `Account` (один ID и логин). Виртуальную базу создаёт самый производный класс, поэтому `Account(...)` первым вызывают конструкторы `SuperUser` и `GuestAccount`. `role()` и `print()` переопределены и в `UserAccount`, и в `AdminAccount`, поэтому `SuperUser` обязан переопределить их сам.

### Элементы файловой системы
```
IPrintable, ISizeable (интерфейсы)
└── FsEntry (абстрактный)
    ├── File (абстрактный: не переопределяет typeName)
    │   ├── TextFile
    │   ├── ImageFile
    │   ├── ExecutableFile        [final]
    │   └── ArchiveFile           [final]
    ├── Directory                 [final]
    └── Symlink                   [final]
```

### Интерфейсы

| Интерфейс | Метод | Кто реализует |
| --- | --- | --- |
| `IPrintable` | `void print(std::ostream&) const` + общий `operator<<` | Account, FsEntry, AuditRecord, Backup |
| `ISizeable` | `DataSize size() const` | FsEntry, RecycleBin, Backup |
| `IExpirable` | `bool isExpired(const Timestamp&) const` | Session, ShareLink, TrashItem, FileLock, GuestAccount |

`ReportGenerator::expiryStatus(const IExpirable&, ...)` и `sizeLine(const ISizeable&, ...)` работают с любым из этих классов.

### Исключения
```
std::runtime_error
└── FileSystemException
    ├── EntryNotFoundException
    ├── InvalidPathException
    ├── EntryAlreadyExistsException
    ├── DirectoryNotEmptyException
    ├── InvalidOperationException
    ├── AccessDeniedException
    ├── AuthenticationException
    ├── WeakPasswordException
    ├── SessionExpiredException
    ├── QuotaExceededException
    ├── InsufficientStorageException
    ├── ReadOnlyVolumeException
    ├── FileLockedException
    ├── VersionNotFoundException
    ├── ShareLinkExpiredException
    ├── CorruptedFileException
    └── BackupFailedException
```

## 5. Сводная таблица классов

Модель и интерфейсы, без исключений и классов `ui/`. Правила подсчёта — в разделе 8.

| Класс | Модуль | Наследует | Поля | Методы | Ассоциации |
| --- | --- | --- | ---: | ---: | ---: |
| AccessControlEntry | accounts | — | 4 | 3 | 1 |
| AccessControlList | accounts | — | 2 | 3 | 2 |
| Account | accounts | IPrintable | 8 | 11 | 3 |
| AdminAccount | accounts | Account | 2 | 11 | 2 |
| ArchiveEntry | meta | — | 3 | 4 | 1 |
| ArchiveFile | fs | File | 2 | 7 | 3 |
| AuditLog | ops | — | 3 | 5 | 2 |
| AuditRecord | ops | IPrintable | 7 | 2 | 1 |
| AuthService | service | — | 3 | 7 | 4 |
| Backup | ops | IPrintable, ISizeable | 6 | 8 | 2 |
| BackupManager | ops | — | 3 | 8 | 4 |
| BackupPolicy | ops | — | 4 | 3 | 3 |
| Checksum | core | — | 1 | 3 | 0 |
| Compressor | ops | — | 2 | 5 | 3 |
| Credentials | accounts | — | 3 | 4 | 1 |
| DataSize | core | — | 1 | 6 | 0 |
| Directory | fs | FsEntry | 2 | 10 | 2 |
| ExecutableFile | fs | File | 3 | 6 | 3 |
| File | fs | FsEntry | 3 | 4 | 5 |
| FileLock | ops | IExpirable | 5 | 5 | 1 |
| FileSystem | service | — | 29 | 90 | 29 |
| FileVersion | storage | — | 6 | 2 | 2 |
| FsEntry | fs | IPrintable, ISizeable | 8 | 8 | 3 |
| GuestAccount | accounts | UserAccount, IExpirable | 1 | 8 | 2 |
| IExpirable | interfaces | — | 0 | 0 | 1 |
| ImageFile | fs | File | 4 | 8 | 2 |
| IPrintable | interfaces | — | 0 | 0 | 0 |
| ISizeable | interfaces | — | 0 | 0 | 0 |
| LockManager | ops | — | 1 | 7 | 2 |
| Metadata | meta | — | 3 | 9 | 1 |
| MimeType | meta | — | 2 | 6 | 0 |
| Notification | service | — | 5 | 1 | 1 |
| NotificationService | service | — | 1 | 8 | 4 |
| PasswordPolicy | accounts | — | 4 | 7 | 0 |
| Path | core | — | 2 | 9 | 0 |
| Permission | meta | — | 1 | 10 | 0 |
| Quota | storage | — | 4 | 7 | 1 |
| RecycleBin | storage | ISizeable | 3 | 8 | 3 |
| ReportGenerator | service | — | 2 | 10 | 4 |
| SearchQuery | ops | — | 8 | 9 | 3 |
| SearchResult | ops | — | 3 | 5 | 2 |
| ServiceAccount | accounts | Account | 3 | 9 | 2 |
| Session | accounts | IExpirable | 6 | 4 | 1 |
| ShareLink | ops | IExpirable | 9 | 5 | 2 |
| ShareManager | ops | — | 2 | 8 | 3 |
| StorageDevice | storage | — | 6 | 5 | 1 |
| SuperUser | accounts | UserAccount, AdminAccount | 0 | 5 | 2 |
| Symlink | fs | FsEntry | 2 | 7 | 2 |
| Tag | meta | — | 2 | 4 | 0 |
| TextFile | fs | File | 1 | 9 | 2 |
| Timestamp | core | — | 5 | 9 | 0 |
| TrashItem | storage | IExpirable | 7 | 2 | 2 |
| UserAccount | accounts | Account | 2 | 6 | 3 |
| UserGroup | accounts | — | 3 | 6 | 0 |
| VersionHistory | storage | — | 3 | 9 | 2 |
| Volume | storage | — | 5 | 2 | 1 |
| **Итого: 56** | | | **210** | **417** | **126 пар** |

## 6. Классы по модулям

### core — базовые типы

#### Checksum

Контрольная сумма содержимого (хеш djb2). Нужна для проверки целостности файлов.

- **Поля (1):** `value_` (unsigned long long)
- **Поведение (3):** `of`, `matches`, `toHex`

#### DataSize

Объём данных. Хранится в байтах.

- **Поля (1):** `bytes_` (long long)
- **Поведение (6):** `fromKB`, `fromMB`, `fromGB`, `isZero`, `percentOf`, `toString`

#### Path

Путь в файловой системе, разобранный на части. "/home/ivan/a.txt" хранится как {"home", "ivan", "a.txt"} и absolute_ = true. Корень "/" — абсолютный путь без частей.

- **Поля (2):** `parts_` (std::vector<std::string>), `absolute_` (bool)
- **Поведение (9):** `parse`, `isValidName`, `toString`, `parent`, `fileName`, `extension`, `join`, `startsWith`, `isRoot`

#### Timestamp

Момент времени с точностью до минуты: дата и время суток.

- **Поля (5):** `year_` (int), `month_` (int), `day_` (int), `hour_` (int), `minute_` (int)
- **Поведение (9):** `isLeap`, `daysInMonth`, `isValid`, `toMinutes`, `minutesTo`, `daysTo`, `addMinutes`, `addDays`, `toDays`

### meta — права, типы и метаданные

#### ArchiveEntry

Запись о файле внутри архива: имя, размер до и после сжатия.

- **Поля (3):** `name_` (std::string), `originalSize_` (DataSize), `compressedSize_` (DataSize)
- **Поведение (4):** `ratio`, `saved`, `savedPercent`, `isCompressed`
- **Ассоциации (1):** DataSize

#### Metadata

Дополнительные сведения о файле: пары «ключ — значение» и метки. Пары хранятся в двух параллельных векторах: keys_[i] соответствует values_[i].

- **Поля (3):** `keys_` (std::vector<std::string>), `values_` (std::vector<std::string>), `tags_` (std::vector<Tag>)
- **Поведение (9):** `set`, `get`, `remove`, `has`, `addTag`, `removeTag`, `hasTag`, `indexOf`, `tagIndexOf`
- **Ассоциации (1):** Tag

#### MimeType

Тип содержимого файла в формате MIME: "text/plain", "image/png".

- **Поля (2):** `type_` (std::string), `subtype_` (std::string)
- **Поведение (6):** `fromExtension`, `isText`, `isImage`, `isArchive`, `isExecutable`, `toString`

#### Permission

Права доступа в стиле Unix: 9 битов rwx для владельца, группы и остальных. Хранятся в одном int, как в chmod: 0750 = rwxr-x---.

- **Поля (1):** `mode_` (int)
- **Поведение (10):** `fromString`, `forFile`, `forDirectory`, `forPrivate`, `has`, `allows`, `grant`, `revoke`, `toString`, `bit`

#### Tag

Метка файла или папки: имя и цвет, например #учёба(blue).

- **Поля (2):** `name_` (std::string), `color_` (std::string)
- **Поведение (4):** `isValidName`, `isValidColor`, `matches`, `setColor`

### fs — элементы файловой системы

#### ArchiveFile  (наследует `File`; `final`)

Архив: список записей о сжатых файлах. Размер архива — сумма сжатых размеров.

- **Поля (2):** `entries_` (std::vector<ArchiveEntry>), `format_` (std::string)
- **Поведение (7):** `typeName`, `size`, `preview`, `addEntry`, `contains`, `originalTotal`, `compressionRatio`
- **Ассоциации (3):** ArchiveEntry, Permission, Timestamp

#### Directory  (наследует `FsEntry`; `final`)

Папка. Хранит только ID своих элементов: имена и размеры детей она не видит, поэтому проверку имени и полный размер с содержимым делает FileSystem.

- **Поля (2):** `childIds_` (std::vector<int>), `maxChildren_` (int)
- **Поведение (10):** `typeName`, `size`, `isDirectory`, `typeChar`, `preview`, `addChild`, `removeChild`, `hasChild`, `isFull`, `itemsWord`
- **Ассоциации (2):** Permission, Timestamp

#### ExecutableFile  (наследует `File`; `final`)

Исполняемый файл: платформа, версия и цифровая подпись.

- **Поля (3):** `platform_` (std::string), `signed_` (bool), `version_` (std::string)
- **Поведение (6):** `typeName`, `preview`, `canRunOn`, `isSafeToRun`, `isNewerThan`, `parseVersion`
- **Ассоциации (3):** DataSize, Permission, Timestamp

#### File  (наследует `FsEntry`)

Общая часть всех файлов: размер, MIME-тип и контрольная сумма. Класс абстрактный, хотя в нём нет ни одного "= 0": он не переопределяет typeName(), и этот метод остаётся чисто виртуальным из FsEntry. Создаются только наследники.

- **Поля (3):** `size_` (DataSize), `mime_` (MimeType), `checksum_` (Checksum)
- **Поведение (4):** `print`, `extension`, `verify`, `updateChecksum`
- **Ассоциации (5):** Checksum, DataSize, MimeType, Permission, Timestamp

#### FsEntry  (наследует `IPrintable`, `ISizeable`)

Абстрактный элемент файловой системы: файл, папка или ссылка. Связь с деревом только через ID: parentId_ — папка, в которой лежит элемент.

- **Поля (8):** `id_` (int), `name_` (std::string), `parentId_` (int), `ownerId_` (int), `permission_` (Permission), `created_` (Timestamp), `modified_` (Timestamp), `metadata_` (Metadata)
- **Поведение (8):** `isDirectory`, `typeChar`, `preview`, `print`, `rename`, `moveTo`, `canAccess`, `isHidden`
- **Ассоциации (3):** Metadata, Permission, Timestamp

#### ImageFile  (наследует `File`)

Изображение: размеры в пикселях, формат и глубина цвета. Размер файла считается из них.

- **Поля (4):** `width_` (int), `height_` (int), `format_` (std::string), `colorDepth_` (int)
- **Поведение (8):** `typeName`, `size`, `preview`, `resize`, `aspectRatio`, `megapixels`, `thumbnailSize`, `isCompressedFormat`
- **Ассоциации (2):** Permission, Timestamp

#### Symlink  (наследует `FsEntry`; `final`)

Символическая ссылка: путь к цели и её ID. targetId_ = 0 — цель удалена, ссылка битая. Циклы ссылок (a -> b -> a) ссылка сама не видит, их ловит FileSystem.

- **Поля (2):** `targetPath_` (std::string), `targetId_` (int)
- **Поведение (7):** `typeName`, `size`, `typeChar`, `preview`, `resolve`, `isBroken`, `retarget`
- **Ассоциации (2):** Permission, Timestamp

#### TextFile  (наследует `File`)

Текстовый файл: содержимое хранится строкой, размер — её длина в байтах.

- **Поля (1):** `content_` (std::string)
- **Поведение (9):** `typeName`, `size`, `preview`, `write`, `append`, `lineCount`, `wordCount`, `countOccurrences`, `replaceAll`
- **Ассоциации (2):** Permission, Timestamp

### storage — место, версии, корзина

#### FileVersion

Снимок содержимого файла в какой-то момент: номер, автор, время и контрольная сумма.

- **Поля (6):** `number_` (int), `content_` (std::string), `created_` (Timestamp), `authorId_` (int), `checksum_` (Checksum), `comment_` (std::string)
- **Поведение (2):** `size`, `isIntact`
- **Ассоциации (2):** Checksum, Timestamp

#### Quota

Лимит дискового пространства пользователя.

- **Поля (4):** `userId_` (int), `limit_` (DataSize), `used_` (DataSize), `warnPercent_` (double)
- **Поведение (7):** `canAllocate`, `consume`, `release`, `remaining`, `usagePercent`, `isNearLimit`, `setLimit`
- **Ассоциации (1):** DataSize

#### RecycleBin  (наследует `ISizeable`)

Корзина: удалённые элементы хранятся retentionDays_ дней, общий размер не больше maxSize_. Корзина хранит только записи; сами объекты удаляет FileSystem по ID, которые возвращают add и purgeExpired.

- **Поля (3):** `items_` (std::vector<TrashItem>), `retentionDays_` (int), `maxSize_` (DataSize)
- **Поведение (8):** `add`, `restore`, `purgeExpired`, `size`, `contains`, `itemsOf`, `indexOf`, `oldestIndex`
- **Ассоциации (3):** DataSize, Timestamp, TrashItem

#### StorageDevice

Физический накопитель: SSD или HDD с ёмкостью и занятым местом.

- **Поля (6):** `id_` (int), `name_` (std::string), `type_` (std::string), `capacity_` (DataSize), `used_` (DataSize), `online_` (bool)
- **Поведение (5):** `allocate`, `free`, `freeSpace`, `usagePercent`, `estimateWriteSeconds`
- **Ассоциации (1):** DataSize

#### TrashItem  (наследует `IExpirable`)

Запись в корзине: что удалено, откуда, кем и до какого срока хранится.

- **Поля (7):** `entryId_` (int), `originalParentId_` (int), `originalPath_` (std::string), `deletedAt_` (Timestamp), `expiresAt_` (Timestamp), `deletedBy_` (int), `size_` (DataSize)
- **Поведение (2):** `isExpired`, `daysLeft`
- **Ассоциации (2):** DataSize, Timestamp

#### VersionHistory

История версий одного файла. Версии лежат по возрастанию номера, самая старая — первая. Хранится не больше maxVersions_ версий: лишние старые удаляет prune().

- **Поля (3):** `fileId_` (int), `versions_` (std::vector<FileVersion>), `maxVersions_` (int)
- **Поведение (9):** `addVersion`, `get`, `latest`, `rollbackTo`, `prune`, `hasVersion`, `totalSize`, `indexOf`
- **Ассоциации (2):** FileVersion, Timestamp

#### Volume

Том: часть дерева, смонтированная в mountPath_ и лежащая на устройстве deviceId_.

- **Поля (5):** `name_` (std::string), `mountPath_` (Path), `deviceId_` (int), `readOnly_` (bool), `mounted_` (bool)
- **Поведение (2):** `checkWritable`, `contains`
- **Ассоциации (1):** Path

### accounts — учётные записи и безопасность

#### AccessControlEntry

Одно правило ACL: кому (пользователю или группе), какое действие, разрешить или запретить. Флаг isGroup_ нужен, потому что ID пользователя и группы могут совпасть (пользователь #2 и группа #2).

- **Поля (4):** `principalId_` (int), `isGroup_` (bool), `access_` (Permission::Access), `allow_` (bool)
- **Поведение (3):** `appliesTo`, `matches`, `accessName`
- **Ассоциации (1):** Permission

#### AccessControlList

Список правил доступа к одному файлу или папке. Проверяется раньше обычных прав rwx. Запрет всегда важнее разрешения, поэтому результат не зависит от порядка правил.

- **Поля (2):** `entryId_` (int), `rules_` (std::vector<AccessControlEntry>)
- **Поведение (3):** `add`, `removeFor`, `check`
- **Ассоциации (2):** AccessControlEntry, Permission

#### Account  (наследует `IPrintable`)

Абстрактная учётная запись: логин, пароль (в виде хеша), группы и блокировка.

- **Поля (8):** `id_` (int), `login_` (std::string), `displayName_` (std::string), `credentials_` (Credentials), `groupIds_` (std::vector<int>), `created_` (Timestamp), `blocked_` (bool), `failedLogins_` (int)
- **Поведение (11):** `maxSessionMinutes`, `canManageUsers`, `canReadAnyFile`, `canLogin`, `print`, `checkPassword`, `changePassword`, `joinGroup`, `leaveGroup`, `isInGroup`, `unblock`
- **Ассоциации (3):** Credentials, PasswordPolicy, Timestamp

#### AdminAccount  (наследует `Account`)

Администратор: уровень 1–3 и список привилегий. Читать любые файлы может с уровня 2. Наследование virtual — ради ромба SuperUser, как у UserAccount.

- **Поля (2):** `adminLevel_` (int), `privileges_` (std::vector<std::string>)
- **Поведение (11):** `role`, `print`, `maxSessionMinutes`, `canManageUsers`, `canReadAnyFile`, `hasPrivilege`, `grantPrivilege`, `revokePrivilege`, `promote`, `demote`, `printAdminPart`
- **Ассоциации (2):** Credentials, Timestamp

#### Credentials

Данные для входа: логин, соль и хеш пароля. Сам пароль нигде не хранится. Создать можно только через create(): так пароль сразу превращается в хеш.

- **Поля (3):** `login_` (std::string), `salt_` (std::string), `passwordHash_` (Checksum)
- **Поведение (4):** `create`, `verify`, `change`, `hash`
- **Ассоциации (1):** Checksum

#### GuestAccount  (наследует `UserAccount`, `IExpirable`; `final`)

Временный гость: маленькая квота, короткая сессия, нельзя делиться файлами, есть срок действия.

- **Поля (1):** `expiresAt_` (Timestamp)
- **Поведение (8):** `role`, `print`, `maxSessionMinutes`, `defaultQuota`, `canShare`, `canLogin`, `isExpired`, `extend`
- **Ассоциации (2):** Credentials, Timestamp

#### PasswordPolicy

Требования к паролю: минимальная длина, цифра, заглавная буква, спецсимвол и запрет на логин внутри пароля.

- **Поля (4):** `minLength_` (int), `requireDigit_` (bool), `requireUpper_` (bool), `requireSpecial_` (bool)
- **Поведение (7):** `validate`, `strength`, `letterCount`, `hasDigit`, `hasUpper`, `hasSpecial`, `containsLogin`

#### ServiceAccount  (наследует `Account`; `final`)

Служебная учётная запись для программ (например, резервного копирования). Права задаются списком разрешённых операций; программа подтверждает себя API-ключом.

- **Поля (3):** `serviceName_` (std::string), `allowedOperations_` (std::vector<std::string>), `apiKey_` (std::string)
- **Поведение (9):** `role`, `print`, `maxSessionMinutes`, `canReadAnyFile`, `canPerform`, `allow`, `disallow`, `rotateKey`, `verifyKey`
- **Ассоциации (2):** Credentials, Timestamp

#### Session  (наследует `IExpirable`)

Сеанс работы пользователя после входа. Истекает, если закрыт или простой больше maxIdleMinutes_.

- **Поля (6):** `id_` (int), `userId_` (int), `started_` (Timestamp), `lastActivity_` (Timestamp), `maxIdleMinutes_` (int), `active_` (bool)
- **Поведение (4):** `touch`, `isExpired`, `minutesLeft`, `durationMinutes`
- **Ассоциации (1):** Timestamp

#### SuperUser  (наследует `UserAccount`, `AdminAccount`; `final`)

Администратор с личной домашней папкой и квотой. Ромб: UserAccount и AdminAccount наследуют Account виртуально, поэтому Account в SuperUser один — один ID и один логин.

- **Поля:** нет
- **Поведение (5):** `role`, `print`, `maxSessionMinutes`, `canReadAnyFile`, `defaultQuota`
- **Ассоциации (2):** Credentials, Timestamp

#### UserAccount  (наследует `Account`)

Обычный пользователь с домашней папкой и квотой. Наследование virtual: в ромбе SuperUser (UserAccount + AdminAccount) должен быть один Account, а не два.

- **Поля (2):** `homeDirId_` (int), `quota_` (Quota)
- **Поведение (6):** `role`, `print`, `defaultQuota`, `canShare`, `resetQuota`, `printUserPart`
- **Ассоциации (3):** Credentials, Quota, Timestamp

#### UserGroup

Группа пользователей (например, students). Участники хранятся по ID, без повторов.

- **Поля (3):** `id_` (int), `name_` (std::string), `memberIds_` (std::vector<int>)
- **Поведение (6):** `addMember`, `removeMember`, `contains`, `rename`, `checkName`, `membersWord`

### ops — блокировки, аудит, общий доступ, поиск, архивы, резервные копии

#### AuditLog

Журнал аудита: последние maxRecords_ действий всех пользователей, старые вытесняются.

- **Поля (3):** `records_` (std::vector<AuditRecord>), `maxRecords_` (int), `nextId_` (int)
- **Поведение (5):** `record`, `byUser`, `failures`, `failedLoginsSince`, `suspiciousUsers`
- **Ассоциации (2):** AuditRecord, Timestamp

#### AuditRecord  (наследует `IPrintable`)

Одна запись журнала аудита: кто, когда, что сделал, с чем и получилось ли.

- **Поля (7):** `id_` (int), `time_` (Timestamp), `userId_` (int), `action_` (std::string), `target_` (std::string), `success_` (bool), `details_` (std::string)
- **Поведение (2):** `toLine`, `print`
- **Ассоциации (1):** Timestamp

#### Backup  (наследует `IPrintable`, `ISizeable`)

Одна резервная копия: какие файлы в неё вошли, общий размер и статус. Статус — маленький конечный автомат: InProgress -> Completed или InProgress -> Failed; оба конечных состояния окончательные.

- **Поля (6):** `id_` (int), `created_` (Timestamp), `entryIds_` (std::vector<int>), `totalSize_` (DataSize), `status_` (Status), `failReason_` (std::string)
- **Поведение (8):** `addEntry`, `complete`, `fail`, `contains`, `isUsable`, `statusName`, `print`, `requireInProgress`
- **Ассоциации (2):** DataSize, Timestamp

#### BackupManager

Резервное копирование: запускает копии по политике, хранит историю и удаляет лишние.

- **Поля (3):** `policy_` (BackupPolicy), `backups_` (std::vector<Backup>), `nextId_` (int)
- **Поведение (8):** `isDue`, `runBackup`, `cleanupOld`, `latest`, `get`, `forRestore`, `storageUsed`, `latestUsableIndex`
- **Ассоциации (4):** Backup, BackupPolicy, DataSize, Timestamp

#### BackupPolicy

Правила резервного копирования: как часто, сколько копий хранить, брать ли скрытые, предел размера.

- **Поля (4):** `intervalDays_` (int), `keepCount_` (int), `includeHidden_` (bool), `maxSize_` (DataSize)
- **Поведение (3):** `isDue`, `nextDue`, `shouldInclude`
- **Ассоциации (3):** DataSize, FsEntry, Timestamp

#### Compressor

Модель сжатия: оценивает степень сжатия по типу файла и время работы по уровню 1–9.

- **Поля (2):** `algorithm_` (std::string), `level_` (int)
- **Поведение (5):** `estimateRatio`, `compress`, `estimateSeconds`, `setLevel`, `checkLevel`
- **Ассоциации (3):** DataSize, File, MimeType

#### FileLock  (наследует `IExpirable`)

Блокировка файла на время работы. Shared — «читаю, другим тоже можно читать», Exclusive — «пишу, больше никому нельзя». Действует до expiresAt_.

- **Поля (5):** `entryId_` (int), `userId_` (int), `mode_` (LockMode), `acquiredAt_` (Timestamp), `expiresAt_` (Timestamp)
- **Поведение (5):** `conflictsWith`, `isExpired`, `isHeldBy`, `refresh`, `modeName`
- **Ассоциации (1):** Timestamp

#### LockManager

Все блокировки всех файлов: выдаёт, снимает и проверяет, можно ли писать.

- **Поля (1):** `locks_` (std::vector<FileLock>)
- **Поведение (7):** `acquire`, `release`, `releaseAllFor`, `isLocked`, `canWrite`, `cleanupExpired`, `locksOf`
- **Ассоциации (2):** FileLock, Timestamp

#### SearchQuery

Условия поиска: шаблон имени ( и ?), расширение, размер, дата изменения, владелец, скрытые. Пустой фильтр не ограничивает. Сеттеры возвращают this, поэтому их можно писать цепочкой: q.byName(" .txt").bySize(min, max).byOwner(7).

- **Поля (8):** `namePattern_` (std::string), `extension_` (std::string), `minSize_` (DataSize), `maxSize_` (DataSize), `modifiedAfter_` (Timestamp), `useDateFilter_` (bool), `ownerId_` (int), `includeHidden_` (bool)
- **Поведение (9):** `wildcardMatch`, `matches`, `byName`, `byExtension`, `bySize`, `modifiedSince`, `byOwner`, `withHidden`, `lower`
- **Ассоциации (3):** DataSize, FsEntry, Timestamp

#### SearchResult

Результат поиска: ID найденных элементов и их размеры в двух параллельных векторах (ids_[i] и sizes_[i] — один элемент), плюс общий размер.

- **Поля (3):** `ids_` (std::vector<int>), `sizes_` (std::vector<DataSize>), `totalSize_` (DataSize)
- **Поведение (5):** `add`, `contains`, `sortBySizeDesc`, `top`, `sortDesc`
- **Ассоциации (2):** DataSize, FsEntry

#### ShareLink  (наследует `IExpirable`)

Ссылка для общего доступа к файлу: токен, доступ (чтение или запись), срок и лимит переходов. maxUses_ = 0 — переходов без ограничения.

- **Поля (9):** `token_` (std::string), `entryId_` (int), `createdBy_` (int), `access_` (Permission::Access), `createdAt_` (Timestamp), `expiresAt_` (Timestamp), `maxUses_` (int), `uses_` (int), `revoked_` (bool)
- **Поведение (5):** `isExpired`, `use`, `remainingUses`, `allowsWrite`, `expiryReason`
- **Ассоциации (2):** Permission, Timestamp

#### ShareManager

Все ссылки общего доступа: создаёт, находит по токену и отзывает.

- **Поля (2):** `links_` (std::vector<ShareLink>), `counter_` (int)
- **Поведение (8):** `create`, `find`, `contains`, `revoke`, `revokeAllFor`, `activeOf`, `indexOf`
- **Ассоциации (3):** Permission, ShareLink, Timestamp

### service — сервисы и FileSystem

#### AuthService

Вход, выход и проверка сессий. Учётные записи хранит FileSystem и передаёт сюда параметром; журнал аудита тоже ведёт FileSystem, здесь только проверки и исключения.

- **Поля (3):** `sessions_` (std::vector<Session>), `policy_` (PasswordPolicy), `nextSessionId_` (int)
- **Поведение (7):** `login`, `logout`, `requireSession`, `activeSessionsOf`, `cleanup`, `session`, `indexOf`
- **Ассоциации (4):** Account, PasswordPolicy, Session, Timestamp

#### FileSystem

Файловая система: владеет всеми объектами и связывает их по ID. Элементы дерева лежат в отдельном векторе на каждый конечный тип, а наружу отдаются ссылкой на базовый класс FsEntry — полиморфизм без указателей.

- **Поля (29):** `name_` (std::string), `now_` (Timestamp), `directories_` (std::vector<Directory>), `textFiles_` (std::vector<TextFile>), `images_` (std::vector<ImageFile>), `executables_` (std::vector<ExecutableFile>), `archives_` (std::vector<ArchiveFile>), `symlinks_` (std::vector<Symlink>), `users_` (std::vector<UserAccount>), `guests_` (std::vector<GuestAccount>), `admins_` (std::vector<AdminAccount>), `services_` (std::vector<ServiceAccount>), `superUsers_` (std::vector<SuperUser>), `groups_` (std::vector<UserGroup>), `device_` (StorageDevice), `acls_` (std::vector<AccessControlList>), `versions_` (std::vector<VersionHistory>), `trash_` (RecycleBin), `locks_` (LockManager), `audit_` (AuditLog), `shares_` (ShareManager), `backups_` (BackupManager), `auth_` (AuthService), `notifier_` (NotificationService), `compressor_` (Compressor), `nextEntryId_` (int), `nextAccountId_` (int), `nextGroupId_` (int), `sessionId_` (int)
- **Поведение (90):** `advanceTime`, `addUser`, `addGuest`, `addAdmin`, `addSuperUser`, `account`, `findAccount`, `login`, `logout`, `currentUserId`, `isLoggedIn`, `peekUserId`, `changePassword`, `createGroup`, `addToGroup`, `removeFromGroup`, `setQuota`, `setBlocked`, `extendGuest`, `setAclRule`, `clearAclRules`, `aclOf`, `entry`, `resolve`, `followLink`, `pathOf`, `stat`, `list`, `directorySize`, `createDirectory`, `createTextFile`, `createExecutable`, `createSymlink`, `createImage`, `readText`, `writeText`, `rollback`, `versionsOf`, `rename`, `move`, `copy`, `chmod`, `remove`, `restore`, `emptyTrash`, `archive`, `lock`, `unlock`, `share`, `openShared`, `readShared`, `writeShared`, `search`, `runBackup`, `verifyIntegrity`, `simulateCorruption`, `requireAdmin`, `accountIdOf`, `groupNamed`, `accountRef`, `checkNewLogin`, `createHome`, `directory`, `textFile`, `findChild`, `symlink`, `sharesGroup`, `allowedByRules`, `denialReason`, `file`, `addIfVisible`, `addIfBackedUp`, `checkAccess`, `hasQuota`, `quotaOwner`, `reserveSpace`, `freeSpace`, `checkCanCreate`, `childPath`, `foreignLockOwner`, `writeContent`, `historyOf`, `isInsideSubtree`, `isProtected`, `refreshLinks`, `freeName`, `isInTree`, `deleteForever`
- **Ассоциации (29):** AccessControlList, AdminAccount, ArchiveFile, AuditLog, AuthService, BackupManager, Compressor, DataSize, Directory, ExecutableFile, FsEntry, GuestAccount, ImageFile, LockManager, NotificationService, Permission, RecycleBin, SearchQuery, SearchResult, ServiceAccount, ShareManager, StorageDevice, SuperUser, Symlink, TextFile, Timestamp, UserAccount, UserGroup, VersionHistory

#### Notification

Уведомление пользователю: текст, канал ("system" или "email"), время и отметка о прочтении.

- **Поля (5):** `userId_` (int), `text_` (std::string), `channel_` (std::string), `time_` (Timestamp), `read_` (bool)
- **Поведение (1):** `format`
- **Ассоциации (1):** Timestamp

#### NotificationService

Отправка уведомлений и их история: квота, открытые ссылки, безопасность.

- **Поля (1):** `history_` (std::vector<Notification>)
- **Поведение (8):** `notify`, `quotaWarning`, `shareOpened`, `securityAlert`, `unreadFor`, `allFor`, `markAllRead`, `hasUnreadQuotaWarning`
- **Ассоциации (4):** Notification, ShareLink, Timestamp, UserAccount

#### ReportGenerator

Текстовые отчёты за период [from_, to_]: занятое место, квоты, действия пользователя, безопасность. Методы возвращают строки — печатает меню, в модели нет cout.

- **Поля (2):** `from_` (Timestamp), `to_` (Timestamp)
- **Поведение (10):** `diskUsage`, `quotaReport`, `activity`, `securityReport`, `expiryStatus`, `sizeLine`, `setPeriod`, `inPeriod`, `nameOf`, `pad`
- **Ассоциации (4):** FileSystem, IExpirable, ISizeable, Timestamp

### interfaces — интерфейсы

#### IExpirable

То, что имеет срок действия: сессия, ссылка, запись в корзине, блокировка, гостевой доступ. Пять не связанных между собой классов с одним общим поведением «истёк ли срок».

- **Поля:** нет
- **Ассоциации (1):** Timestamp

#### IPrintable

Объект, который умеет вывести себя в поток.

- **Поля:** нет

#### ISizeable

То, у чего есть размер на диске: элемент файловой системы, корзина, резервная копия.

- **Поля:** нет

## 7. Исключения

Все наследуют `FileSystemException`, поэтому меню ловит любое одним `catch (const FileSystemException&)`. Порядок `catch` в меню — от частного к общему: `SessionExpiredException` → `AccessDeniedException` → `QuotaExceededException` → `FileSystemException` → `std::exception`.

| Исключение | Когда | Где выбрасывается |
| --- | --- | --- |
| FileSystemException | база всех исключений системы; напрямую — неверная строка прав | `Permission::fromString` |
| EntryNotFoundException | нет элемента, учётной записи, ссылки, записи в корзине, группы или резервной копии; битая ссылка | `Symlink::resolve`, `RecycleBin::restore`, `BackupManager::get`, `ShareManager::find`, `FileSystem::account`, `FileSystem::accountRef`, `FileSystem::entry`, `FileSystem::followLink` и ещё 3 |
| InvalidPathException | путь пустой, относительный, не найден, в середине не папка, ".." выше корня, недопустимое имя | `Path::parse`, `FsEntry::rename`, `FileSystem::checkNewLogin`, `FileSystem::directory`, `FileSystem::resolve`, `FileSystem::list`, `FileSystem::checkCanCreate`, `FileSystem::rename` и ещё 1 |
| EntryAlreadyExistsException | имя занято в папке, логин занят, запись уже в архиве или корзине, группа уже есть | `ArchiveFile::addEntry`, `Directory::addChild`, `RecycleBin::add`, `FileSystem::checkNewLogin`, `FileSystem::checkCanCreate`, `FileSystem::rename`, `FileSystem::move`, `FileSystem::createGroup` |
| DirectoryNotEmptyException | удаление непустой папки | `FileSystem::remove` |
| InvalidOperationException | бессмысленная операция: перенос папки в саму себя, цикл ссылок, неверные размеры, версия, дата, срок, системная папка | `Timestamp::Timestamp`, `Metadata::set`, `Tag::Tag`, `Tag::setColor`, `Directory::Directory`, `Directory::addChild`, `ExecutableFile::parseVersion`, `ImageFile::ImageFile` и ещё 57 |
| AccessDeniedException | нет права по rwx, ACL или роли (запись, чтение, чужая ссылка, администрирование, chmod не владельцем) | `ShareManager::revoke`, `FileSystem::checkAccess`, `FileSystem::chmod`, `FileSystem::restore`, `FileSystem::share`, `FileSystem::writeShared`, `FileSystem::runBackup`, `FileSystem::requireAdmin` и ещё 2 |
| AuthenticationException | неверный логин или пароль, учётная запись заблокирована, гостевой срок истёк | `Account::changePassword`, `AuthService::login`, `FileSystem::login` |
| WeakPasswordException | пароль не проходит политику (все причины сразу) | `PasswordPolicy::validate` |
| SessionExpiredException | не выполнен вход, сессия истекла или закрыта | `Session::touch`, `AuthService::logout`, `AuthService::requireSession`, `AuthService::session`, `FileSystem::currentUserId` |
| QuotaExceededException | не хватает квоты пользователя | `Quota::consume`, `FileSystem::reserveSpace` |
| InsufficientStorageException | на устройстве нет места или оно отключено | `StorageDevice::allocate` |
| ReadOnlyVolumeException | запись в том только для чтения или не смонтированный том | `Volume::checkWritable` |
| FileLockedException | файл заблокирован другим пользователем | `LockManager::acquire`, `FileSystem::readText`, `FileSystem::writeContent`, `FileSystem::rename`, `FileSystem::move`, `FileSystem::copy`, `FileSystem::remove`, `FileSystem::lock` и ещё 1 |
| VersionNotFoundException | нет версии с таким номером, история пуста | `VersionHistory::get`, `VersionHistory::latest` |
| ShareLinkExpiredException | ссылка отозвана, истекла или кончились переходы | `ShareLink::use` |
| CorruptedFileException | контрольная сумма не совпадает, версия повреждена | `VersionHistory::rollbackTo`, `FileSystem::rollback`, `FileSystem::verifyIntegrity` |
| BackupFailedException | резервная копия больше предела, нет готовых копий, копия не готова к восстановлению | `BackupManager::runBackup`, `BackupManager::latest`, `BackupManager::forRestore` |

Каждое из 18 исключений можно увидеть в пункте меню 16, а `./build/fms --test` проверяет, что все они бросаются.

## 8. Итоговая статистика

| Требование | Нужно | Факт |
| --- | ---: | ---: |
| Классы | 50 | 56 |
| Поля | 150 | 210 |
| Поведения (методы с логикой) | 100 | 417 |
| Ассоциации | 30 | 126 |
| Собственные исключения | 12 | 18 |

**Как считалось:**

- **Классы** — модель и интерфейсы (`core`, `meta`, `fs`, `storage`, `accounts`, `ops`, `service`, `interfaces`); исключения и классы `ui/` не входят, `enum class` — не классы.
- **Поля** — только объявленные в самом классе (с суффиксом `_`), без констант `static const`.
- **Поведения** — методы с логикой, включая `override` со своей логикой; без конструкторов, деструкторов, `==`/`!=`/`<<`, чисто виртуальных объявлений, геттеров и однострочных сеттеров вида `return x_;` / `x_ = v;`.
- **Ассоциации** — свои классы, которые встречаются как тип поля или параметра метода; каждая пара классов считается один раз, наследование и типы `std::` не считаются.
