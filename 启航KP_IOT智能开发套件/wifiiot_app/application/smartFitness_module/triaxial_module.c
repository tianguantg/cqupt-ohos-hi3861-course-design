#include <stdio.h>
#include "triaxial_module.h"

#define PWM_CLK_FREQ 160000000 
#define freq 2442  //min

static unsigned int g_MonitorTask;
const hi_task_attr MonitorTaskAttr_tr = {
    .task_prio = 20,
    .stack_size = 4096,
    .task_name = "BuggyNetworkMonitorTask",
};

void *MonitorTriaxialTask(void * para) /* triaxialtask处理函数 */
{
    while(1){
        usleep(30000);
        Read_XYZ();
    }
    return NULL;
}
void triaxial_gpio_init(hi_void)
{
    int ret;
    ret = hi_io_set_func(HI_IO_NAME_GPIO_5, HI_IO_FUNC_GPIO_5_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio5 fan set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_5, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n"); 
}

hi_void triaxial_pwm_init(hi_void)
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

hi_void triaxial_pwm_start(unsigned int duty)
{
    int ret = 0;  

    if(duty == 0){
        hi_gpio_set_ouput_val(HI_GPIO_IDX_5,HI_GPIO_VALUE0);
        ret = hi_pwm_stop(HI_PWM_PORT_PWM2); 
        if(ret != 0){ 
            printf("hi_pwm_start failed ret : %#x \r\n",ret); 
        }
    }else{
        printf("buzzer start \r\n");
        hi_gpio_set_ouput_val(HI_GPIO_IDX_5,HI_GPIO_VALUE1);
        ret = hi_pwm_start(HI_PWM_PORT_PWM2, duty*(PWM_CLK_FREQ/freq)/100, PWM_CLK_FREQ/freq); 
        if(ret != 0){ 
            printf("hi_pwm_start failed ret : %#x \r\n",ret); 
        }
    }
}

hi_void triaxial_demo(hi_void)
{
    int ret;
    triaxail_i2c_init();
    triaxial_gpio_init();
    triaxial_pwm_init();
    
    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr_tr,
        MonitorTriaxialTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor oled task failed [%d]\r\n", ret);
        return;
    }

}
