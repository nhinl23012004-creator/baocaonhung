#ifndef __DS1307_H_
#define __DS1307_H_

#include <stdint.h>

typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} rtc_time_t;

void DS1307_Init(void);
void DS1307_ReadTime(rtc_time_t *t);
void DS1307_SetTime(rtc_time_t *t);

#endif
