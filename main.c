/*
 * Copyright (C) 2017  Piotr Janczyk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "buttons.h"
#include "display.h"
#include "relay.h"
#include "settings.h"
#include "thermometer.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>
#include <util/delay.h>

// Must be defined: F_CPU = 1000000L (1MHz)

static const int8_t MIN_THRESHOLD_TEMPERATURE = -50;
static const int8_t MAX_THRESHOLD_TEMPERATURE = 50;

volatile uint16_t setting_counter = 0;

int8_t lower_threshold; // lower threshold temperature
int8_t upper_threshold; // upper threshold temperature

ISR(TIMER0_OVF_vect) { // called every 2 ms
	display_routine();
	buttons_routine();
	
	setting_counter++;
}

static int8_t clamp_i8(int8_t val, int8_t min, int8_t max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

static void setup(void) {
	/* I/O
	 * --------------------------------------------------------------
	 *   PB0     -- 1-wire               -- input
	 *   PB1     -- *unused*             -- input, pull-up enabled
	 *   PB2     -- relay                -- output, high
	 *   PB3-PB5 -- *unused*             -- input, pull-up enabled
	 *   PB6-PB7 -- switches             -- input, pull-up enabled
	 *
	 *   PC0-PC3 -- led display anodes   -- output, low
	 *   PC4-PC5 -- *unused*             -- input, pull-up enabled
	 *
	 *   PD0-PD7 -- led display cathodes -- output, high
	 */
	
	PORTB = (1 << PB1) | (1 << PB2) | (1 << PB3) | (1 << PB4) | (1 << PB5) | (1 << PB6) | (1 << PB7);
	DDRB = (1 << PB2);
	
	PORTC = (1 << PC4) | (1 << PC5);
	DDRC = (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3);
	
	PORTD = 0xFF;
	DDRD = 0xFF;
	
	settings_read(&lower_threshold, &upper_threshold);
	lower_threshold = clamp_i8(lower_threshold, MIN_THRESHOLD_TEMPERATURE, MAX_THRESHOLD_TEMPERATURE - 1);
	upper_threshold = clamp_i8(upper_threshold, MIN_THRESHOLD_TEMPERATURE + 1, MAX_THRESHOLD_TEMPERATURE);

	
	display_show_nothing();
	
	
	/* Timer */
	TCCR0 |= 1 << CS01; // prescaler 8, timer interrupt every 2 ms
	TIMSK |= 1 << TOIE0;
	sei();
}

static void setting_loop(void) {
	enum { NONE, LOW, HIGH } setting = NONE;
	
	if (was_button_long_pressed(BUTTON_MINUS_ID))
		setting = LOW;
	if (was_button_long_pressed(BUTTON_PLUS_ID))
		setting = HIGH;
	
	if (setting == NONE)
		return;
	
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		setting_counter = 0;
	}
	
	int8_t val;
	int8_t min_value;
	int8_t max_value;
	display_flags_t color;
	
	if (setting == LOW) {
		val = lower_threshold;
		min_value = MIN_THRESHOLD_TEMPERATURE;
		max_value = MAX_THRESHOLD_TEMPERATURE - 1;
		color = DISPLAY_FLAG_GREEN;
	} else {
		val = upper_threshold;
		min_value = MIN_THRESHOLD_TEMPERATURE + 1;
		max_value = MAX_THRESHOLD_TEMPERATURE;
		color = DISPLAY_FLAG_RED;
	}
	
	bool timeout = false; // after 4 sec without button press
	
	while (!timeout) {
		display_show_number(val, color | DISPLAY_FLAG_BLINK);
		
		bool reset_setting_counter = false;
		
		if (was_button_pressed(BUTTON_MINUS_ID)) {
			if (val > min_value)
				val--;
			reset_setting_counter = true;
		}
		if (was_button_pressed(BUTTON_PLUS_ID)) {
			if (val < max_value)
				val++;
			reset_setting_counter = true;
		}
		
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (reset_setting_counter)
				setting_counter = 0;
				
			if (setting_counter >= 2000) // 2000 * 2ms = 4sec
				timeout = true;
		}
	}
	
	if (setting == LOW) {
		lower_threshold = val;
		
		if (upper_threshold <= val)
			upper_threshold = val + 1;
	} else {
		upper_threshold = val;
		
		if (lower_threshold >= val)
			lower_threshold = val - 1;
	}
	
	settings_write(lower_threshold, upper_threshold);
}

static void main_loop(void) {
	int8_t current_temperature;
	
	for (;;) {
		if (!thermometer_begin_measure()) {
			// If temperature is unknown, for safety reasons we set relay state to ON.
			relay_set_on(true);
			
			display_show_number(DISPLAY_ERROR_NUMBER, DISPLAY_FLAG_YELLOW);
			_delay_ms(1900);
			display_show_nothing();
			_delay_ms(100);
			continue;
		}

		display_show_number(lower_threshold, DISPLAY_FLAG_GREEN);
		_delay_ms(900);
		display_show_nothing();
		_delay_ms(100);

		setting_loop();

		display_show_number(upper_threshold, DISPLAY_FLAG_RED);
		_delay_ms(900);
		display_show_nothing();
		_delay_ms(100);
		
		setting_loop();
		
		if (!thermometer_finish_measure(&current_temperature)) {
			// If temperature is unknown, for safety reasons we set relay state to ON.
			relay_set_on(true);
			
			display_show_number(DISPLAY_ERROR_NUMBER, DISPLAY_FLAG_YELLOW);
			_delay_ms(1900);
			display_show_nothing();
			_delay_ms(100);
			continue;
		}
		
		if (current_temperature <= lower_threshold) {
			relay_set_on(true);
		} else if (current_temperature >= upper_threshold) {
			relay_set_on(false);	
		}
		
		display_show_number(current_temperature, DISPLAY_FLAG_YELLOW);
		_delay_ms(1900);
		display_show_nothing();
		_delay_ms(100);
		
		setting_loop();
	}
}


int main(void) {
	setup();
	main_loop();
}
