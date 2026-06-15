#include <stdio.h>
#include <unistd.h>
#include "hi_timer.h"
#include "hi_gpio.h"
#include "hi_pwm.h"
#include "dbg.h"
#include "heart_rate_module.h"
#include "MAX30102.h"
#include "algorithm.h"

static unsigned int g_MonitorTask;
const hi_task_attr MonitorTaskAttr_h = {
    .task_prio = 20,
    .stack_size = 4096,
    .task_name = "MonitorTask",
};
const hi_task_attr MonitorTaskAttr_d = {
    .task_prio = 20,
    .stack_size = 4096,
    .task_name = "DisplayTask",
};
void *MonitorHRTask(void * para) /* HRtask处理函数 */
{
    while(1){
        test_main();
    }
    return NULL;
}

void *MonitorDisplayTask(void * para) /* OLEDtask处理函数 */
{
    while(1){
        display_hr();
    }
    return NULL;
}
hi_void heart_rate_gpio_init(hi_void)
{
    hi_u32 ret;
    
    /*gpio2心率检测模块中断输入*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_2, HI_IO_FUNC_GPIO_2_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio2  set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_2, HI_GPIO_DIR_IN);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio2 set dir success! -----\r\n");
    
    ret = hi_io_set_func(HI_IO_NAME_GPIO_8, HI_IO_FUNC_GPIO_8_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_8, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

    ret = hi_io_set_func(HI_IO_NAME_GPIO_11, HI_IO_FUNC_GPIO_11_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_11, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

    ret = hi_io_set_func(HI_IO_NAME_GPIO_12, HI_IO_FUNC_GPIO_12_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_12, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

}

int hr_int(hi_void)
{       
    int ret = 0;
    hi_gpio_value gpio2_val = HI_GPIO_VALUE0;
    ret = hi_gpio_get_input_val(HI_GPIO_IDX_2, &gpio2_val);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_get_input_val ret:%d\r\n", ret);
        return 0;
    }
   return gpio2_val; 
}

hi_void heart_rate_detection_demo(hi_void)
{    
    int ret;

    heart_rate_gpio_init();
    hi_spi_deinit(HI_SPI_ID_0);
    screen_spi_master_init(0);

    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr_h,
        MonitorHRTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor hear_rate_detection task failed [%d]\r\n", ret);
        return;
    }
    
    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr_d,
        MonitorDisplayTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor oled task failed [%d]\r\n", ret);
        return;
    }
    return;
}
