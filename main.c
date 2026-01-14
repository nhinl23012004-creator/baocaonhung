#include "xil_printf.h"
#include "sleep.h"
#include "i2c.h"
#include "ds1307.h"

int main()
{
    rtc_time_t now;

    I2C_Init();
    DS1307_Init();

    xil_printf("DS1307 RTC START\r\n");


    while (1)
    {
        DS1307_ReadTime(&now);

        xil_printf("Time: %02d:%02d:%02d  Date: %02d/%02d/20%02d\r\n",
                   now.hour, now.min, now.sec,
                   now.date, now.month, now.year);

        sleep(1);
    }
}
