// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

#pragma once
#include <avr/io.h>
#include <stdlib.h>

 /**
 * @file Serial.h
 * @brief Implementation of a simple Serial Interface
 */

/**
 * @class Serial
 * @brief Class for basic serial communication
 *
 * This class provides a simple serial interface. The code is currently
 * hardcoded to the Atmega 328. See the following Todo section for more
 * information.
 * 
 * @todo Perhaps move this class to the HAL if it will later be fully
 * integrated into the project. A simple serial monitor, a debug interface for
 * the library, or a serial console for control and configuration would be
 * conceivable. The class should also become platform-independent, but remain
 * optional in principle.
 */
class Serial {
	public:
		Serial();	// Constructor
		~Serial();	// Destructor
		/**
		* @brief Send string to the serial interface
		* 
		* This function is used to send individual ASCII characters to the serial
		* interface. This allows strings to be transferred to a PC.
		* 
		* @param[in] string	The ASCII character to be sent to the interface.
		*/
		void serialSend(const char*);
		/**
		* @brief Send string to the serial interface
		* 
		* The function is used to send an eight-bit value to the serial interface.
		* 
		* @param[in] number	The value to be sent to the serial interface
		*/
		void serialSend(uint8_t);
};