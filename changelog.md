
# Changelog

All significant changes to the **OneWire embedded C++ Library** are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com) and this project adheres to [Semantic Versioning](https://semver.org).

## [Unreleased]

### Added
- Pre-configured **PlatformIO environment** in `platformio.ini` for easy selection and execution of the DS18B20 examples.
- **Device Family Codes:** Added entries for numerous additional 1-Wire family codes to the FamilyCode enum list (e.g., DS2401, DS18S20, DS2431).

### Changed
- Example project for the DS18B20 sensor under `examples/ds18b20/main.cpp` revised.
- **Device::calculateCRC8()** Changed length parameter from uint8_t to size_t to support larger buffers and prevent narrowing warnings.

### Deprecated
-

### Removed
-

### Fixed
- 

### Security
-

---