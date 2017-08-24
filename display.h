#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

typedef uint8_t display_flags_t;
enum {
	DISPLAY_FLAG_RED = 1,
	DISPLAY_FLAG_GREEN = 2,
	DISPLAY_FLAG_BLINK = 4,
	DISPLAY_FLAG_YELLOW = (DISPLAY_FLAG_RED | DISPLAY_FLAG_GREEN)
};

// only values from range [-99, 99] are allowed
static const int8_t DISPLAY_ERROR_NUMBER = 100;

/* Routine called from interrupt every 2ms */
void display_routine(void);

void display_show_number(int8_t number, uint8_t flags);
void display_show_nothing(void);

#endif /* DISPLAY_H_ */