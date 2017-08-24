#include "buttons.h"
#include <util/atomic.h>

typedef uint8_t button_event_t;
enum {
	BUTTON_EVENT_NONE = 0,
	BUTTON_EVENT_PRESSED = 1,
	BUTTON_EVENT_LONG_PRESSED = 2
};


static volatile button_event_t button_events[2] = { BUTTON_EVENT_NONE, BUTTON_EVENT_NONE };

void buttons_routine(void) {
	static uint16_t button_counters[2] = { 0, 0 };
	
	bool currently_pressed[2] = {
		!(PINB & (1 << PB6)),
		!(PINB & (1 << PB7))
	};
	
	for (uint8_t i = 0; i < 2; i++) {
		if (currently_pressed[i]) {
			if (button_counters[i] == 1000) { // 1000 * 2ms = 2sec
				button_events[i] = (BUTTON_EVENT_PRESSED | BUTTON_EVENT_LONG_PRESSED);
			}
			
			if (button_counters[i] == 4) { // 4 * 2ms = 8 ms
				button_events[i] = BUTTON_EVENT_PRESSED;
			}
			
			// increase counter until it reach 1001
			if (button_counters[i] <= 1000) {
				button_counters[i]++;
			}
		} else {
			button_counters[i] = 0;
		}
	}
}

static button_event_t pop_button_event(button_id_t id) {
	button_event_t result;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		result = button_events[id];
		button_events[id] = BUTTON_EVENT_NONE;
	}
	return result;
}

bool was_button_pressed(button_id_t id) {
	return (pop_button_event(id) & BUTTON_EVENT_PRESSED) ? true : false;
}

bool was_button_long_pressed(button_id_t id) {
	return (pop_button_event(id) & BUTTON_EVENT_LONG_PRESSED) ? true : false;
}