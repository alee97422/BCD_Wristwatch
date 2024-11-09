/*
 * matrix.h
 *
 *  Created on: Nov 8, 2024
 *      Author: alee9
 */

#ifndef USER_SRC_MATRIX_H_
#define USER_SRC_MATRIX_H_



#include <stdint.h>



#define MATRIX_COLS    4
#define MATRIX_ROWS    5



void matrix_init();
uint8_t matrix_next();
void matrix_gettime();



#endif /* USER_SRC_MATRIX_H_ */
