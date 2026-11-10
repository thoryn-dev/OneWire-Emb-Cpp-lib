// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

#include <stdint.h>
#include "OwUtils.h"

namespace OWUTILS {

uint32_t calculateHumidity (uint16_t supplyVoltage, uint16_t sensorVoltage, uint32_t temperature){
	uint32_t relativeHumidity = (645161 * sensorVoltage/supplyVoltage - 103225); // humidity value in mrH
	relativeHumidity = relativeHumidity * 250 / ((1054600000 - 2160 * temperature) / 1000000); // corrected with temperature value
	return relativeHumidity;
}

uint32_t calculateCurrentDs2438 (int16_t senseVoltageRaw, uint16_t senseResitor){
	uint32_t current = senseVoltageRaw * 1000000 / (4096 * senseResitor);
	return current;
}
				
} // end namespace OWUTILS