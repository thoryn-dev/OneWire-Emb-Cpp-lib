// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

/**
 * @file OwManager.h
 *
 * @brief Implementation for 1-Wire device management
 *
 * The library provides an API for OW management.
 */

#pragma once
#include <stdint.h>
#include <avr/eeprom.h>

/**
 * @class OwDeviceManager
 * @brief Class for OW device management
 *
 * The class manages the OW devices located on the bus.
 */
class OwDeviceManager {
    public:
        OwDeviceManager();
        ~OwDeviceManager();
        /**
        * @brief Completely empty the device database
        *
        * @param[in] start Starting address of the EEPROM field
        */
        void clearDatabase(uint16_t);
        /**
        * @brief Finds the first vacancy in the OW database
        * 
        * Locates the first free memory location in the EEPROM device database.
        * It is used to store unknown devices from an OW search.
        *
        * @param[in] start Starting address of the EEPROM field
        * 
        * @return x = address of the first empty field
        * @return 0 = EEPROM memory full
        */
        uint16_t findFirstEmpty(uint16_t);
        /**
        * @brief Update EEPROM database for new devices
        * @param[in] start Start address of EEPROM field
        */
        void updateDatabase(uint8_t);
    private:
        uint16_t m_firstEmpty = 0;
};
