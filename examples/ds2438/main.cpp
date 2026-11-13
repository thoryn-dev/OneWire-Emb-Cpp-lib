/** 
* @example ds2438/main.cpp
* @brief Simple example for DS2438 communication
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include <util/delay.h>

#include "hal/OwHal.h"
#include "OwCore.h"
#include "OwDs18B20.h"
#include "OwDs2438.h"
#include "Serial.h"

using namespace OW;

int main(void) {
	char serialBuffer[50];

	Serial Ser1;

	Hal myHal(&DDRD, &PIND, &PORTD, PORTD7);    // initializing the 1-Wire HAL
	Bus myInterface(myHal);                     // initializing the 1-Wire Bus
	Device myDevice(myInterface);               // initializing the 1-Wire Device
	Ds18B20 myDs18B20(myInterface);             // initializing the DS18B20 sensor

//-----------------------------------------------------------------------------
// Test code: set resolution of temperature sensor DS18B20
//-----------------------------------------------------------------------------
	myDs18B20.Data.configResolution = 10;
	myDs18B20.Data.configTemperatureHigh = 0;
	myDs18B20.Data.configTemperatureLow = 0;

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

	Ser1.serialSend(serialBuffer);

	_delay_ms (1000);
	
	while (1) { // begin while

//-----------------------------------------------------------------------------
// Test code: ROM-ID search algorithm
//-----------------------------------------------------------------------------
	Device::SearchDeviceStatus status;

	do{
		status = myDevice.searchDevice(false);
		if(status == Device::SearchDeviceStatus::DeviceFound){
			uint8_t romId[8];
			myDevice.getRomId(romId);
			snprintf(serialBuffer, sizeof(serialBuffer), "Rom ID: %X, %X, %X, %X, %X, %X, %X, %X\n", romId[0],
																									 romId[1],
																									 romId[2],
																									 romId[3],
																									 romId[4],
																									 romId[5],
																									 romId[6],
																									 romId[7]);
		Ser1.serialSend(serialBuffer);
		}
	}while(status == Device::SearchDeviceStatus::DeviceFound);

	switch (status){
	    case Device::SearchDeviceStatus::LastDeviceFound:
	    	snprintf(serialBuffer, sizeof(serialBuffer), "Search: last device found!\n\n");
	    	break;
	    case Device::SearchDeviceStatus::ResetFailed:
	    	snprintf(serialBuffer, sizeof(serialBuffer), "Search: Reset failed!\n\n");
	    	break;
	    case Device::SearchDeviceStatus::CRCFailed:
	    	snprintf(serialBuffer, sizeof(serialBuffer), "Search: CRC failed!\n\n");
	    	break;
	    case Device::SearchDeviceStatus::SearchFailed:
	    	snprintf(serialBuffer, sizeof(serialBuffer), "Search: no device respond!\n\n");
	    	break;
	    default:
	    	break;
	}

	Ser1.serialSend(serialBuffer);
			
//-----------------------------------------------------------------------------
// Test code: temperature conversation on DS18B20
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
// Test code: receive temperature from DS18B20
//-----------------------------------------------------------------------------
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
			snprintf(serialBuffer, sizeof(serialBuffer), "Temperature m°C: %ld\n", static_cast<signed long>(myDs18B20.Data.valueTemperature));
			Ser1.serialSend(serialBuffer);
			break;				
		default:
			break;
	}

	} // end while
} // end main
