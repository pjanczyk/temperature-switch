/*
 * Copyright (C) 2017  Piotr Janczyk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef THERMOMETER_H_
#define THERMOMETER_H_

#include <stdbool.h>
#include <stdint.h>

bool thermometer_begin_measure(void);
bool thermometer_finish_measure(int8_t* result);

#endif /* THERMOMETER_H_ */