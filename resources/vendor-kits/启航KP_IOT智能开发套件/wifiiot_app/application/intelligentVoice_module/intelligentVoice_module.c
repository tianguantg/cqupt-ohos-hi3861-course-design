#include <stdio.h>
#include <unistd.h>
#include "hi_timer.h"
#include "hi_gpio.h"
#include "dbg.h"
#include "intelligentVoice_module.h"
#include "intelligentVoice_uart.h"
#include "hi_uart.h"
#include "hi_fs.h"

hi_void intelligentVoice_gpio_io_init(void)
{
    hi_u32 ret;
    
    /*gpio2 led*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_7, HI_IO_FUNC_GPIO_7_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio7 set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_7, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

}   

hi_void intelligentVoice_led_control(int duty)
{

    if(duty == 1){
        hi_gpio_set_ouput_val(HI_GPIO_IDX_7,HI_GPIO_VALUE1);
        printf("======= led on =======\r\n");
    }else{
        hi_gpio_set_ouput_val(HI_GPIO_IDX_7,HI_GPIO_VALUE0);
        printf("======= led off =======\r\n");
    }
}

hi_void intelligentVoice_demo(hi_void)
{
    int ret;
    intelligentVoice_uart_demo();
    intelligentVoice_gpio_io_init();
   
    while(1){
        sleep(3);
        printf("intelligentVoice module \r\n");
    }
}
