// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

/**
 * @file OwHal.h
 * @brief HAL Wrapper for multi platform support
 *
 * OwHal.h serves as a simple wrapper designed to keep the library's core components
 * independent of the underlying hardware driver. It evaluates flags from
 * PlatformIO or the Arduino IDE to include the appropriate hardware library.
 * If no flags are available, the platform can be specified manually.
 * 
 * @warning This wrapper is intended to be a new feature, but it is not yet
 * fully implemented.
 */

 #pragma once

// PlatformIO sets flag to indicate the used platform
#if defined(__AVR__)
    #include <hal/OwAtmega328.h>
#elif defined(STM32F4xx)
    #include <hal/OwHalStm32F4.h>
#else
    #error "Unsupported architecture for OneWire Library!"
#endif
