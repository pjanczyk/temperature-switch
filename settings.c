/*
 * Copyright (C) 2017  Piotr Janczyk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "settings.h"
#include <avr/eeprom.h>

static int8_t EEMEM lower_threshold_eemem = 0;
static int8_t EEMEM upper_threshold_eemem = 0;

void settings_read(int8_t* lower_threshold, int8_t* upper_threshold) {
	*lower_threshold = eeprom_read_byte((uint8_t*) &lower_threshold_eemem);
	*upper_threshold = eeprom_read_byte((uint8_t*) &upper_threshold_eemem);
}
void settings_write(int8_t lower_threshold, int8_t upper_threshold) {
	eeprom_write_byte((uint8_t*) &lower_threshold_eemem, lower_threshold);
	eeprom_write_byte((uint8_t*) &upper_threshold_eemem, upper_threshold);
}
