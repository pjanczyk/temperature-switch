#ifndef THERMOMETER_H_
#define THERMOMETER_H_

#include <stdbool.h>
#include <stdint.h>

bool thermometer_begin_measure(void);
bool thermometer_finish_measure(int8_t* result);

#endif /* THERMOMETER_H_ */