#include "relay.h"
#include <avr/io.h>

void relay_set_on(bool on) {
	if (on) {
		PORTB |= (1 << PB2);
	} else {
		PORTB &= ~(1 << PB2);
	}
}