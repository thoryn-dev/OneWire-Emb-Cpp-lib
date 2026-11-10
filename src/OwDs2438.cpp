// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

/**
* @file OwDs2438.cpp
*/
#include <stdint.h>
#include "OwCore.h"
#include "OwDs2438.h"

namespace OW{

Ds2438::Ds2438(Bus& interface) : Device::Device (interface) {
}

Ds2438::SetConfigurationStatus Ds2438::setConfiguration(bool saveEeprom, const uint8_t* romId) {
	// prepare command and data sequence
	uint8_t commandSet[3] {
		Device::Command::WriteScratch,		// 1-Wire command
		0,									// target page 0
		0									// configuration register
	};
	commandSet[2] |= (Data.configIad << 0);
	commandSet[2] |= (Data.configCa  << 1);
	commandSet[2] |= (Data.configEe  << 2);
	commandSet[2] |= (Data.configAd  << 3);
	// perform 1-Wire reset
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return SetConfigurationStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return SetConfigurationStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	// select device via ROM-ID
	selectRom(romId);
	// send command and data sequence
	for (uint8_t byteIndex = 0; byteIndex < sizeof(commandSet); ++byteIndex) {
		m_busInterface.sendByte(commandSet[byteIndex]);
	}
	return SetConfigurationStatus::ConfigurationSuccessful;
}

Ds2438::ConvertVoltageStatus Ds2438::convertVoltage(bool waitConversation, const uint8_t* romId) {
	// perform 1-Wire reset
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return Ds2438::ConvertVoltageStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return Ds2438::ConvertVoltageStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	// select device via ROM-ID
	selectRom(romId);
	m_busInterface.sendByte(Device::Command::ConvertVoltage);
	return Ds2438::ConvertVoltageStatus::StartedConversion;
}

Ds2438::ConvertTemperatureStatus Ds2438::convertTemperature(bool waitConversation, const uint8_t* romId) {
	// perform 1-Wire reset
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return Ds2438::ConvertTemperatureStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return Ds2438::ConvertTemperatureStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	// select device via ROM-ID
	selectRom(romId);
	// send 1-Wire command
	m_busInterface.sendByte(Device::Command::ConvertTemperature);
	return Ds2438::ConvertTemperatureStatus::ConversionStarted;
}

Ds2438::ReceiveVoltageStatus Ds2438::receiveVoltage(const uint8_t* romId) {
	// perform 1-Wire reset
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return ReceiveVoltageStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return ReceiveVoltageStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	// select device via ROM-ID
	selectRom(romId);
	m_busInterface.sendByte(Device::Command::ReadScratch);	// command to read scratch pad from device
	m_busInterface.sendByte(Device::Command::Page0);
	for (uint8_t byteIndex = 0; byteIndex < 9; ++byteIndex) {								// receive data from sensor
		m_data[byteIndex] = m_busInterface.receiveByte();
	}
	if(calculateCRC8(m_data, 9)){
		return ReceiveVoltageStatus::CrcFailed;
	}
	// process temperature value
	Data.valueTemperatureRaw = (m_data[2] << 8) | m_data[1];
	Data.valueTemperature = ((static_cast<int32_t>(Data.valueTemperatureRaw) * 1000) / 256);
	// process voltage value
	Data.valueVoltage = static_cast<uint16_t>((m_data[4] << 8) | m_data[3])*10;	// range of 0 to 1000 digits corresponds 0 to 10V
	// process current value
	Data.valueCurrentRaw = (m_data[6] << 8) | m_data[5];
	return ReceiveVoltageStatus::successfullyReceived; // calculate CRC8 from data if successful return 1
}

} // end namespace OW
