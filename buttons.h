#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t button_id_t;
enum {
	BUTTON_MINUS_ID = 0,
	BUTTON_PLUS_ID = 1
};

/* Routine called from interrupt every 2ms */
void buttons_routine(void);

bool was_button_pressed(button_id_t id);
bool was_button_long_pressed(button_id_t id);

#endif /* BUTTONS_H_ */