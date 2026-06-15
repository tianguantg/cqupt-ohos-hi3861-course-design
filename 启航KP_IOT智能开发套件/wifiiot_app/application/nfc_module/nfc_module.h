#ifndef __NFC_MODULE_H__
#define __NFC_MODULE_H__

#include <hi_types_base.h>
#include <hi_i2c.h>
#include <hi_early_debug.h>
#include <hi_stdlib.h>

#define I2C_REG_ARRAY_LEN 64
#define ES8311_DEV_ADDR 0x30          /* 11000 0 */
#define ES8311_REG_ADDR 0x10
#define I2C_SEND_LEN_2  2
#define I2C_RECV_LEN_1  1


hi_void NFCTask(hi_void);
#endif 
