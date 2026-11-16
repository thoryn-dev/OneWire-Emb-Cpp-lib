// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 M. Fiedler (thoryn-dev)

/** 
* @example ds18b20_stm32f0_disco/main.cpp
* @brief Simple example for DS18B20 communication
*
* Here is a simple example demonstrating how to use the 1-Wire library with a
* single DS18B20 temperature sensor.
*
* The example sets up and configures a new 1-Wire interface. It then reads data
* from a single DS18B20 and outputs the measured value to the serial terminal.
*
* @note This example is only for Arduino Nano (Atmega 328) and only a single
* DS18B20 must be connected to the bus!
*
* Here is a breakdown of what this example does:
*
* - It initializes a bus interface
* - It configures the resolution of the DS18B20
* - It initiates the temperature conversion on the DS18B20
* - It reads the temperature and outputs it to the serial terminal
* - At the end the code waits for 1 second and started the next temperatur
*   meassurment
* - Additionally, status messages and any errors that may occur are output to
*   the serial terminal
*/
#include <stdio.h>
// Platform specific librarys
#include <stm32f0xx.h>
// 1-Wire specific librarys
#include "hal/OwHal.h"
#include "OwCore.h"
#include "OwDs18B20.h"

using namespace OW;

int main(void) {

	//-------------------------------------------------------------------------
    // This section initializes the STM32F0 and is not a specific part
	// of the 1-Wire Library but needed to run the microcontroller
    //-------------------------------------------------------------------------
	
	// Configure the STM32F0 to a system clock of 48 MHz
	FLASH->ACR |= FLASH_ACR_LATENCY;
    RCC->CFGR |= RCC_CFGR_PLLMUL12;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

	// Activate the clock for GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    
    // This is where the cyclical part of the program begins.
    while (1)
    {
        /* code */
    } // end while
    
} // end main
