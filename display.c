#include "display.h"
#include <avr/pgmspace.h>
#include <stdbool.h>

volatile uint8_t display_port[4] = { 0, 0, 0, 0 };
volatile bool display_blink = false;

/**
 * Display connection:
 *
 *  Anodes:
 *   left digit, green  - PC0 
 *   right digit, green - PC1
 *   left digit, red    - PC2
 *   right digit, red   - PC3
 *
 *  Cathodes:
 *   A - PD2
 *   B - PD3
 *   C - PD6
 *   D - PD5
 *   E - PD4
 *   F - PD1
 *   G - PD7
 *   DOT - PD0
*/
static const uint8_t PROGMEM DISPLAY_DIGIT_SEGMENTS[13] = {
	0x7E,	// 0
	0x48,	// 1
	0xBC,	// 2
	0xEC,	// 3
	0xCA,	// 4
	0xE6,	// 5
	0xF6,	// 6
	0x4E,	// 7
	0xFE,	// 8
	0xEE,	// 9
	0x00,	// empty
	0xB6,	// E  (used to display "Er")
	0x90,	// r  (used to display "Er")
};


static inline uint8_t abs8(int8_t v) {
	if (v < 0) {
		return -v;
	} else {
		return v;
	}
}


void display_routine(void) {
	static uint8_t current_digit = 0;
	static uint8_t blink_counter = 0;
	
	blink_counter = (blink_counter + 1) % 256;
	current_digit = (current_digit + 1) % 6;
	
	PORTD = 0xff;
	if (!display_blink || blink_counter >= 64) {
		PORTC = 1 << (current_digit % 4);
		PORTD = ~display_port[current_digit % 4];
	}
}

void display_show_number(int8_t number, uint8_t flags) {
	uint8_t absolute = abs8(number);
	
	uint8_t d1;
	uint8_t d2;
	
	if (number >= -9 && number <= 9) {
		d1 = absolute % 10;
		d2 = 10; // empty
	} else if (number >= -99 && number <= 99) {
		d1 = absolute % 10;
		d2 = absolute / 10;
	} else {
		d1 = 12; // R
		d2 = 11; // e
	}
	
	uint8_t minus_sign = (number < 0) ? 1 : 0;
	
	d1 = pgm_read_byte(&DISPLAY_DIGIT_SEGMENTS[d1]);
	d2 = pgm_read_byte(&DISPLAY_DIGIT_SEGMENTS[d2]) | minus_sign;
	
	display_port[0] = (flags & DISPLAY_FLAG_GREEN) ? d2 : 0;
	display_port[1] = (flags & DISPLAY_FLAG_GREEN) ? d1 : 0;
	display_port[2] = (flags & DISPLAY_FLAG_RED) ? d2 : 0;
	display_port[3] = (flags & DISPLAY_FLAG_RED) ? d1 : 0;
	display_blink = (flags & DISPLAY_FLAG_BLINK);
}

void display_show_nothing(void) {
	display_port[0] = 0;
	display_port[1] = 0;
	display_port[2] = 0;
	display_port[3] = 0;
}
