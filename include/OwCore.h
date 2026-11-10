// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

 /**
 * @file OwCore.h
 * @brief Implementation for 1-Wire communication
 *
 * This library provides an API for communicating with 1-Wire devices. To this
 * end, it offers an interface (OW::Bus) that handles basic bus communication and
 * functions, a device class (OW::Device) that manages device-agnostic functions,
 * and specific device classes (e.g. OW::Ds18B20) for the actual device.
 * 
 * @note The library requires a HAL to access the specific hardware of the
 * platform being used.
 * 
 * @todo
 * - constexpr Set timing values ​​according to Maxim specifications, even if
 * the values ​​need to be calculated.
 * - potentially use std::array
 * - use reference instead of pointer for "romId"
 * - implement family code selection and/or skipROM functionality by hand over an empty ROM-ID Array as enum keyword
 */

#pragma once
#include "hal/OwHal.h"

namespace OW{

/**
 * @class Bus
 * @brief Class for basic 1-Wire communication
 *
 * This class provides an interface that handles bit and byte communication on
 * the 1-Wire bus via the HAL class. Additionally, functions for basic 1-Wire
 * bus commands are provided—for example, Search ROM.
 */
class Bus {
    public:
        /*!
        * @enum ResetStatus
        * @brief reset function status codes
        */
        enum class ResetStatus : uint8_t {
	        ResetSuccessful		= 0,    /**< Reset successful and at least one device present */
	        ResetFailed			= 1,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed        = 2     /**< Pull up failed or bus data line shorted to GND */
        };
        /**
        * @brief Constructor for Bus interface
        * 
        * This function is the constructor of the Bus class and accepts the
        * hardware interface from Hal.
        * 
        * @param[in] interface interface from hardware abstraction layer
        * 
        * @todo
        * - Migration of the OW timing to a hardware timer implemented as a
        *   state machine.
        */
        explicit Bus(Hal&);  // Constructor
        ~Bus();     // Destructor
        /**
         * @brief Performs an 1-Wire reset.
         * 
         * The function sends a reset pulse to the 1-Wire bus and evaluates the
         * presence pulse of any device that may be present.
         * 
         * @return Reset function status code
         * @retval Bus::ResetStatus::ResetSuccessful
         * @retval Bus::ResetStatus::ResetFailed
         * @retval Bus::ResetStatus::PullUpFailed
        */
        ResetStatus reset();
        /**
         * @brief Reads a bit from the bus
         * 
         * The function reads a bit from the 1-Wire bus and returns its value.
         * 
         * @return Value of the read bit
        */
        bool readBit();
        /**
         * @brief Writes a bit to the bus
         * 
         * The function writes a bit to the 1-Wire bus.
         * 
         * @param[in] dataBit Bit to be sent to the bus
        */
        void writeBit(bool);
        /**
         * @brief Reads a byte from the bus
         * 
         * The function reads a byte from the 1-Wire bus and returns its value.
         * To do this, the byte is read bit by bit using the Bus::readBit()
         * function. Reading the byte begins with the MSB.
         * 
         * @todo
         * - Potentially optimize the algorithm
         * 
         * @return Value of the read byte
        */
        uint8_t receiveByte();
        /**
         * @brief Writes a byte to the bus
         * 
         * The function writes a byte to the 1-Wire bus. To do this, the byte
         * is written bit by bit using the Bus::writeBit() function. Writing
         * begins with the LSB.
         *
         * @todo
         * - Potentially optimize the algorithm
         * 
         * @param[in] dataByte byte to be sent to the bus
        */
        void sendByte(uint8_t);
    private:
        Hal& m_interfaceHal;
        /** 
        * @name 1-Wire Timing constants
        * @anchor owtimingConstants
        * Timing constants for bit-banging
        */
        ///@{
        static constexpr uint16_t TIME_SLOT          = 100;                                             ///< Duration of a complete time slot
        static constexpr uint16_t TIME_RECOVERY      =   5;                                             ///< Bus recovery time after a read/write operation
        static constexpr uint16_t TIME_WRITE_1       =   5;                                             ///< Time to write a "1" bit
        static constexpr uint16_t TIME_WRITE_0       =  80;                                             ///< Time to write a "0" bit
        static constexpr uint16_t TIME_READ          =  10 - TIME_WRITE_1;                              ///< Time until the sampling for reading begins
        static constexpr uint16_t TIME_PRESENCE_LOW  =  70;                                             ///< Reset: time until sample input state
        static constexpr uint16_t TIME_PRESENCE_HIGH =  20;                                             ///< Reset: Time during which the bus must be high after a reset.
        static constexpr uint16_t TIME_RESET_LOW     = 500;                                             ///< Reset: time for drive bus low (min. 480us)
        static constexpr uint16_t TIME_RESET_HIGH    = 500 - TIME_PRESENCE_HIGH - TIME_PRESENCE_LOW;    ///< Reset: time for become bus high after present pulse
        static constexpr uint16_t TIME_SLOT_1        = TIME_SLOT - TIME_WRITE_1;
        static constexpr uint16_t TIME_SLOT_0        = TIME_SLOT - TIME_WRITE_0;
        static constexpr uint16_t TIME_SLOT_READ     = TIME_SLOT - TIME_WRITE_1 - TIME_READ;            ///< Read:  time for recovery after reading
        ///@}

        static constexpr uint8_t MSB = 0b10000000;	// Most significant bit
        static constexpr uint8_t LSB = 0b00000001;	// Least significant bit
}; // end class Bus

/**
 * @class Device
 * @brief General class for 1-Wire devices
 *
 * This class represents a generic 1-Wire device. It serves as the parent class
 * for specific device classes. It contains functions that are not limited to
 * any specific 1-Wire device. The child classes provide additional functions
 * that can only be executed on that specific device.
 */
class Device {
    public:
        /**
        * @enum Command
        * @brief Standard 1-Wire commands
        * 
        * 1-Wire commands in hex according to the Maxim (Dallas) datasheet
        */
        enum Command : uint8_t {
            AlarmSearchRom          = 0xEC,     /**< Select all devices with an active alarm */
            ConvertTemperature      = 0x44,     /**< Triggers devices to start a temperature conversion */
            ConvertVoltage        	= 0xB4,     /**< Triggers devices to start a voltage conversion */
            CopyScratch     	    = 0x48,     /**< causes the device to copy the data from the scratchpad to the EEPROM */
            MatchRom        	    = 0x55,     /**< Selects a single device in conjunction with a ROM ID */
            NormalSearchRom       	= 0xF0,     /**< Starts a ROM ID search for all devices */
            OverdriveMatchRom	    = 0x69,
            OverdriveSkipRom	    = 0x3C,
            ReadRom				    = 0x33,     /**< Reads the ROM ID directly (only one device may be on the bus for this) */
            ReadScratch     	    = 0xBE,     /**< Initiates the reading of the scratchpad by the master */
            RecallMemory       	    = 0xB8,     /**< causes the device to copy the data from the EEPROM to the scratchpad */
            SkipRom         	    = 0xCC,     /**< Selects all devices on the bus, regardless of the ROM ID */
            WriteScratch		    = 0x4E,     /**< Starts writing to the scratchpad, followed by the data to be written */
            NoPage                  = 0xFF,     /**< Selector to use when the sensor supports only one page and no page specification is to be sent. */
            Page0                   = 0x00,     /**< Selector for page 0 in the scratchpad / EEPROM. */
            Page1                   = 0x01,     /**< Selector for page 1 in the scratchpad / EEPROM. */
            Page2                   = 0x02,     /**< Selector for page 2 in the scratchpad / EEPROM. */ 
            Page3                   = 0x03,     /**< Selector for page 3 in the scratchpad / EEPROM. */
            Page4                   = 0x04,     /**< Selector for page 4 in the scratchpad / EEPROM. */
            Page5                   = 0x05,     /**< Selector for page 5 in the scratchpad / EEPROM. */
            Page6                   = 0x06,     /**< Selector for page 6 in the scratchpad / EEPROM. */
            Page7                   = 0x07      /**< Selector for page 7 in the scratchpad / EEPROM. */
        };
        /**
        * @enum FamilyCode
        * @brief 1-Wire device family codes
        * 
        * The enum contains the family codes of the various 1-Wire devices
        * in hex.
        */
        enum FamilyCode : uint8_t {
            Ds18B20     = 0x28,     /**< Programmable Resolution 1-Wire Digital Thermometer */
            Ds2438      = 0x26      /**< Smart Battery Monitor */
        };
        /**
        * @enum SearchDeviceStatus
        * @brief searchDevice function status codes
        */
        enum class SearchDeviceStatus : uint8_t {
	        DeviceFound     = 0,    /**< Device found and more devices available */
	        LastDeviceFound = 1,    /**< Last device found; no more undiscovered devices. */
            ResetFailed     = 2,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed    = 3,    /**< Pull up failed or bus data line shorted to GND */
            SearchFailed    = 4,    /**< No device responded to the bit query. */
            CRCFailed       = 5     /**< CRC check of ROM-ID failed */
        };
        /**
        * @enum WriteScratchStatus
        * @brief writeScratch function status codes
        */
        enum class WriteScratchStatus : uint8_t {
	        TransmissionSuccessful  = 0,    /**< Data transfer successful */
	        ResetFailed			    = 1,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed            = 2,    /**< Pull up failed or bus data line shorted to GND */
            CrcFailed               = 3,    /**< CRC check of data failed */
            TransmissionFailed      = 4     /**< Data transfer failed */
        };
        /**
        * @enum CopyScratchStatus
        * @brief copyScratch function status codes
        */
        enum class CopyScratchStatus : uint8_t {
            CopySuccessful  = 0,    /**< Copying process successful */
	        ResetFailed     = 1,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed    = 2     /**< Pull up failed or bus data line shorted to GND */
        };
        /**
        * @enum RecallMemoryStatus
        * @brief recallMemory function status codes
        */
        enum class RecallMemoryStatus : uint8_t {
            RecallSuccessfull   = 0,    /**< Recall successfull */
	        ResetFailed         = 1,    /**< Reset failed, no presence puls or no device on bus */
            PullUpFailed        = 2,    /**< Pull up failed or bus data line shorted to GND */
        };
        /**
        * @brief Constructor for Device class
        * 
        * This function is the constructor for the Device class and accepts the
        * bus interface from Bus. It provides basic, device-independent
        * functionality, such as ROM ID searching, CRC checking, etc.
        * 
        * @param[in] Bus interface from Bus
        */
        explicit Device(Bus&);
        ~Device();
        /**
        * @brief Search algorithm for 1-Wire devices on the bus
        * 
        * The algorithm was implemented similar to Maxim (Dallas) Application
        * Note 187. The search performs a binary tree search to locate all
        * devices on the 1-Wire bus.
        * 
        * One device is found per call, and the determined ROM-ID is
        * returned. To find all devices, the function must be called repeatedly
        * until it returns SearchDeviceStatus::LastDeviceFound. If an error
        * occurs during the search, the process is aborted and the error status
        * is returned and on the next call the search then starts over from the
        * beginning.
        * 
        * The search status is saved after each invocation so that the next
        * device can be searched for during the subsequent call. The search
        * status is reset only after the last device or following a failed
        * search. If, for any reason, the search needs to be reset manually,
        * the function Device:resetSearch() can be called.
        * 
        * @see Device:resetSearch
        * @see Device:getRomId
        * 
        * This example shows how to perform a complete search for all devices
        * on the bus:
        * 
        * \code{.cpp}
        * Device::SearchDeviceStatus status;
        * 
        * do{
        *    uint8_t romId[8];
        *    status = device1.searchDevice(false);
        *    if(status == Device::SearchDeviceStatus::DeviceFound){
        *       device1.getRomId(romId);
        *       // ...Code that processes the found ROM-ID...
        *    }
        * }while(status == Device::SearchDeviceStatus::DeviceFound);
        * 
        * switch (status)
        * {
        *    case Device::SearchDeviceStatus::LastDeviceFound:
        *       // ...search complete, no further devices....
        *       break;
        *    case Device::SearchDeviceStatus::ResetFailed:
        *       // ...error handling code...
        *       break;
        *    case Device::SearchDeviceStatus::CRCFailed:
        *       // ...error handling code...
        *       break;
        *    case Device::SearchDeviceStatus::SearchFailed:
        *       // ...error handling code...
        *       break;
        *    default:
        *       break;
        * }
        * \endcode
        * 
        * The alarmSearch parameter allows you to select whether to search for
        * all devices or only those with an active alarm.
        * 
        * @param[in] alarmSearch true = search only devices with alarms
        * 
        * @return searchDevice function status code
        * @retval Device::SearchDeviceStatus::DeviceFound
        * @retval Device::SearchDeviceStatus::LastDeviceFound
        * @retval Device::SearchDeviceStatus::ResetFailed
        * @retval Device::SearchDeviceStatus::SearchFailed
        * @retval Device::SearchDeviceStatus::CRCFailed
        */
        SearchDeviceStatus searchDevice(bool);
        /**
        * @brief Resets the search
        * 
        * Helper function that resets the last saved state of
        * Device::searchDevice() to start a new search.
        */
        void resetSearch();
        /**
        * @brief Returns the found ROM-ID 
        * 
        * The function returns the last ROM-ID found. To read a valid ROM-ID,
        * a search must first be initiated using Device::searchDevice().
        * 
        * @see Device::searchDevice
        * 
        * @param[out] romId pointer to the target device's ROM ID stored
        */
        void getRomId(uint8_t*);
        /**
        * @brief Select ROM-ID
        * 
        * This function selects a device based on its ROM-ID, or all devices if
        * no ROM-ID is passed.
        * 
        * \code{.cpp} selectRom(romId); \endcode Is equivalent to MatchROM
        * Command with ID xx xx xx xx xx xx xx xx (stored in parameter "romId")
        * 
        * \code{.cpp} selectRom(); \endcode Is equivalent to SkipROM Command
        * 
        * @todo
        * - implementing Family Match command
        */
        void selectRom(const uint8_t* = nullptr);
        /**
        * @brief Writes data to the scratchpad
        * 
        * The function writes data to the scratchpad and verifies correct
        * transmission. To do this, it reads the data back from the scratchpad
        * and checks the CRC checksum.
        * 
        * @param[in] data   Data to be written to the scratchpad
        * @param[in] page   Scratchpad page to be written on
        * @param[in] romId  Pointer to the target device's ROM ID

        * @return writeScratch function status code
        * @retval Device::WriteScratchStatus::TransmissionSuccessful
	    * @retval Device::WriteScratchStatus::ResetFailed
        * @retval Device::WriteScratchStatus::CrcFailed
        * @retval Device::WriteScratchStatus::TransmissionFailed
        */
        WriteScratchStatus writeScratch(uint8_t, uint8_t, const uint8_t* = nullptr);
        /**
        * @brief Copies scratchpad -> EEPROM
        * 
        * The function copies the values ​​in the scratchpad to the target page
        * in SRAM/EEPROM. If the "page" parameter is set to
        * Device::Command::NoPage, sending the target page is skipped. This is
        * used for devices like the DS18B20, which do not have multiple pages
        * in their scratchpad/EEPROM. Otherwise, this parameter specifies the
        * target page in the scratchpad/EEPROM. For example, if you want to
        * write to page 3, you use Device::Command::Page3.
        * 
        * @param[in] romId  Pointer to the target device's ROM ID
        * @param[in] page   Target page in SRAM/EEPROM/Scratchpad
        * 
        * @return copyScratch function status codes
        * @retval Device::CopyScratchStatus::CopySuccessful
        * @retval Device::CopyScratchStatus::ResetFailed
        * @retval Device::CopyScratchStatus::PullUpFailed
        */
        CopyScratchStatus copyScratch(const uint8_t*, uint8_t);
        /**
        * @brief Copies EEPROM -> Scratch Pad
        * 
        * The function copies the values ​​from SRAM/EEPROM to the target page
        * in the scratchpad. If the "page" parameter is set to
        * Device::Command::NoPage, sending the target page is skipped. This is
        * used for devices like the DS18B20, which do not have multiple pages
        * in their scratchpad/EEPROM. Otherwise, this parameter specifies the
        * target page in the scratchpad/EEPROM. For example, if you want to
        * write to page 3, you use Device::Command::Page3.
        * 
        * @param[in] romId  Pointer to the target device's ROM ID
        * @param[in] page   Target page in SRAM/EEPROM/Scratchpad
        * 
        * @return recallMemory function status codes
        * @retval Device::RecallMemoryStatus::RecallSuccessfull
        * @retval Device::RecallMemoryStatus::ResetFailed
        * @retval Device::RecallMemoryStatus::PullUpFailed
        */
        RecallMemoryStatus recallMemory(const uint8_t*, uint8_t);
      protected:
        uint8_t m_data[9];     
        Bus& m_busInterface;
        /**
        * @brief CRC8 - Calculation
        * 
        * The function calculates the CRC8 value using the Maxim (Dallas)
        * polynomial. This is used to verify the correctness of data received
        * from the 1-Wire bus.
        * 
        * Polynomial CRC8 Maxim (Dallas): \f$x^8 + x^5 + x^4 + 1\f$
        * 
        * @return Calculated CRC8 value
        */
        uint8_t calculateCRC8(const uint8_t*, uint8_t);
    private:
        // Variables for 1-Wire search
        bool m_cmpIdBit = false;
        bool m_idBit = false;
        bool m_lastDeviceFlag = false;
        bool m_searchDirection = false;
        uint8_t m_bitMask = 1;
        uint8_t m_byteMask = 0;        
        uint8_t m_idBitIndex = 1;
        uint8_t m_lastDiscrepancy = 0;
        uint8_t m_lastFamilyDiscrepancy = 0;
        uint8_t m_lastZero = 0;
        uint8_t m_romNo[8];
}; // end class Device

} // end namespace OW
