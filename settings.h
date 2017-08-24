/*
 * Copyright (C) 2017  Piotr Janczyk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>

void settings_read(int8_t* lower_threshold, int8_t* upper_threshold);
void settings_write(int8_t lower_threshold, int8_t upper_threshold);


#endif /* SETTINGS_H_ */