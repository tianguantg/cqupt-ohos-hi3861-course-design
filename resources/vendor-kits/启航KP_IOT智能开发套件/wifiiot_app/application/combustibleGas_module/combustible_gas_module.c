#include <stdio.h>
#include "hi_timer.h"
#include "hi_gpio.h"
#include "hi_pwm.h"
#include "dbg.h"
#include "combustible_gas_module.h"
#include "combustible_gas_adc.h"

#define PWM_CLK_FREQ 160000000 
#define freq 2442  //min


static unsigned int g_MonitorTask;
static const hi_task_attr MonitorTaskAttr = {
    .task_prio = 20,
    .stack_size = 4096,
    .task_name = "gas_alarm_task",
};

void *MonitorTask_gas(void * para) /* OLEDtask处理函数 */
{
    while(1){
        gas_adc_gather();
        printf("gas monitor task \r\n");
    }
    return NULL;
}

hi_void gas_gpio_io_init(void)
{
    hi_u32 ret;
    
    /*gpio5烟雾报警灯*/

    ret = hi_io_set_func(HI_IO_NAME_GPIO_5, HI_IO_FUNC_GPIO_5_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio5 gas set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_5, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");
    
}

hi_void gas_pwm_init(hi_void)
{
    int ret = -1;
    ret = hi_pwm_deinit(HI_PWM_PORT_PWM2);
    if(ret != 0){ 
        printf("hi_pwm_deinit failed :%#x \r\n",ret); 
    }

    ret = hi_pwm_init(HI_PWM_PORT_PWM2);
    if(ret != 0){ 
        printf("hi_pwm_init failed :%#x \r\n",ret); 
    }

    ret = hi_pwm_set_clock(PWM_CLK_160M);
    if(ret != 0){ 
        printf("hi_pwm_set_clock failed ret : %#x \r\n",ret); 
    }
}

hi_void gas_led_ctrl(unsigned int state)
{  
    if(state == 1){
        printf("----- gas alarm led turn on -----\r\n");
        hi_gpio_set_ouput_val(HI_GPIO_IDX_5,HI_GPIO_VALUE1);
    }else{
        printf("----- gas alarm led turn off -----\r\n");
        hi_gpio_set_ouput_val(HI_GPIO_IDX_5,HI_GPIO_VALUE0);
    }
}

hi_void gas_pwm_start(unsigned int duty)
{
    int ret = 0;  
    DBG("motor start \r\n");

    if(duty == 0){
        gas_led_ctrl(0);
        ret = hi_pwm_stop(HI_PWM_PORT_PWM2); 
        if(ret != 0){ 
            printf("hi_pwm_start failed ret : %#x \r\n",ret); 
        }
    }else{
        gas_led_ctrl(1);
        ret = hi_pwm_start(HI_PWM_PORT_PWM2, duty*(PWM_CLK_FREQ/freq)/100, PWM_CLK_FREQ/freq); 
        if(ret != 0){ 
            printf("hi_pwm_start failed ret : %#x \r\n",ret); 
        }
    }
}
 
hi_void combustible_gas_demo(hi_void)
{   
    int ret;

    gas_gpio_io_init();
    gas_pwm_init();
    
    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr,
        MonitorTask_gas, // task处理函数 //
        NULL); // task处理函数参数 //
}

