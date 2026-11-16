// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 M. Fiedler (thoryn-dev)

/**
* @file OwStm32F051x8.cpp
* @brief Implementation for the HAL on platform STM32F051x8
*/
#if defined(STM32F051x8)
#include "hal/OwStm32F051x8.h"

namespace OW{

//-----------------------------------------------------------------------------
// Class Hal
//-----------------------------------------------------------------------------

Hal::Hal(volatile uint8_t* directionRegister, volatile uint8_t* portInRegister, volatile uint8_t* portOutRegister, uint8_t pinNumber)
            : m_directionRegister(directionRegister), m_portInRegister(portInRegister), m_portOutRegister(portOutRegister), m_pinNumber(1 << pinNumber){

}

Hal::~Hal(){

}

void Hal::setPinLow(){

}

void Hal::releasePin(){

}

bool Hal::readPin(){
    return 0;
}

void Hal::lockInterrupt(){
    // backup SREG register and disable Interrupts

}

void Hal::unlockInterrupt(){
    // restore SREG register and enable Interrupts

}

void Hal::waitUs(uint16_t delay) {

}

//-----------------------------------------------------------------------------
// Class SerialConsole
//-----------------------------------------------------------------------------

SerialConsole::SerialConsole() {

}

SerialConsole::~SerialConsole() {
	
}

void SerialConsole::serialSend(const char* string) {

}

void SerialConsole::serialSend(uint8_t number) {

}

} //end namespace OW
#endif
