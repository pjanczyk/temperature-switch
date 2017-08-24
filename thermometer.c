#include "thermometer.h"
#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>

/*
 * 1-wire Software Implementation
 * (based on AVR318 note)
 */


/*
 * Physical Layer Functions
 */

/* Sets bus pin to output low */
static inline void onewire_bus_pull_down(void) {
	// assume PORT bit is 0
	// change direction bit
	DDRB |= (1 << PB0);
}

/* Sets bus pin to input */
static inline void onewire_bus_release(void) {
	// assume PORT bit is 0
	// change direction bit
	DDRB &= ~(1 << PB0);
}

/* Returns a logic value of the bus pin */
static inline bool onewire_bus_read_state(void) {
	return (PINB & (1 << PB0)) ? true : false;
}


/*
 * Bit Transfer Layer Functions
 */

static inline void onewire_write_bit_1(void) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		onewire_bus_pull_down();
		_delay_us(6);
		onewire_bus_release();
		_delay_us(64);
	}
}

static inline void onewire_write_bit_0(void) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		onewire_bus_pull_down();
		_delay_us(60);
		onewire_bus_release();
		_delay_us(10);
	}
}

static inline bool onewire_read_bit(void) {
	bool result;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		onewire_bus_pull_down();
		_delay_us(6);
		onewire_bus_release();
		_delay_us(9);
		result = onewire_bus_read_state();
		_delay_us(55);
	}
	return result;
}

static inline bool onewire_detect_presence(void) {
	bool responce;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		onewire_bus_pull_down();
		_delay_us(480);
		onewire_bus_release();
		_delay_us(70);
		responce = onewire_bus_read_state();
		_delay_us(410);
	}
	
	return !responce;
}


/*
 * Byte Transmission Functions
 */

static inline void onewire_send_byte(uint8_t data) {
	for (uint8_t i = 0; i < 8; i++) {
		if (data & 0x01) {
			onewire_write_bit_1();
		} else {
			onewire_write_bit_0();
		}
		data >>= 1;
	}
}

static inline uint8_t onewire_receive_byte(void) {
	uint8_t data = 0;
	for (uint8_t i = 0; i < 8; i++) {
		data >>= 1;
		if (onewire_read_bit())
			data |= (1 << 7); // set MSB
	}
	return data;
}

/*
 *  DS18B20 Specific Functions
 */

static const uint8_t DS18B20_COMMAND_CONVERT = 0x44;
static const uint8_t DS18B20_COMMAND_READ_SCRACHPAD = 0xBE;

static inline uint8_t compute_crc8(uint8_t seed, uint8_t data) {
	for (uint8_t j = 0; j < 8; j++) {
		if ((seed ^ data) & 0x01) {
			seed ^= 0x18;
			seed >>= 1;
			seed |= 0x80;
			} else {
			seed >>= 1;
		}
		data >>= 1;
	}
	return seed;
}

typedef union {
	uint8_t data[9];
	struct {
		uint8_t temperature_lsb;
		uint8_t temperature_msb;
		uint8_t th_register;
		uint8_t tl_register;
		uint8_t configuration_register;
		uint8_t reserved[3];
		uint8_t crc;
	};
} scratchpad_t;

bool thermometer_begin_measure(void) {
	if (!onewire_detect_presence())
		return false;
	
	onewire_send_byte(DS18B20_COMMAND_CONVERT);
	return true;
}

bool thermometer_finish_measure(int8_t* result) {
	if (!onewire_detect_presence())
		return false;
	
	onewire_send_byte(DS18B20_COMMAND_READ_SCRACHPAD);
	
	scratchpad_t scratchpad;
	for (uint8_t i = 0; i < 9; i++) {
		scratchpad.data[i] = onewire_receive_byte();
	}
	
	// compute CRC
	uint8_t crc = 0;
	for (uint8_t i = 0; i < 8; i++) {
		crc = compute_crc8(crc, scratchpad.data[i]);
	}
	
	if (crc != scratchpad.crc)
		return false;
	
	*result = (scratchpad.temperature_msb << 4) | (scratchpad.temperature_lsb >> 4);
	return true;	
}

