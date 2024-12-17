/*
 * buttons.c
 *
 *  Created on: Nov 13, 2024
 *      Author: alee9
 */



#include <ch32v20x.h>
#include "buttons.h"

#include "matrix.h"     // for wakeup_timer
#include "rtc.h"



#define BTN_PORT                GPIOA

#define BTN0_PIN                0       // WKUP
#define BTN1_PIN                1       // HOURS
#define BTN2_PIN                5       // MINUTES

#define BTN0_BIT                (1 << BTN0_PIN)
#define BTN1_BIT                (1 << BTN1_PIN)
#define BTN2_BIT                (1 << BTN2_PIN)

#define WAKEUP_TIME_DEFAULT     6000        // 600 * seconds



uint16_t btn_pushed = 0;



void btn_init()
{
    GPIO_InitTypeDef gpio;

    // Enable GPIOA clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure PA3 and PA4 as outputs
    gpio.GPIO_Pin = BTN0_BIT | BTN1_BIT | BTN2_BIT;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Speed = GPIO_Speed_2MHz; // Set GPIO speed
    GPIO_Init(BTN_PORT, &gpio);

    // by default, show the LEDs after powering on
    wakeup_timer = WAKEUP_TIME_DEFAULT;
}

void btn_process()
{
    int any_button_pressed = 0;

    // is wkup pushed?
    if (!(BTN_PORT->INDR & BTN0_BIT)) {
        btn_pushed |= BTN0_BIT;
        any_button_pressed = 1;
    } else {
        btn_pushed &= ~BTN0_BIT;
    }
    if (!(BTN_PORT->INDR & BTN0_BIT)) {
            btn_pushed |= BTN0_BIT;
            any_button_pressed = 1;
        } else {
            btn_pushed &= ~BTN0_BIT;
        }

    // is HOURS pushed?
    if (!(BTN_PORT->INDR & BTN1_BIT)) {
        if (!(btn_pushed & BTN1_BIT)) {
            btn_pushed |= BTN1_BIT;
            if (wakeup_timer) {
                RTC_Get();
                rtclock.hour++;
                if (rtclock.hour >= 24)
                    rtclock.hour = 0;

                RTC_Set(2024, 1, 1, rtclock.hour, rtclock.min, rtclock.sec);
            }
        }
        any_button_pressed = 1;
    } else {
        btn_pushed &= ~BTN1_BIT;
    }

    // is MIN pushed?
    if (!(BTN_PORT->INDR & BTN2_BIT)) {
        if (!(btn_pushed & BTN2_BIT)) {
            btn_pushed |= BTN2_BIT;
            if (wakeup_timer) {
                RTC_Get();
                rtclock.min++;
                if (rtclock.min >= 60)
                    rtclock.min = 0;

                RTC_Set(2024, 1, 1, rtclock.hour, rtclock.min, rtclock.sec);
            }
        }
        any_button_pressed = 1;
    } else {
        btn_pushed &= ~BTN2_BIT;
    }

    // Set wakeup timer if any button is pressed
    if (any_button_pressed) {
        wakeup_timer = WAKEUP_TIME_DEFAULT;
    }
}

