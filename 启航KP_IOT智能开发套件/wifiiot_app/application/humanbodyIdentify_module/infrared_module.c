#include <stdio.h>
#include "hi_timer.h"
#include "hi_gpio.h"
#include "hi_pwm.h"
#include "dbg.h"
#include "infrared_module.h"
#include "infrared_adc.h"

#define PWM_CLK_FREQ 160000000 
#define freq 2442  //min

hi_void infrared_gpio_io_init(void)
{
    hi_u32 ret;
    /*gpio2 人体感应模块led*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_2, HI_IO_FUNC_GPIO_2_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio8 set body induction led func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_2, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");
}

hi_void infrared_led_ctrl(unsigned int state)
{  
    if(state == 1){
        printf("----- infrared test led turn on -----\r\n");
        hi_gpio_set_ouput_val(HI_GPIO_IDX_2,HI_GPIO_VALUE1);
    }else{
        printf("----- infrared test led turn off -----\r\n");
        hi_gpio_set_ouput_val(HI_GPIO_IDX_2,HI_GPIO_VALUE0);
    }
}

hi_void infrared_adc_demo(hi_void)
{
    infrared_gpio_io_init();
    
    while(1){
        infrared_adc_gather();
    }
}

