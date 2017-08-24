/*
 * Copyright (C) 2017  Piotr Janczyk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "relay.h"
#include <avr/io.h>

void relay_set_on(bool on) {
	if (on) {
		PORTB |= (1 << PB2);
	} else {
		PORTB &= ~(1 << PB2);
	}
}