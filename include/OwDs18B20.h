// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2026 M. Fiedler (thoryn-dev)

/**
 * @file OwDs18B20.h
 * @brief Declarations and interface for the DS18B20 device
 */
#pragma once
#include <stdint.h>
#include "OwCore.h"

namespace OW{

/**
 * @class Ds18B20
 * @brief Class for DS18B20 devices
 *
 * Provides functions for evaluating and configuring DS18B20 devices.
 */
class Ds18B20 : public Device {
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
        * @enum GetConfigurationStatus
        * @brief getConfiguration function status codes
        */
        enum class GetConfigurationStatus : uint8_t {
            ReceivedSuccessfully    = 0,    /**< Configuration was received correctly */
	        ResetFailed			    = 1,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed            = 2,    /**< Pull up failed or bus data line shorted to GND */
            CrcFailed               = 3     /**< CRC check of data failed */
        };
        /**
        * @enum ConvertTemperatureStatus
        * @brief convertTemperature function status codes
        */
        enum class ConvertTemperatureStatus : uint8_t {
            ConversionCompleted     = 0,    /**< Conversion completed */
	        ResetFailed             = 1,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed            = 2,    /**< Pull up failed or bus data line shorted to GND */
	        ConversionStarted		= 3     /**< Conversion has started */            
        };
        /**
        * @enum ReceiveTemperatureStatus
        * @brief receiveTemperature function status codes
        */
        enum class ReceiveTemperatureStatus : uint8_t {
            ReceivedSuccessfully    = 0,    /**< Temperature was received correctly  */
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
            int16_t valueTemperatureRaw = 0;    /**< actual temperaturen in digits */
            uint8_t configResolution = 12;      /**< resolution temperature measurement */
            uint8_t configTemperatureHigh = 0;  /**< temperature upper alarm threshold */
            uint8_t configTemperatureLow = 0;   /**< temperature lower alarm threshold */
        } Data; ///< Stores the configuration and measurement data of the Sensor 
        /**
        * @brief Constructor for Device DS18B20
        * 
        * @param[in] interface interface from Bus
        */
        explicit Ds18B20(Bus&);
        /**
        * @brief Configures the sensor
        * 
        * This function is used to configure the sensor. The resolution, the
        * upper alarm threshold, and the lower alarm threshold can be set.
        * 
        * To perform the configuration, the configuration values ​​must be
        * written into the Data (SensorData) struct. The following resolutions
        * are possible:
        * 
        * - 12-bit – resolution 0.0625°C
        * - 11-bit – resolution 0.125°C
        * - 10-bit – resolution 0.25°C
        * - 9-bit – resolution 0.5°C
        * 
        * The function verifies the correct transfer to the scratchpad memory
        * by reading the data back using the Ds18B20::getConfiguration()
        * function, which performs a CRC check. If the function does not report
        * an error status, the data is correct.
        * 
        * You can choose whether the configuration is permanently stored in the
        * EEPROM. If permanent storage is disabled, the data last written to
        * the EEPROM is used for configuration following a power-on reset.
        * 
        * @param[in] saveEeprom     true = enables permanent storage in the EEPROM
        * @param[in] romId          pointer to the target device's ROM ID
        * 
        * @return setConfiguration function status codes
        * @retval Ds18B20::SetConfigurationStatus::ConfigurationSuccessful
        * @retval Ds18B20::SetConfigurationStatus::ResetFailed
        * @retval Ds18B20::SetConfigurationStatus::PullUpFailed
        * @retval Ds18B20::SetConfigurationStatus::ConfigurationFailed
        */
        SetConfigurationStatus setConfiguration(bool, const uint8_t* = nullptr);
        /**
        * @brief Read sensor configuration
        * 
        * This function reads the current configuration from the scratchpad and
        * stores it in the SensorData structure. The received data undergoes a
        * CRC check, and an error code is output if reception fails.
        * 
        * @note If the configuration is to be read back from the EEPROM, the
        * Device::Command::RecallMemory (0xB8) 1-Wire command must be sent
        * first. However, this also overwrites the current configuration in the
        * scratchpad.
        * 
        * @param[in] romId  pointer to the target device's ROM ID
        * 
        * @return getConfiguration function status codes
        * @retval Ds18B20::GetConfigurationStatus::ReceivedSuccessfully
        * @retval Ds18B20::GetConfigurationStatus::ResetFailed
        * @retval Ds18B20::GetConfigurationStatus::PullUpFailed
        * @retval Ds18B20::GetConfigurationStatus::CrcFailed
        */
        GetConfigurationStatus getConfiguration(const uint8_t* = nullptr);
        /**
        * @brief Starts temperature conversion
        * 
        * This function initiates the temperature conversion. You can choose
        * whether to wait for the conversion to complete or to manage the
        * correct conversion time yourself.
        * 
        * Once the conversion is complete, the temperature can be read from the
        * device using the Bs18B20::receiveTemperature() function.
        * 
        * @bug If waiting for the complete conversion is enabled and the
        * DS18B20 does not provide a completion signal, the program will halt
        * at this point. The function is slated for revision and should be used
        * with caution until then.
        * 
        * @param[in] waitConversation   true = wait for the completion signal from the DS18B20
        * @param[in] romId              pointer to the target device's ROM ID
        * 
        * @return convertTemperature function status code
        * @retval Ds18B20::ConvertTemperatureStatus::ConversionCompleted
        * @retval Ds18B20::ConvertTemperatureStatus::ResetFailed
        * @retval Ds18B20::ConvertTemperatureStatus::PullUpFailed
        * @retval Ds18B20::ConvertTemperatureStatus::ConversionStarted
        */
        ConvertTemperatureStatus convertTemperature(bool, const uint8_t* = nullptr);
        /**
        * @brief Receive the temperature
        * 
        * This function receives the digital temperature value from the sensor.
        * 
        * Upon receipt, the data undergoes a CRC check. If the data is valid,
        * the temperature is stored in the @c valueTemperatureRaw field of the
        * SensorData struct. The value range extends from -880 to +2000 digits.
        * To obtain the physical temperature in different formats,
        * use Ds18B20::getTemperature().
        * 
        * @note Before the current temperature can be read, a conversion must
        * be initiated using function Ds18B20::convertTemperature()!
        * 
        * @param[in] romId  pointer to the target device's ROM ID
        * 
        * @return receiveTemperature function status code
        * @retval Ds18B20::ReceiveTemperatureStatus::ReceivedSuccessfully
        * @retval Ds18B20::ReceiveTemperatureStatus::ResetFailed
        * @retval Ds18B20::ReceiveTemperatureStatus::PullUpFailed
        * @retval Ds18B20::ReceiveTemperatureStatus::CrcFailed
        */
        ReceiveTemperatureStatus receiveTemperature(const uint8_t* = nullptr);
        /**
        * @brief Gives back the temperature value last received
        *
        * This general template function returns either the raw value or the
        * physical temperature of the internally stored temperature value
        * previously read from the DS18B20 device. There are three possible
        * return values, which differ in data type, scaling, and precision.
        * 
        * @note The maximum resolution also depends on the sensor
        * configuration. For maximum resolution, the sensor must be configured
        * to 12 bits. @see Ds18B20::setConfiguration()
        * 
        * Possible data types
        * - `<int16_t>` - raw value direct from the sensor register -880...+2000
        * - `<int32_t>` - calculated temperature value in -55000...+125000 mdegC
        * - `<float>`   - calculated temperature value in -55.0...+125.0 degC
        *
        * @tparam T Desired return datatype
        * @return T Temperature value
        */
        template <typename T>
        T getTemperature() const = delete;
}; // end class Ds18B20

/**
 * @brief Gives back the temperature value last received (int16_t)
 *
 * Explicit template specialization for @c int16_t. It directly outputs the raw
 * sensor value read from the DS18B20 register. The value has a range of
 * -880...+2000 which corresponds to a temperature range of -55...+125 degC.
 * It has a maximum resolution of 0.0625°C.
 * 
 * @see Ds18B20::getTemperature()
 *
 * @return Temperature value (int16_t)
 */
template <>
inline int16_t Ds18B20::getTemperature<int16_t>() const {
    return Data.valueTemperatureRaw;
}
/**
 * @brief Gives back the temperature value last received (int32_t)
 *
 * Explicit template specialization for @c int32_t. Returns the converted
 * temperature value of the sensor, with a range of -55,000...+125,000 mdegC.
 * 
 * @see Ds18B20::getTemperature()
 *
 * @return Temperature value (int32_t)
 */
template <>
inline int32_t Ds18B20::getTemperature<int32_t>() const {
    // turn raw temperature in Millidegrees Celsius
    // corresponds -55,000 to +125,000 mdegC
    return ((static_cast<int32_t>(Data.valueTemperatureRaw) * 1000) / 16);
}
/**
 * @brief Gives back the temperature value last received (float)
 *
 * Explicit template specialization for @c float. Returns the converted
 * temperature value of the sensor, with a range of -55.0...+125.0 degC.
 * 
 * @warning Calculating floating point data types can lead to performance
 * issues and high memory consumption on low-performance and 8-bit
 * microcontrollers.
 * 
 * @see Ds18B20::getTemperature()
 *
 * @return Temperature value (float)
 */
template <>
inline float Ds18B20::getTemperature<float>() const {
    return static_cast<float>(Data.valueTemperatureRaw)/16.0f;
}

} // end namespace OW