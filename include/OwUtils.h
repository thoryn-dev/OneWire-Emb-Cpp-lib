// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

#pragma once
#include <stdint.h>

namespace OWUTILS {

    /**
    * @brief calculate humidity (Honeywell HIH-4030/31)
    * 
    * The function calculates the relative humidity using the supply voltage
    * and the sensor voltage, in accordance with the formula specified in the
    * HIH 4030/31 datasheet. In addition, a correction for the actual ambient
    * temperature is applied.
    * 
    * Formular for humidity: \f$V_{OUT} = V_{SUPPLY} * (0,0062 * rH + 0,16)\f$
    * 
    * Formular for temperature correction: \f$rH_{TRUE} = rH / 1,0546 - 0,00216 * T_{AMB}\f$
    * 
    * @note This function uses the first approximation. For a more accurate
    * calculation, the data from the calibration certificate must be used.
    * 
    * @note The factors in the formula were adjusted to make the fullest
    * possible use of the 32-bit data type in order to minimize rounding errors.
    * 
    * @param[in] supplyVoltage  Supply voltage of humidity sensor [mV]
    * @param[in] sensorVoltage  voltage from humidity sensor [mV]
    * @param[in] temperature    Ambient temperature [m°C]
    * 
    * @return Calculated humidity value
    */
	uint32_t calculateHumidity (uint16_t, uint16_t, uint32_t);
    /**
    * @brief calculate current DS2438
    * 
    * This function calculates the physical current from the DS2438's raw
    * value, taking into account the shunt resistor used.
    * 
    * Formular for humidity: \f$I = senseVoltageRaw / (4096 * R_{SENS})\f$
    * 
    * @note The factors in the formula were adjusted to make the fullest
    * possible use of the 16-bit data type in order to minimize rounding errors.
    * 
    * @todo validate the code for funcionality
    * 
    * @param[in] senseVoltageRaw    sense voltage from DS2438 in RAW format
    * @param[in] senseResitor       sense Resitor in mOhms
    * 
    * @return Calculated current value
    */
	uint32_t calculateCurrentDs2438 (int16_t, uint16_t);

} // end namespace OWUTILS