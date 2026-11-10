// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 M. Fiedler

#include <stdio.h>
#include <avr/io.h>
#include "Serial.h"

Serial::Serial() {
	UBRR0 = 51;		//Baud 19.2k
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ00) |
	(1 << UCSZ01);
	return;
}

Serial::~Serial() {
	
}

void Serial::serialSend(const char* string) {
	while((*string)) {
		while(!(UCSR0A & 0b00100000)){}
		UDR0 = *string;
		while(!(UCSR0A & 0b01000000)){}
		string += 1;
	}
}

void Serial::serialSend(uint8_t number) {
	//sprintf( String, "%d", number );
	char String = number + '0';
	while((String)) {
		while(!(UCSR0A & 0b00100000)){}
		UDR0 = String;
		while(!(UCSR0A & 0b01000000)){}
		String += 1;
	}
}
