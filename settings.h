#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>

void settings_read(int8_t* low_temperature, int8_t* high_temperature);
void settings_write(int8_t low_temperature, int8_t high_temperature);


#endif /* SETTINGS_H_ */