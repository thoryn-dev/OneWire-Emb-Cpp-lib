
# Changelog

All significant changes to the **OneWire embedded C++ Library** are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com) and this project adheres to [Semantic Versioning](https://semver.org).

## [Unreleased]

### Added
- Template **Ds18B20::getTemperature()** added for returning temperature in different data types and formats.
- Pre-configured **PlatformIO environment** in `platformio.ini` for easy selection and execution of the DS18B20 examples.
- **Device Family Codes:** Added entries for numerous additional 1-Wire family codes to the FamilyCode enum list (e.g., DS2401, DS18S20, DS2431).

### Changed
- Moved Serial implementation into the ATmega328 HAL as Class SerialConsole
- **Bare-Metal AVR Support:** Removed the dependency on the Arduino framework in `platformio.ini` and switched architecture detection to compiler-native AVR macros (`__AVR__`) for true framework-independent usage.
- Example project for the DS18B20 sensor under `examples/ds18b20/main.cpp` revised.
- **Device::calculateCRC8()** Changed length parameter from uint8_t to size_t to support larger buffers and prevent narrowing warnings.

### Deprecated
-

### Removed
- Unused function **serialSend(uint8_t)** is removed from OwAtmega328.h and OwAtmega328.cpp.

### Fixed
- 

### Security
-

---