![Status](https://img.shields.io/badge/Status-Work%20in%20Progress-lightgrey.svg)
[![GitHub License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)
![C++ Standard](https://img.shields.io/badge/C%2B%2B-17%2F20-blue.svg)
![PlatformIO Registry](https://img.shields.io/badge/PlatformIO-supported-orange.svg)
![Doxygen Documentation](https://img.shields.io/badge/Docs-doxygen-brightgreen.svg)

# OneWire embedded C++ Library

<div align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="images/logos/logo-git-dark.svg">
    <source media="(prefers-color-scheme: light)" srcset="images/logos/logo-git-light.svg">
    <img alt="Projekt Logo" src="images/logos/logo-git-light.svg" width="450">
  </picture>
</div>

A clean, object-oriented C++ library for 1-Wire bus communication and devices (like DS18B20 and DS2438), free of Arduino IDE bloat and equipped with a dedicated Hardware Abstraction Layer (HAL).

## Features

- **Modern C++ Architecture:** Clear class hierarchy (`Bus`, `Device`, specific sensors).
- **No Arduino Bloat:** Decoupled from heavy Arduino core overhead through a clean HAL interface.
- **Robust Error Handling:** Expressive `enum class` status codes for resets, CRC checks, and transmissions.
- **Easy-to-use drivers** for 1-Wire devices without needing to use low-level commands or understand the protocol.
- **Supported Devices:**
    - DS18B20 (Temperature)
    - DS2438 (Smart Battery Monitor)

## 📝 Documentation

A lot of effort went into writing comprehensive **Doxygen documentation** to make the library as easy to use and integrate as possible. Every class, method, and status code is fully documented with usage examples and return values.

## Background

This project actually started just for personal use in a custom 1-Wire gateway. However, it grew rapidly and has now reached a solid state. It would be a shame to keep this code locked away on my hard drive—especially since modern C++ libraries for 1-Wire are hard to come by. That is why I decided to release it as open source, in the hope that it might be useful to someone else.

Work is still in progress. 👨🏻‍💻

---

## 🛠️ Getting started with an example

Here is a description of how to compile and test the existing examples in the `examples` folder. The project contains pre-built PlatformIO environments to make compiling as easy as possible. The examples are described in detail in the Doxygen documentation.

> [!IMPORTANT]
> The examples are currently only available for the AVR platform.

### Requirements

- Software: VS Code with installed PlatformIO
- Hardware: Arduino Nano (ATmega328P)
- One DS18B20 sensor
- 4.7 kΩ pull-up resistor
- Sensor wiring:

```
┌──────────┐            +5V            +5V
┆          │             │              │
┆          │            ┌┴┐             │
┆          │            │ │ 4.7k   ┌────┴────┐
┆ Arduino  │            └┬┘        │         │
┆ Nano     ├<pin D7>─────┴─────<DQ>┤ DS18B20 │
┆          │                       │         │
┆          │                       └────┬────┘
┆          │                            │
┆          │                            │
└──────────┘                           GND
```

### Download & compile the project

1. Download the GitHub repository or clone it via Terminal:
```bash
   git clone https://github.com/thoryn-dev/OneWire-Emb-Cpp-lib.git
```
2. Open the project folder with VS Code.
3. In the bottom status bar of VS Code, click the list box to select the PlatformIO environments.
4. Select the `env:example_ds18b20_Atmega328` environment in the list.
5. Connect your Arduino Nano to your computer via USB.
6. Click the **checkmark icon (Build)** in the bottom status bar to compile the example.
7. Then click the **arrow icon (Upload)** to upload the program to the microcontroller.
8. In VS Code, open the **Serial Monitor** (plug icon in the bottom status bar).
9. Set the baud rate to `19200` baud.
10. The current temperature values ​​are now displayed in the terminal as follows:
```
Convert: finished!
Receive: successful!
Temperature mdegC: 22500
Temperature degC: 22.50
```
---

## ⚖️ License & Copyright

### Source Code
The source code of this project is licensed under the **Apache License 2.0**. See the [LICENSE](LICENSE) file for details.

### Project Logo
The project logos (located in `images/logos/`) are **Copyright © 2026 M. Fiedler (thoryn-dev)**. All rights reserved.  
You may not copy, modify, distribute, or use the logos without explicit prior written permission, except when referring directly to this project (e.g., in links or articles about this library).

### Trademarks
1-Wire is a registered trademark of Analog Devices, Inc. This library is an independent open-source implementation and is not affiliated with, endorsed by, or sponsored by Analog Devices, Inc.