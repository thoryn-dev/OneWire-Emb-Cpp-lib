// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

#pragma once
#include <avr/io.h>
#include <stdlib.h>

class Serial {
	public:
		Serial();	// Constructor
		~Serial();	// Destructor
		
		void serialSend(const char*);
		void serialSend(uint8_t);
};