/*
 * rtc.h
 *
 *  Created on: Sep 20, 2024
 *      Author: alee9
 */

#ifndef USER_SRC_RTC_H_
#define USER_SRC_RTC_H_



#include <stdint.h>



typedef struct
{
    volatile uint8_t hour;
    volatile uint8_t min;
    volatile uint8_t sec;

    volatile uint16_t w_year;
    volatile uint8_t  w_month;
    volatile uint8_t  w_date;
    volatile uint8_t  week;
} _calendar_obj;



extern _calendar_obj rtclock;



uint8_t RTC_Init(void);
uint8_t Is_Leap_Year(uint16_t year);

uint8_t RTC_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);
uint8_t RTC_Get(void);

uint8_t RTC_Get_Week(uint16_t year, uint8_t month, uint8_t day);

uint8_t RTC_Alarm_Set(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec);



#endif /* USER_SRC_RTC_H_ */
