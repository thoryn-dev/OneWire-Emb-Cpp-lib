// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

/**
 * @file OwHal.cpp
 */

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include "hal/OwAtmega328.h"

namespace OW{

Hal::Hal(volatile uint8_t* directionRegister, volatile uint8_t* portInRegister, volatile uint8_t* portOutRegister, uint8_t pinNumber)
            : m_directionRegister(directionRegister), m_portInRegister(portInRegister), m_portOutRegister(portOutRegister), m_pinNumber(1 << pinNumber){

    // initialize pins for use with Bus
    *m_directionRegister &= (~m_pinNumber); // config pin as input
    *m_portOutRegister &= (~m_pinNumber);   // set output register drive pin low

    TCCR1A = 0;             // set TIMER 1 to normal mode (clear TCCR1A and TCCR1B completely)
    TCCR1B = 0;
    TCCR1B |= (1 << CS10);  // set prescaler to 1 (CS10 = 1)
}

Hal::~Hal(){

}

void Hal::setPinLow(){
    *m_directionRegister |= m_pinNumber;    // set pin LOW by setting direction register to output
}

void Hal::releasePin(){
    *m_directionRegister &= (~m_pinNumber); // release pin by setting direction register to input
}

bool Hal::readPin(){
    return (*m_portInRegister & m_pinNumber);   // read pin value from input register
}

void Hal::lockInterrupt(){
    // backup SREG register and disable Interrupts
    m_sregBackup = SREG;
    cli();
}

void Hal::unlockInterrupt(){
    // restore SREG register and enable Interrupts
    SREG = m_sregBackup;
    sei();
}

void Hal::waitUs(uint16_t delay) {
    uint16_t tick = delay * 16 - 15;    // at prescaler of 1 one tick correspond to 62,5 ns (2 Ticks) and correction factor
    TCNT1 = 0;                          // reset TIMER1 count register
    while(TCNT1 < tick);                // wait until the target time is reached
}

} //end namespace OW
