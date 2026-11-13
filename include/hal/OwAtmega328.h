// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

/**
 * @file OwAtmega328.h
 * @brief Implementation of the ATmega328P hardware abstraction layer
 *
 * The library provides an API for the 1-Wire core and abstracts the hardware
 * layer. For this task it provides basic I/O functions and a blocking delay
 * function (OW::Hal::waitUs) thats used for 1-Wire timing.
 * 
 * @todo
 * - use constexpr system clock speed to calculate correct TIMER1 configuration
 */

#pragma once
#include <stdint.h>

namespace OW{

/**
 * @class Hal
 * @brief Hardware abstraction class
 *
 * This class implements a hardware abstraction layer to keep the 1-Wire core
 * library portable. Registers for timers, inputs, outputs, etc., are
 * encapsulated in standard functions and passed to the Bus class. The Bus
 * class can then control the hardware without needing knowledge of the
 * specific hardware registers.
 */
class Hal {
    public:
        /**
        * @brief Constructor for Hal
        * 
        * This function is the constructor of the Hal class.
        * 
        * This function initializes the input and output pins for the 1-Wire
        * bus. The port registers and pin number can be passed as arguments,
        * allowing you to select which pin is used for the 1-Wire bus.
        * 
        * The HAL must be initialized in the main program, for example:
        * 
        * \code{.cpp} Hal Hal1(&DDRD, &PORTD, &PIND, PORTD7); \endcode
        * 
        * @param[in] directionRegister Data direction register
        * @param[in] portInRegister Port output register
        * @param[in] portOutRegister Port input register
        * @param[in] pinNumber PIN number to use vor Bus
        */
        Hal(volatile uint8_t*, volatile uint8_t*, volatile uint8_t*, uint8_t);
        ~Hal();
        /**
        * @brief Set the MCU pin low
        * 
        * This Function set the MCU pin to low via the hardware specific
        * registers.
        * 
        * @note Setting the pin to low is not accomplished via the output
        * register as usual; instead, the output register is permanently low,
        * and only the data direction register is switched between input and
        * output. This allows the pin to switch either to low or into high
        * impedance mode.
        */
        void setPinLow();
        /**
        * @brief Release the MCU pin
        * 
        * This Function releases the MCU pin to high impedance mode via the
        * hardware specific registers.
        * 
        * @note Releasing the pin is not accomplished via the output
        * register as usual; instead, the output register is permanently low,
        * and only the data direction register is switched between input and
        * output. This allows the pin to switch either to low or into high
        * impedance mode.
        */
        void releasePin();
        /**
        * @brief Read MCU pin
        * 
        * This Function reads the current Value from the MCU pin via the
        * hardware input register.
        */
        bool readPin();
        /**
        * @brief Lock MCU interrupts
        * 
        * This Function backup the SREG register and disable the interrupts.
        * 
        * @warning After the task is done make sure to unlock the interrupts by calling Hal::unlockInterrupt 
        */
        void lockInterrupt();
        /**
        * @brief Unlock MCU interrupts
        * 
        * Restore the SREG register and enable the interrupts.
        */
        void unlockInterrupt();
        /**
        * @brief Blocking wait funktion
        * 
        * This function is used to wait a specific ammount of time. It has a
        * resolution of 1 µs and a maximum range of 4 ms.
        * 
        * ATmega328P @ 16 MHz
        * 
        * @note This is a blocking function; this means that no further command
        * is executed while waiting.
        * 
        * @warning Particularly for time-critical applications, such as 1-Wire
        * timing, it must be ensured that MCU interrupts are disabled. The HAL
        * provides two special functions (Hal::lockInterrupt /
        * Hal::unlockInterrupt) for this purpose.
        * 
        * @bug This timer isn't save in any way. It has no protection against
        * overflow of Parameter "delay" in the process and no protection
        * against underflow because of the correction factor!
        * 
        * @todo improve the quality of the wait function and its functionality 
        * 
        * @param[in] delay The time interval to wait for [µs]
        */
        void waitUs(uint16_t);
    private:      
        volatile uint8_t* m_directionRegister;  ///< Stores the pointer to the Data Direction Register that was passed to the constructor
        volatile uint8_t* m_portInRegister;     ///< Stores the pointer to the Port Input Register that was passed to the constructor
        volatile uint8_t* m_portOutRegister;    ///< Stores the pointer to the Port Output Register that was passed to the constructor
        uint8_t m_pinNumber;                    ///< Stores the pin bit mask that was passed to the constructor
        uint8_t m_sregBackup;                   ///< Stores the value of the SREG register when interrupt disabling is toggled.
};

} // end namespace OW
