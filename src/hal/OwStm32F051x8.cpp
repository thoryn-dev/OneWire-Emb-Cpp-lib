// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

/**
 * @file OwStm32F051x8.cpp
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
