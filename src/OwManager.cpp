// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

#include "OwManager.h"

OwDeviceManager::OwDeviceManager() {
}

void OwDeviceManager::clearDatabase(uint16_t start) {
    uint8_t nullFilledData[] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t i = 0;
    for (i = start; i < (start + 255); ++i) {
        do {} while (!eeprom_is_ready());
        eeprom_update_block((uint8_t*)nullFilledData, (uint16_t*)(i * 8), 8);
    }
}

uint16_t OwDeviceManager::findFirstEmpty(uint16_t start) {
    uint8_t data;
    uint8_t* p = &data;
    uint16_t i = 0;
    for (i = start; i < (start + 41); ++i) {
        do {} while (!eeprom_is_ready());
        eeprom_read_block((uint8_t*)p, (uint16_t*)(i * 8), 1);
        if (data == 0) {
            m_firstEmpty = i;
            return m_firstEmpty;
        }
    }
    return 0;
}

void updateDatabase(uint8_t start) {
    
}