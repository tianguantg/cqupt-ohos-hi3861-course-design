#include <stdio.h>
#include "nfc_module.h"
#include <string.h>
#include <unistd.h>
#include "nfc.h"
#define I2C_TASK_STACK_SIZE 1024 * 8
#define I2C_TASK_PRIO 25

#define TEXT "Welcome to isfotstone"
//#define TEXT "D2025A68772001004800323543370081090037200685031417910306E4357A9AD6D6141145343A33353A37413A39413A44363A44361701099117E4357A9AD6D645343A33353A37413A39413A44363A44368B0C216546215648561A3165416A"
//#define WEB "harmonyos.com"

hi_void NFCTask(void)
{
    uint8_t ret;

    //baudrate: 400kbps
    //i2c_demo_send_data_init();
    hi_i2c_init(HI_I2C_IDX_0, 400000); /* baudrate: 100000 */

    printf("I2C Test Start\n");
#if 1
    
    ret = storeText(NDEFFirstPos, (uint8_t *)TEXT);
    if (ret != 1)
    {
        printf("NFC Write Data Falied :%d ", ret);
    }
#else if
    ret = storeUrihttp(NDEFLastPos, (uint8_t *)WEB);
    if (ret != 1)
    {
        printf("NFC Write Data Falied :%d ", ret);
    }
#endif
    while (1)
    {
        printf("=======================================\r\n");
        printf("***********I2C_NFC_example**********\r\n");
        printf("=======================================\r\n");
        printf("Please use the mobile phone with NFC function close to the development board!\r\n");
        usleep(1000000);
    }
}


