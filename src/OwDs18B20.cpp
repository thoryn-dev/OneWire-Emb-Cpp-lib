// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

#include <stdint.h>
#include "OwCore.h"
#include "OwDs18B20.h"

namespace OW{

Ds18B20::Ds18B20(Bus& interface) : Device::Device (interface) {

}

Ds18B20::SetConfigurationStatus Ds18B20::setConfiguration(bool saveEeprom, const uint8_t* romId){
	// prepare command and data sequence
	uint8_t commandSet[4] {
		Device::Command::WriteScratch,
		Data.configTemperatureHigh,
		Data.configTemperatureLow,
		static_cast<uint8_t>(((Data.configResolution - 9) << 5) + 0x1F)
	};
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
	for (uint8_t byteIndex = 0; byteIndex < sizeof(commandSet); ++byteIndex) {	// Loop to write byte to bus
		m_busInterface.sendByte(commandSet[byteIndex]);
	}
	// varify data transmission
	if (getConfiguration(romId) != GetConfigurationStatus::ReceivedSuccessfully) {
		return Ds18B20::SetConfigurationStatus::ConfigurationFailed;
	}
	// save data to EEPROM if selected
	if(saveEeprom){
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
		selectRom(romId);
		copyScratch(romId, Device::Command::NoPage);
	}
	return  Ds18B20::SetConfigurationStatus::ConfigurationSuccessful;
}

Ds18B20::GetConfigurationStatus Ds18B20::getConfiguration(const uint8_t* romId) {
	uint8_t receivedData[9];
	// perform 1-Wire reset
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return GetConfigurationStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return GetConfigurationStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	// select device via ROM-ID
	selectRom(romId);
	// send 1-Wire command
	m_busInterface.sendByte(Device::Command::ReadScratch);
	// receive data from scratch pad
	for (uint8_t i = 0; i < 9; ++i) {
		receivedData[i] = m_busInterface.receiveByte();
	}
	// verify CRC
	if (calculateCRC8(receivedData,9)) {
		return GetConfigurationStatus::CrcFailed;	// calculate CRC8 from data if unsuccessful return 0
	}
	Data.configTemperatureHigh = receivedData[2];			// save upper alarm threshold
	Data.configTemperatureLow = receivedData[3];			// save lower alarm threshold
	Data.configResolution = (receivedData[4] >> 5) + 9;   // save resolution
	return GetConfigurationStatus::ReceivedSuccessfully;
}

Ds18B20::ConvertTemperatureStatus Ds18B20::convertTemperature(bool waitConversation, const uint8_t* romId) {
	bool conversionDone = false;
	// perform 1-Wire reset
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return ConvertTemperatureStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return ConvertTemperatureStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	// select device via ROM-ID
	selectRom(romId);
	// start conversion
	m_busInterface.sendByte(Device::Command::ConvertTemperature);
	// wait on conversion if selected
	if(waitConversation){
		while(!conversionDone){
			conversionDone = m_busInterface.readBit();
		}
		return Ds18B20::ConvertTemperatureStatus::ConversionCompleted;
	}
	return Ds18B20::ConvertTemperatureStatus::ConversionStarted;
}

Ds18B20::ReceiveTemperatureStatus Ds18B20::receiveTemperature(const uint8_t* romId) {
	// perform 1-Wire reset
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return ReceiveTemperatureStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return ReceiveTemperatureStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	// select device via ROM-ID
	selectRom(romId);
	// read temperature value from scratch pad
	m_busInterface.sendByte(Device::Command::ReadScratch);	// command to read scratch pad from device
	for (uint8_t byteIndex = 0; byteIndex < 9; ++byteIndex) {						// receive data from sensor
		m_data[byteIndex] = m_busInterface.receiveByte();
	}
	// verify CRC
	if(calculateCRC8(m_data, 9)){                          // calculate CRC8 from data if successful return 1
		return ReceiveTemperatureStatus::CrcFailed;
	}
	// turn digital temperature in Millidegrees Celsius corresponds -55000 to +125000 m°C
	Data.valueTemperatureRaw = (m_data[1] << 8) | m_data[0];
	Data.valueTemperature = ((static_cast<int32_t>(Data.valueTemperatureRaw) * 1000) / 16);
	return ReceiveTemperatureStatus::ReceivedSuccessfully;
}

} // end namespace OW