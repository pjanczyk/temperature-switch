#include "settings.h"
#include <avr/eeprom.h>

static int8_t EEMEM low_temperature_eemem = 0;
static int8_t EEMEM high_temperature_eemem = 0;

void settings_read(int8_t* low_temperature, int8_t* high_temperature) {
	*low_temperature = eeprom_read_byte((uint8_t*) &low_temperature_eemem);
	*high_temperature = eeprom_read_byte((uint8_t*) &high_temperature_eemem);
}
void settings_write(int8_t low_temperature, int8_t high_temperature) {
	eeprom_write_byte((uint8_t*) &low_temperature_eemem, low_temperature);
	eeprom_write_byte((uint8_t*) &high_temperature_eemem, high_temperature);
}
