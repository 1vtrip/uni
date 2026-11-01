# Лабораторная работа №2. Фирма по ремонту техники

Формат строки: `Класс | число полей | число методов | связанные классы`.

| Класс | Поля | Методы | Ассоциации (связанные классы) |
|---|---|---|---|
| Accountant | 1 | 6 | CashRegister, ContactInfo, Date, Money, Payment, PriceList, RepairOrder |
| Address | 4 | 2 | — |
| Appointment | 7 | 6 | Date |
| Brand | 3 | 0 | — |
| CashRegister | 2 | 3 | Money |
| Client | 3 | 9 | ContactInfo, Date, Invoice, LoyaltyCard, Money, RepairOrder |
| ContactInfo | 3 | 3 | Address |
| CorporateClient | 3 | 7 | ContactInfo, Date, Money, ServiceContract |
| Courier | 3 | 6 | ContactInfo, Date, Money |
| Date | 3 | 8 | — |
| Defect | 3 | 2 | — |
| Delivery | 6 | 4 | Address, Courier |
| Device | 5 | 8 | Brand, Date, DeviceCategory |
| DeviceCategory | 3 | 1 | Money |
| Diagnosis | 4 | 7 | Date, Defect, PriceList |
| Discount | 5 | 3 | Date, Money |
| Employee | 4 | 7 | ContactInfo, Date, Device, Money, Schedule, Shift |
| EmployeeClient | 0 | 5 | ContactInfo, Date, Money |
| HomeAppliance | 3 | 6 | Address, Brand, Date, DeviceCategory |
| ICertifiable | 0 | 0 | Date |
| Invoice | 8 | 8 | Date, Discount, InvoiceLine |
| InvoiceLine | 3 | 1 | Money |
| IPayable | 0 | 0 | — |
| IPrintable | 0 | 0 | — |
| Laptop | 3 | 6 | Brand, Date, DeviceCategory |
| LoyaltyCard | 4 | 4 | Money |
| Manager | 2 | 6 | ContactInfo, Date, Discount, Money, RepairOrder, Technician |
| Money | 1 | 4 | — |
| Notification | 4 | 2 | Date |
| NotificationService | 1 | 5 | Appointment, Client, Date, Notification, Person, RepairOrder |
| OrderStatus | 1 | 5 | — |
| Payment | 7 | 3 | CashRegister, Date, Invoice, Money, PaymentMethod |
| PaymentMethod | 2 | 2 | Money |
| Person | 4 | 3 | ContactInfo, Date |
| PriceList | 3 | 5 | Money, ServiceItem |
| PurchaseOrder | 6 | 5 | Date, SparePart, Supplier, Warehouse |
| QualityCheck | 4 | 5 | — |
| Receptionist | 1 | 5 | Client, ContactInfo, Date, Device, Invoice, Money, RepairOrder |
| RepairFirm | 23 | 41 | Accountant, Address, Appointment, CashRegister, Client, ContactInfo, CorporateClient, Courier, Date, Delivery, Device, EmployeeClient, Invoice, Manager, Money, NotificationService, Payment, PaymentMethod, PriceList, PurchaseOrder, Receptionist, RepairOrder, Review, ServiceContract, Technician, Warehouse, Warranty, Workshop |
| RepairOrder | 12 | 12 | Date, Device, Diagnosis, Money, OrderStatus, PriceList, RepairTask |
| RepairTask | 6 | 6 | Device, PriceList, SparePart, UsedPart |
| ReportGenerator | 2 | 8 | Date, Employee, ICertifiable, Invoice, IPayable, RepairOrder, Technician, Warehouse |
| Review | 4 | 2 | — |
| Salary | 6 | 7 | Money |
| Schedule | 1 | 5 | Date, Shift |
| ServiceContract | 5 | 4 | Date, Money |
| ServiceItem | 4 | 1 | DeviceCategory, Money |
| Shift | 3 | 3 | Date |
| Smartphone | 3 | 6 | Brand, Date, DeviceCategory |
| SparePart | 5 | 4 | Device, Money |
| Specialization | 3 | 1 | DeviceCategory |
| StockItem | 4 | 6 | SparePart |
| Supplier | 6 | 3 | ContactInfo, Date, SparePart |
| Technician | 6 | 11 | ContactInfo, Date, Device, Money, Specialization |
| Tool | 4 | 7 | Date |
| UsedPart | 3 | 1 | Money |
| Warehouse | 2 | 7 | Address, SparePart, StockItem |
| Warranty | 4 | 5 | Date |
| WarrantyClaim | 6 | 4 | Date, Warranty |
| WashingMachine | 2 | 4 | Brand, Date, DeviceCategory |
| Workbench | 3 | 3 | — |
| Workshop | 4 | 9 | Address, Date, Tool, Workbench |

### Исключения (17)

- RepairFirmException
- InvalidClientDataException
- ClientNotFoundException
- OrderNotFoundException
- InvalidStatusTransitionException
- DeviceNotRepairableException
- IncompatiblePartException
- OutOfStockException
- TechnicianNotAvailableException
- ScheduleConflictException
- WorkbenchOccupiedException
- WarrantyExpiredException
- InvalidDiscountException
- InsufficientFundsException
- QualityCheckFailedException
- InvalidReviewException
- InvalidDateException

### Итоговая статистика

| Показатель | Значение |
|---|---|
| Поля | 240 |
| Поведения | 322 |
| Ассоциации | 175 |
| Исключения | 17 |
