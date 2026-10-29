#pragma once
#include "service/RepairFirm.h"

/**
 * @brief Готовая фирма для демонстрации: клиенты, сотрудники всех типов, склад, прайс,
 * мастерская и два заказа в работе (один в ремонте, один готов к выдаче).
 * «Сегодня» в демо — 27.10.2026.
 */
RepairFirm createDemoFirm();

/** @brief Категории техники демо-фирмы. Меню создаёт устройства с ними, чтобы совпадали со специализациями мастеров. */
DeviceCategory phoneCategory();
DeviceCategory laptopCategory();
DeviceCategory applianceCategory();
