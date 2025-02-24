/*
 * matrix.c
 *
 *  Created on: Nov 8, 2024
 *      Author: alee9
 */


#include <ch32v20x.h>
#include "matrix.h"

#include "rtc.h"



typedef struct OutPin {
    GPIO_TypeDef *port;
    uint8_t pin;
} OutPin;


static const struct OutPin mcol[MATRIX_COLS] = {
    {GPIOB, 5},
    {GPIOB, 4},
    {GPIOB, 7},
    {GPIOB, 6}
};

static const struct OutPin mrow_hi[MATRIX_ROWS] = {
    {GPIOB, 3},
    {GPIOB, 11},
    {GPIOB, 1},
    {GPIOA, 7},
    {GPIOA, 9},
};

static const struct OutPin mrow_lo[MATRIX_ROWS] = {
    {GPIOB, 3},
    {GPIOB, 10},
    {GPIOB, 0},
    {GPIOA, 6},
    {GPIOA, 10},
};

// using less efficient storage (one byte for col, one byte for row) for faster code and easier understanding
// first byte is column, second byte is row, 0-indexed of course
static const uint8_t hour_map[6][2] = {
    {0, 0}, {1, 0}, {2, 0}, {3, 1},
    {3, 0}, {3, 3}
};

static const uint8_t min_map[7][2] = {
    {0, 2}, {1, 2}, {2, 2}, {3, 2},
    {0, 1}, {1, 1}, {2, 1}
};

static const uint8_t sec_map[7][2] = {
    {0, 4}, {1, 4}, {2, 4}, {3, 4},
    {0, 3}, {1, 3}, {2, 3}
};

static uint8_t matrix_col = 0;        // actively displayed matrix column

uint16_t wakeup_timer;


struct MatrixTime {
    uint8_t h;
    uint8_t h_tens;
    uint8_t h_ones;
    uint8_t m;
    uint8_t m_tens;
    uint8_t m_ones;
    uint8_t s;
    uint8_t s_tens;
    uint8_t s_ones;
} matime;            // precalculated time every loop through all columns

void matrix_gettime()
{
    // rtc_gettime(&matime.h, &matime.m, &matime.s);
    RTC_Get();

    // todo: change me, temporary
    matime.h = rtclock.hour;
    matime.m = rtclock.min;
    matime.s = rtclock.sec;

    // convert to BCD
    matime.h_tens = matime.h / 10;
    matime.h_ones = matime.h % 10;

    matime.m_tens = matime.m / 10;
    matime.m_ones = matime.m % 10;

    matime.s_tens = matime.s / 10;
    matime.s_ones = matime.s % 10;
}

static void matrix_set_pingroup_dir(const struct OutPin *outp, uint8_t count, uint8_t mode)
{
    uint8_t i;
    uint16_t pin;

    for (i = 0; i < count; i++) {
        pin = outp[i].pin;
        if (outp[i].pin >= 8) {
            pin -= 8;
            outp[i].port->CFGHR &= ~(0xf  << (pin*4));
            outp[i].port->CFGHR |=  (mode << (pin*4));
        } else {
            outp[i].port->CFGLR &= ~(0xf << (pin*4));
            outp[i].port->CFGLR |=  (mode << (pin*4));
        }
    }
}

static void matrix_set_pingroup_out(const struct OutPin *outp, uint8_t count, uint8_t hi_lo)
{
    uint8_t i;

    for (i = 0; i < count; i++) {
        if (hi_lo) outp[i].port->BSHR |= (1 << outp[i].pin);
        else       outp[i].port->BCR  |= (1 << outp[i].pin);
    }
}

// call once at startup
void matrix_init()
{
    // we're not using mrow_hi for now, so set as floating input (CFGxR 0x4, 0b0100)
    matrix_set_pingroup_dir(mrow_lo, MATRIX_ROWS, 0x4);

    // configure cols, mrow_lo as high (disabled) 2MHz push/pull outputs (CFGxR 0x2, 0b0010)
    matrix_set_pingroup_out(mrow_hi, MATRIX_ROWS, 1);
    matrix_set_pingroup_dir(mrow_hi, MATRIX_ROWS, 0x2);

    matrix_set_pingroup_out(mcol,    MATRIX_COLS, 1);
    matrix_set_pingroup_dir(mcol,    MATRIX_COLS, 0x2);

    // reset matrix column
    matrix_col = MATRIX_COLS - 1;
}

// call this at some high frequency, ~200-500Hz
uint8_t matrix_next()
{
    uint8_t i;

    uint8_t bits;
    struct OutPin *outp;

    // turn off all columns and rows
    matrix_set_pingroup_out(mcol,    MATRIX_COLS, 1);
    matrix_set_pingroup_out(mrow_hi, MATRIX_COLS, 1);

    // next column
    matrix_col++;

    // reset column when done
    if (matrix_col >= MATRIX_COLS) {
        // reset column
        matrix_col = 0;
    }

    // == BCD mode clock ==
    // now turn on required rows (active low)
    // render hour rows
    bits = matime.h_tens << 4 | matime.h_ones;
    for (i = 0; i < sizeof(hour_map) / sizeof(hour_map[0]); i++) {
        if (matrix_col == hour_map[i][0]) {
            outp = (OutPin *)&mrow_hi[hour_map[i][1]];
            if (bits & 1) outp->port->BCR  = (1 << outp->pin);
            else          outp->port->BSHR = (1 << outp->pin);
        }
        bits >>= 1;
    }

    // render minute rows
    bits = matime.m_tens << 4 | matime.m_ones;
    for (i = 0; i < sizeof(min_map) / sizeof(min_map[0]); i++) {
        if (matrix_col == min_map[i][0]) {
            outp = (OutPin *)&mrow_hi[min_map[i][1]];
            if (bits & 1) outp->port->BCR  = (1 << outp->pin);
            else          outp->port->BSHR = (1 << outp->pin);
        }
        bits >>= 1;
    }

    // render second rows
    bits = matime.s_tens << 4 | matime.s_ones;
    for (i = 0; i < sizeof(sec_map) / sizeof(sec_map[0]); i++) {
        if (matrix_col == sec_map[i][0]) {
            outp = (OutPin *)&mrow_hi[sec_map[i][1]];
            if (bits & 1) outp->port->BCR  = (1 << outp->pin);
            else          outp->port->BSHR = (1 << outp->pin);
        }
        bits >>= 1;
    }

    // enable active column (active low)
    if (wakeup_timer) {
        wakeup_timer--;
        mcol[matrix_col].port->BCR = (1 << mcol[matrix_col].pin);
    }

    return matrix_col;
}
