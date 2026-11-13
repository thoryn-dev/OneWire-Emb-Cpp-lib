/** 
* @file main.cpp
* @example ds18b20/main.cpp
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
#include <util/delay.h>
#include "hal/OwHal.h"
#include "OwCore.h"
#include "OwDs18B20.h"
#include "Serial.h"

using namespace OW;

int main(void) {
	char serialBuffer[50];

	Serial Ser1;

    //-------------------------------------------------------------------------
    // This section initializes the entire 1-Wire bus interface once.
    //-------------------------------------------------------------------------

	// 1. Initializes the 1-Wire HAL and passes the pins to the HAL that are to
	// be used.
	//
	// For this example pin 7 of port D is used.
	// DDRD   - Data direction register for port D
	// PIND   - Pin input register (to read) for Port D
	// PORTD  - Pin output register (to write) for Port D
	// PORTD7 - Bit mask for pin 7 on port D 
	Hal myHal(&DDRD, &PIND, &PORTD, PORTD7);

	// 2. Initializes the bus interface and passes the previously created HAL.
	// This gives the bus a concrete connection to the hardware.
	Bus myInterface(myHal);

	// 3. Initializes the generic device which provides standard functions and
	// binds it to the previously created bus.
	Device myDevice(myInterface);

	// 4. Initializes a specific sensor (in this case the DS18B20) that also
	// accesses the bus interface.
	Ds18B20 myDs18B20(myInterface);

    //-------------------------------------------------------------------------
    // This section defines the configuration of the DS18B20 and sends it to
	// the sensor.
    //-------------------------------------------------------------------------

	// Sets the temperature resolution of the DS18B20 to 10 bits.
	myDs18B20.Data.configResolution = 10;

	// The configuration is then sent to the sensor and evaluated to determine
	// if it was successful. The status is analyzed, a text message is
	// generated, and stored in the send buffer for the serial interface.
	switch (myDs18B20.setConfiguration(false)) {
	    case Ds18B20::SetConfigurationStatus::ResetFailed:
	    	snprintf(serialBuffer, sizeof(serialBuffer), "Config: reset failed!\n\n");
	    	break;
	    case Ds18B20::SetConfigurationStatus::PullUpFailed:
	    	snprintf(serialBuffer, sizeof(serialBuffer), "Config: Pullup failed!\n\n");
	    	break;
	    case Ds18B20::SetConfigurationStatus::ConfigurationFailed:
	    	snprintf(serialBuffer, sizeof(serialBuffer), "Config: failed!\n\n");
	    	break;
	    case Ds18B20::SetConfigurationStatus::ConfigurationSuccessful:
	    	snprintf(serialBuffer, sizeof(serialBuffer), "Config: successful!\n\n");
	    	break;
	    default:
	    	break;
	}

	// The data is sent to the serial terminal
	Ser1.serialSend(serialBuffer);

	_delay_ms (1000);
	
	// This is where the cyclical part of the program begins.
	while (1) {
		//-------------------------------------------------------------------------
		// This section causes the DS18B20 to start a temperature conversion.
		//-------------------------------------------------------------------------

		// The temperature conversion command is sent, then the system waits for
		// the conversion to complete and then evaluates the status.
		// A corresponding text message is generated from the status and stored in
		// the serial interface buffer.
		switch(myDs18B20.convertTemperature(true)) {
			case Ds18B20::ConvertTemperatureStatus::ResetFailed:
				snprintf(serialBuffer, sizeof(serialBuffer), "Convert: reset failed!\n");
				Ser1.serialSend(serialBuffer);
				break;
			case Ds18B20::ConvertTemperatureStatus::ConversionStarted:
				snprintf(serialBuffer, sizeof(serialBuffer), "Convert: started!\n");
				Ser1.serialSend(serialBuffer);
				break;
			case Ds18B20::ConvertTemperatureStatus::ConversionCompleted:
				snprintf(serialBuffer, sizeof(serialBuffer), "Convert: finished!\n");
				Ser1.serialSend(serialBuffer);
				break;		
			default:
				break;
		}

	    //-------------------------------------------------------------------------
	    // In this section, the finished temperature is received and displayed on
		// the terminal.
	    //-------------------------------------------------------------------------

		// Here, the data reception process is initiated and the status is
		// evaluated. If an error occurs, a corresponding text message is displayed
		// on the terminal. If the temperature reading is successful, the current
		// value is displayed on the terminal.
		switch (myDs18B20.receiveTemperature()) {
			case Ds18B20::ReceiveTemperatureStatus::CrcFailed:
				snprintf(serialBuffer, sizeof(serialBuffer), "Receive: CRC failed!\n");
				Ser1.serialSend(serialBuffer);
				break;
			case Ds18B20::ReceiveTemperatureStatus::PullUpFailed:
				snprintf(serialBuffer, sizeof(serialBuffer), "Receive: Pullup failed!\n");
				Ser1.serialSend(serialBuffer);
				break;
			case Ds18B20::ReceiveTemperatureStatus::ResetFailed:
				snprintf(serialBuffer, sizeof(serialBuffer), "Receive: Reset failed!\n");
				Ser1.serialSend(serialBuffer);
				break;
			case Ds18B20::ReceiveTemperatureStatus::ReceivedSuccessfully:
				snprintf(serialBuffer, sizeof(serialBuffer), "Receive: successful!\n");
				Ser1.serialSend(serialBuffer);
				snprintf(serialBuffer, sizeof(serialBuffer), "Temperature mdegC: %ld\n", static_cast<signed long>(myDs18B20.Data.valueTemperature));
				Ser1.serialSend(serialBuffer);
				break;				
			default:
				break;
		}
		_delay_ms(1000);
	} // end while
} // end main
