#include "i2c.h"

void I2C_Init(void)
{
    volatile uint32_t* ctrl = (uint32_t*)REG_CTRL_ADDR;

    *ctrl = 0x00;
    usleep(10);
    *ctrl = CTRL_ENABLE;
    usleep(10);
}

void I2C_Start(void)
{
    volatile uint32_t* ctrl   = (uint32_t*)REG_CTRL_ADDR;
    volatile uint32_t* status = (uint32_t*)REG_STATUS_ADDR;

    *ctrl = CTRL_ENABLE | CTRL_START;
    while ((*status & STS_DONE) == 0);

    usleep(10);
    *ctrl = CTRL_ENABLE;
    usleep(10);
}

void I2C_Stop(void)
{
    volatile uint32_t* ctrl   = (uint32_t*)REG_CTRL_ADDR;
    volatile uint32_t* status = (uint32_t*)REG_STATUS_ADDR;

    *ctrl = CTRL_ENABLE | CTRL_STOP;
    while ((*status & STS_DONE) == 0);

    usleep(10);
    *ctrl = CTRL_ENABLE;
    usleep(10);
}

void tram(uint8_t data)
{
    volatile uint32_t* ctrl   = (uint32_t*)REG_CTRL_ADDR;
    volatile uint32_t* status = (uint32_t*)REG_STATUS_ADDR;
    volatile uint32_t* tx     = (uint32_t*)REG_DATA_TX_ADDR;

    *tx = data;
    *ctrl = CTRL_ENABLE | CTRL_WRITE;

    while ((*status & STS_DONE) == 0);

    usleep(10);
    *ctrl = CTRL_ENABLE;
    usleep(10);
}

uint8_t nhan(uint8_t nack)
{
    volatile uint32_t* ctrl   = (uint32_t*)REG_CTRL_ADDR;
    volatile uint32_t* status = (uint32_t*)REG_STATUS_ADDR;
    volatile uint32_t* rx     = (uint32_t*)REG_DATA_RX_ADDR;

    uint32_t cmd = CTRL_ENABLE | CTRL_READ;
    if (nack) cmd |= CTRL_NACK;

    *ctrl = cmd;
    while ((*status & STS_DONE) == 0);

    uint8_t data = (uint8_t)(*rx);

    usleep(10);
    *ctrl = CTRL_ENABLE;
    usleep(10);

    return data;
}
