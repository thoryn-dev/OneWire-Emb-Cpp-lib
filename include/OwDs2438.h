// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

#pragma once
#include <stdint.h>
#include "OwCore.h"

namespace OW{

/**
 * @class Ds2438
 * @brief Class for DS2438 devices
 *
 * Provides functions for evaluating DS2438 devices.
 */
class Ds2438 : public Device {
    public:
        /**
        * @enum SetConfigurationStatus
        * @brief setConfiguration function status codes
        */
        enum class SetConfigurationStatus : uint8_t {
	        ConfigurationSuccessful = 0,    /**< Configuration was set correctly */
	        ResetFailed			    = 1,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed            = 2,    /**< Pull up failed or bus data line shorted to GND */
            ConfigurationFailed     = 3     /**< Configuration verification failed */
        };
        /**
        * @enum ConvertVoltageStatus
        * @brief convertVoltage function status codes
        */
        enum class ConvertVoltageStatus : uint8_t {
            StartedConversion		= 0,    /**< Conversion has started */
	        ResetFailed             = 1,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed            = 2     /**< Pull up failed or bus data line shorted to GND */	        
        };
        /**
        * @enum ConvertTemperatureStatus
        * @brief convertTemperature function status codes
        */
        enum class ConvertTemperatureStatus : uint8_t {
            ConversionCompleted		= 0,    /**< Conversion completed */
	        ResetFailed             = 1,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed            = 2,    /**< Pull up failed or bus data line shorted to GND */
            ConversionStarted       = 3     /**< Conversion has started */    
        };
        /**
        * @enum ReceiveVoltageStatus
        * @brief receiveVoltage function status codes
        */
        enum class ReceiveVoltageStatus : uint8_t {
            successfullyReceived    = 0,    /**< Conversion completed */
	        ResetFailed             = 1,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed            = 2,    /**< Pull up failed or bus data line shorted to GND */
	        CrcFailed               = 3     /**< CRC check of data failed */
        };
        /**
        * @struct SensorData
        * @brief sensor data and configuration
        * 
        * This struct contains the configuration and sensor data for the DS2438
        * sensor. The struct comprises several sections:
        * 
        * - Values ​​prefixed with "value" contain data read from the sensor.
        * - Values ​​prefixed with "config" contain configuration settings that
        * can be read from or sent to the sensor.
        * - Values ​​prefixed with "flag" contain status bits from the sensor.
        */
        struct SensorData
        {
            int32_t     valueTemperature        = 0;        /**< actual temperaturen in m°C */
            int16_t     valueTemperatureRaw     = 0;        /**< actual temperaturen in digits */
            uint16_t    valueVoltage            = 0;        /**< actual voltage on pin V<sub>DD</sub> */
            int16_t     valueCurrentRaw         = 0;        /**< actual current on pin V<sub>SENS</sub> */
            uint8_t     configThreshold         = 0;        /**< threshold value for charging current accumulator */
            bool        configIad               = false;    /**< Current A/D Control Bit - 1 = current A/D and the ICA are enabled */
            bool        configCa                = false;    /**< Current Accumulator Configuration - 1 = CCA/DCA is enabled */
            bool        configEe                = false;    /**< Current Accumulator Shadow Selector - 1 = CCA/DCA counter data will be shadowed to EEPROM */
            bool        configAd                = false;    /**< Voltage A/D Input Select Bit - 1 = the battery input (VDD) is selected as the input */
            bool        flagTb                  = false;    /**< Temperature Busy Flag. 1 = temperature conversion in progress; 0 = temperature conversion complete. */
            bool        flagNvb                 = false;    /**< Nonvolatile Memory Busy Flag. 1 = Copy from Scratchpad to EEPROM in progress; 0 = Nonvolatile memory not busy */
            bool        flagAdb                 = false;    /**< A/D Converter Busy Flag. 1 = A/D conversion in progress on battery voltage; 0 = conversion complete, or no measurement being made */
        } Data; ///< Stores the configuration and measurement data of the Sensor 
        /**
        * @brief Constructor for Device DS2438
        * 
        * @param[in] interface interface from Bus
        */
        explicit Ds2438(Bus&);
        /**
        * @brief Configures DS2438
        * 
        * This function is used to set the configuration bits in the sensor's
        * configuration register.
        * 
        * To perform the configuration, the configuration bits ​​must be
        * written into the Data (SensorData) struct. For a detailed description
        * of the configuration bit, see Struct SensorData.
        * 
        * The function verifies the correct transfer to the scratchpad memory
        * by reading the data back using the Ds2438::getConfiguration()
        * function, which performs a CRC check. If the function does not report
        * an error status, the data is correct.
        * 
        * You can choose whether the configuration is permanently stored in the
        * EEPROM. If permanent storage is disabled, the data last written to
        * the EEPROM is used for configuration following a power-on reset or
        * by performing a RecallMemory on page 0.
        * 
        * @todo function save to EEPROM and CRC check is not implemented yet
        * 
        * @param[in] saveEeprom true = enables permanent storage in the EEPROM
        * @param[in] romId      Pointer to the target device's ROM ID
        * 
        * @return setConfiguration function status code
        * @retval Ds2438::SetConfigurationStatus::ConfigurationSuccessful
        * @retval Ds2438::SetConfigurationStatus::ResetFailed
        * @retval Ds2438::SetConfigurationStatus::PullUpFailed
        * @retval Ds2438::SetConfigurationStatus::ConfigurationFailed
        */
        SetConfigurationStatus setConfiguration(bool, const uint8_t*);
        /**
        * @brief Starts voltage conversion
        * 
        * The function sends the command to the bus that initiates the voltage
        * conversion. If no address is specified, all devices are addressed via
        * Skip ROM and start the conversion simultaneously.
        * 
        * @param[in] waitConversation   true = wait for the voltage completion 
        * @param[in] romId              Pointer to the target device's ROM ID
        * 
        * @warning Waiting for the conversion to be completed is not yet
        * implemented, so the parameter "waitConversation" has no effect.
        * 
        * @return
        * @retval 1 - successful
        * @retval 0 - reset failed
        */
        ConvertVoltageStatus convertVoltage(bool, const uint8_t* = nullptr);
        /**
        * @brief Starts temperature conversion
        * 
        * This function initiates the temperature conversion. You can choose
        * whether to wait for the conversion to complete or to manage the
        * correct conversion time yourself.
        * 
        * Once the conversion is complete, the temperature can be read from the
        * device using the Ds2438::receiveTemperature() function.
        * 
        * @bug If waiting for the complete conversion is enabled and the
        * DS2438 does not provide a completion signal, the program will halt
        * at this point. The function is slated for revision and should be used
        * with caution until then.
        * 
        * @param[in] waitConversation   true = wait for the temperature completion 
        * @param[in] romId              pointer to the target device's ROM ID
        * 
        * @warning Waiting for the conversion to be completed is not yet
        * implemented, so the parameter "waitConversation" has no effect.
        * 
        * @return convertTemperature function status code
        * @retval Ds18B20::ConvertTemperatureStatus::ConversionCompleted
        * @retval Ds18B20::ConvertTemperatureStatus::ResetFailed
        * @retval Ds18B20::ConvertTemperatureStatus::PullUpFailed
        * @retval Ds18B20::ConvertTemperatureStatus::ConversionStarted
        */
        ConvertTemperatureStatus convertTemperature(bool, const uint8_t* = nullptr);
        /**
        * @brief Reads voltage from DS2438
        * 
        * This function receives the measured voltage from the DS2438.
        * 
        * @note Before the current voltage can be read, a conversion must be
        * initiated using function Ds2438::convertVoltage()!
        * 
        * Upon receipt, the data undergoes a CRC check. If the data is valid,
        * the voltage is converted to mV and stored in the "voltage" field of
        * the SensorData struct. The voltage range is 0 V to 10 V.
        * 
        * @param[in] romId  Pointer to the target device's ROM ID
        * 
        * @return true - successful
        * @return false - CRC failed
        */
        ReceiveVoltageStatus receiveVoltage(const uint8_t* = nullptr);
}; // end class Ds2438

} // end namespace OW