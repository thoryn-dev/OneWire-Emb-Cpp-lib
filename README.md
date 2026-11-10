![Status](https://img.shields.io/badge/Status-Work%20in%20Progress-lightgrey.svg)
![License](https://img.shields.io/badge/license-GPLv3-blue.svg)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-17%2F20-blue.svg)
![PlatformIO Registry](https://img.shields.io/badge/platformio-supported-orange.svg)
![Doxygen Documentation](https://img.shields.io/badge/docs-doxygen-brightgreen.svg)

# OneWire MCU C++ Library

A clean, object-oriented C++ library for 1-Wire bus communication and devices (like DS18B20 and DS2438), free of Arduino IDE bloat and equipped with a dedicated Hardware Abstraction Layer (HAL).

## Features

- **Modern C++ Architecture:** Clear class hierarchy (`Bus`, `Device`, specific sensors).
- **No Arduino Bloat:** Decoupled from heavy Arduino core overhead through a clean HAL interface.
- **Robust Error Handling:** Expressive `enum class` status codes for resets, CRC checks, and transmissions.
- **Supported Devices:** DS18B20 (Temperature), DS2438 (Smart Battery Monitor / Voltage/Temperature).

## 📝 Documentation

A lot of effort went into writing comprehensive **Doxygen documentation** to make the library as easy to use and integrate as possible. Every class, method, and status code is fully documented with usage examples and return values.

## Background

This project actually started just for personal use in a custom OneWire gateway. However, it grew rapidly and has now reached a solid state. It would be a shame to keep this code locked away on my hard drive—especially since modern C++ libraries for 1-Wire are hard to come by. That is why I decided to release it as open source, in the hope that it might be useful to someone else.

Work is still in progress. 👨🏻‍💻

## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.
