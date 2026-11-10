// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

/**
* @file OwCore.cpp
*/

#include <stdint.h>
#include "hal/OwHal.h"
#include "OwCore.h"

namespace OW{

//-----------------------------------------------------------------------------
// Class Bus
//-----------------------------------------------------------------------------

Bus::Bus(Hal& interface) : m_interfaceHal(interface){

}

Bus::~Bus(){

}

Bus::ResetStatus Bus::reset() {
    bool resetDone = false;
	if(!m_interfaceHal.readPin()){				// Check whether the bus is high; if not, it might have a short circuit.
		return Bus::ResetStatus::PullUpFailed;
	}
	m_interfaceHal.lockInterrupt();
	m_interfaceHal.setPinLow();
	m_interfaceHal.waitUs(TIME_RESET_LOW);
	m_interfaceHal.releasePin();
	m_interfaceHal.waitUs(TIME_PRESENCE_HIGH);
	m_interfaceHal.waitUs(TIME_PRESENCE_LOW);
	resetDone = !(m_interfaceHal.readPin());
	m_interfaceHal.waitUs(TIME_RESET_HIGH);
	m_interfaceHal.waitUs(TIME_RECOVERY);
	m_interfaceHal.unlockInterrupt();
	if (resetDone) {
		return Bus::ResetStatus::ResetSuccessful;
	}
    return Bus::ResetStatus::ResetFailed;
}

bool Bus::readBit() {
    bool receivedBit = false;
	m_interfaceHal.lockInterrupt();
	m_interfaceHal.setPinLow();
	m_interfaceHal.waitUs(TIME_WRITE_1);
	m_interfaceHal.releasePin();
	m_interfaceHal.waitUs(TIME_READ);
	receivedBit = m_interfaceHal.readPin();
	m_interfaceHal.waitUs(TIME_SLOT_READ);
	m_interfaceHal.waitUs(TIME_RECOVERY);
	m_interfaceHal.unlockInterrupt();
    return receivedBit;
}

void Bus::writeBit(bool dataBit) {
	m_interfaceHal.lockInterrupt();
	m_interfaceHal.setPinLow();
	if (dataBit){
		m_interfaceHal.waitUs(TIME_WRITE_1);
		m_interfaceHal.releasePin();
		m_interfaceHal.waitUs(TIME_SLOT_1);
	}
	else{
		m_interfaceHal.waitUs(TIME_WRITE_0);
		m_interfaceHal.releasePin();
		m_interfaceHal.waitUs(TIME_SLOT_0);
	}
	m_interfaceHal.waitUs(TIME_RECOVERY);
	m_interfaceHal.unlockInterrupt();
}

uint8_t Bus::receiveByte() {
    uint8_t receivedByte = 0;
    for (uint8_t bitIndex = 0; bitIndex < 8; ++bitIndex) {
        receivedByte >>= 1;		// shift the receivedByte to get it ready for the next bit
        if (readBit())			
		receivedByte |= MSB;	// received bit is stored in the most significant bit
    }
    return receivedByte;
}

void Bus::sendByte(uint8_t dataByte) {
    for (uint8_t bitIndex = 0; bitIndex < 8; ++bitIndex) {
        writeBit(dataByte & LSB);	// send LSB first
        dataByte >>= 1;             // shift data bits for the next bit
    }
}

//-----------------------------------------------------------------------------
// Class Device
//-----------------------------------------------------------------------------

Device::Device(Bus& interface) : m_busInterface(interface) {

}

Device::~Device(){

}

Device::SearchDeviceStatus Device::searchDevice(bool alarmSearch) {
	switch (m_busInterface.reset())	{
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return SearchDeviceStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return SearchDeviceStatus::PullUpFailed;
			break;	
		default:
			break;
	}

	if(m_lastDeviceFlag){
		resetSearch();
		return SearchDeviceStatus::LastDeviceFound;
	}

	m_idBitIndex = 1;
	m_lastZero = 0;

	m_busInterface.sendByte(alarmSearch ? Command::AlarmSearchRom : Command::NormalSearchRom); // Selection: normal search / alarm search

	m_byteMask = 0;
	m_bitMask = 1;
			
	do {
		m_idBit = m_busInterface.readBit();
		m_cmpIdBit = m_busInterface.readBit();

		if (m_idBit && m_cmpIdBit) {
			resetSearch();
			return SearchDeviceStatus::SearchFailed;
		}

		if (m_idBit != m_cmpIdBit) {
			m_searchDirection = m_idBit;			
		}
		else {
			if (m_idBitIndex == m_lastDiscrepancy) {
				m_searchDirection = true;
			}
			else if(m_idBitIndex > m_lastDiscrepancy){
					m_searchDirection = false;
			}
			else if (m_idBitIndex < m_lastDiscrepancy) {
				m_searchDirection = ((m_romNo[m_byteMask] & m_bitMask) != 0);
			}
			if (!m_searchDirection) {
				m_lastZero = m_idBitIndex;
				if (m_lastZero < 9){
					m_lastFamilyDiscrepancy = m_lastZero;
				}
			}
		}
		if (m_searchDirection == true) {
			m_romNo[m_byteMask] |=  m_bitMask;
		}
		else {
			m_romNo[m_byteMask] &= ~m_bitMask;
		}
			
		m_busInterface.writeBit(m_searchDirection);

		++m_idBitIndex;

		m_bitMask <<= 1;
		if (m_bitMask == 0) {
			++m_byteMask;
			m_bitMask = 1;
		}

	} while(m_idBitIndex <= 64);
	
	m_lastDiscrepancy = m_lastZero;

	if(m_lastDiscrepancy == 0){
		m_lastDeviceFlag = true;
	}
	
	if ((calculateCRC8(m_romNo, 8)) == 0) {
		return SearchDeviceStatus::DeviceFound;
	}
	else {
		resetSearch();
		return SearchDeviceStatus::CRCFailed;
	}
}

void Device::resetSearch(){
	m_lastDiscrepancy = 0;
	m_lastFamilyDiscrepancy = 0;
	m_lastDeviceFlag = false;
	for(uint8_t byteIndex = 0; byteIndex < 8; ++byteIndex){
		m_romNo[byteIndex] = 0;
	}
}

void Device::getRomId(uint8_t* romId){
	for(uint8_t byteIndex = 0; byteIndex < 8; ++byteIndex){
		romId[byteIndex] = m_romNo[byteIndex];
	}
}

void Device::selectRom(const uint8_t* romId) {
	if(romId == nullptr){
		m_busInterface.sendByte(Device::Command::SkipRom);
		return;
	}
	m_busInterface.sendByte(Device::Command::MatchRom);
    for (uint8_t byteIndex = 0; byteIndex < 8; ++byteIndex) {	// Loop to write ROM-ID to bus
    	m_busInterface.sendByte(*(romId + byteIndex));
    }
	return;
}

uint8_t Device::calculateCRC8(const uint8_t *data, uint8_t len) {
	uint8_t crc8 = 0x00;
	while (len--) {
		uint8_t extract = *data++;
		for (uint8_t tempI = 8; tempI; tempI--) {
			uint8_t sum = (crc8 ^ extract) & 0x01;
			crc8 >>= 1;
			if (sum) {
				crc8 ^= 0x8C;
			}
			extract >>= 1;
		}
	}
	return crc8;
}

Device::WriteScratchStatus Device::writeScratch(uint8_t data, uint8_t page, const uint8_t* romId) {
	// perform 1-Wire reset
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return WriteScratchStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return WriteScratchStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	// select device via ROM-ID
	selectRom(romId);
	// send 1-Wire command "write scratch pad"
	m_busInterface.sendByte(Device::Command::WriteScratch);
	// If the "page" parameter is set, send page number to device
	if (page != Device::Command::NoPage) {
		m_busInterface.sendByte(page);
	}
	// send data to scratchpad
	m_busInterface.sendByte(data);
	
	// Read and validate data from device
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return WriteScratchStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return WriteScratchStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	selectRom(romId);
	m_busInterface.sendByte(Device::Command::ReadScratch);
	if (page != Device::Command::NoPage) {
		m_busInterface.sendByte(page);
	}
	for (uint8_t byteIndex = 0; byteIndex < 9; ++byteIndex) {
		m_data[byteIndex] = m_busInterface.receiveByte();
	}
	if (calculateCRC8(m_data, 9)) {
		return Device::WriteScratchStatus::CrcFailed;
	}
	if (m_data[0] != data) {
		return Device::WriteScratchStatus::TransmissionFailed;
	}
	return Device::WriteScratchStatus::TransmissionSuccessful;
}

Device::CopyScratchStatus Device::copyScratch(const uint8_t* romId, uint8_t page) {
	// perform 1-Wire reset
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return CopyScratchStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return CopyScratchStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	// select device via ROM-ID
	selectRom(romId);
	// send 1-Wire command "Copy Scratchpad" (0x48)
	m_busInterface.sendByte(Device::Command::CopyScratch);
	// send target page if specified 
	if(page != Device::Command::NoPage){
		m_busInterface.sendByte(page);
	}
	return Device::CopyScratchStatus::CopySuccessful;
}

Device::RecallMemoryStatus Device::recallMemory(const uint8_t* romId, uint8_t page) {
	// perform 1-Wire reset
	switch (m_busInterface.reset()) {
		case Bus::ResetStatus::ResetFailed:
			resetSearch();
			return RecallMemoryStatus::ResetFailed;
			break;
		case Bus::ResetStatus::PullUpFailed:
			resetSearch();
			return RecallMemoryStatus::PullUpFailed;
			break;	
		default:
			break;
	}
	// select device via ROM-ID
	selectRom(romId);
	// start recall
	m_busInterface.sendByte(Device::Command::RecallMemory);
	// send target page if specified
	if(page != Device::Command::NoPage){
		m_busInterface.sendByte(page);
	}
	return RecallMemoryStatus::RecallSuccessfull;
}

} // end namespace OW
