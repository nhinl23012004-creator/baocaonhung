#include "ds1307.h"
#include "i2c.h"

#define DS1307_ADDR_W  0xD0
#define DS1307_ADDR_R  0xD1

/* ================= BCD <-> DEC ================= */
static uint8_t bcd_to_dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t dec_to_bcd(uint8_t dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

/* ================= INIT ================= */
void DS1307_Init(void)
{
    I2C_Start();
    tram(DS1307_ADDR_W);
    tram(0x00);        // Seconds register
    tram(0x00);        // Clear CH bit -> RTC RUN
    I2C_Stop();
}

/* ================= READ TIME (FIXED) ================= */
void DS1307_ReadTime(rtc_time_t *t)
{
    uint8_t raw;

    /* ---- STEP 1: Set register pointer (NO STOP) ---- */
    I2C_Start();
    tram(DS1307_ADDR_W);
    tram(0x00);              // Start at seconds register

    /* ---- STEP 2: REPEATED START ---- */
    I2C_Start();
    tram(DS1307_ADDR_R);

    /* ---- STEP 3: Read data ---- */
    raw = nhan(0);           // Seconds
    raw &= 0x7F;             // CLEAR CH BIT (bit7)
    t->sec = bcd_to_dec(raw);

    raw = nhan(0);           // Minutes
    t->min = bcd_to_dec(raw);

    raw = nhan(0);           // Hours
    raw &= 0x3F;             // 24-hour mode mask
    t->hour = bcd_to_dec(raw);

    t->day   = bcd_to_dec(nhan(0)); // Day
    t->date  = bcd_to_dec(nhan(0)); // Date
    t->month = bcd_to_dec(nhan(0)); // Month
    t->year  = bcd_to_dec(nhan(1)); // Year (NACK last byte)

    I2C_Stop();
}

/* ================= SET TIME ================= */
void DS1307_SetTime(rtc_time_t *t)
{
    I2C_Start();
    tram(DS1307_ADDR_W);
    tram(0x00);

    tram(dec_to_bcd(t->sec));
    tram(dec_to_bcd(t->min));
    tram(dec_to_bcd(t->hour));
    tram(dec_to_bcd(t->day));
    tram(dec_to_bcd(t->date));
    tram(dec_to_bcd(t->month));
    tram(dec_to_bcd(t->year));

    I2C_Stop();
}
