#ifndef __I2C_H_
#define __I2C_H_

#include <stdint.h>
#include "xil_types.h"
#include "xparameters.h"
#include "sleep.h"

/* ================= BASE ADDRESS ================= */
#define I2C_BASE_ADDR    0x44A00000

#define REG_CTRL_ADDR    (I2C_BASE_ADDR + 0x00)
#define REG_STATUS_ADDR  (I2C_BASE_ADDR + 0x04)
#define REG_DATA_TX_ADDR (I2C_BASE_ADDR + 0x08)
#define REG_DATA_RX_ADDR (I2C_BASE_ADDR + 0x0C)

/* ================= CTRL BITS ================= */
#define CTRL_ENABLE   (1 << 0)
#define CTRL_START    (1 << 1)
#define CTRL_STOP     (1 << 2)
#define CTRL_WRITE    (1 << 3)
#define CTRL_READ     (1 << 4)
#define CTRL_NACK     (1 << 5)

/* ================= STATUS BITS ================= */
#define STS_BUSY      (1 << 0)
#define STS_DONE      (1 << 1)
#define STS_RX_ACK    (1 << 2)

/* ================= API ================= */
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void tram(uint8_t data);
uint8_t nhan(uint8_t nack);

#endif
