#include <stdio.h>
#include <unistd.h>
#include "hi_timer.h"
#include "hi_gpio.h"
#include "hi_pwm.h"
#include "dbg.h"
#include "motor_module.h"
#include "hi_spi.h"

#define PWM_CLK_FREQ 160000000 
#define freq 2442  //min


static unsigned int g_MonitorTask;
const hi_task_attr MonitorTaskAttr = {
    .task_prio = 20,
    .stack_size = 4096,
    .task_name = "BuggyNetworkMonitorTask",
};

void *MonitorOledTask(void * para) /* OLEDtask处理函数 */
{
    while(1){
        test_led_screen();
        printf("OLED task \r\n");
    }
    return NULL;
}

void *MonitorMotorTask(void * para) /* 电机task处理函数 */
{
    while(1){
        gpio_getval();
        infrared_ctrl(); 
        //printf("fan task \r\n");
    }
    return NULL;
}

void *MonitorShtTask(void * para) /* 温湿度传感器处理函数 */
{
    while(1){
        sleep(2);
        SHT3X_ReadMeasurementVal(0);
        printf("sth task \r\n");
    }
    return NULL;
}

hi_void motor_gpio_io_init(void)
{
    hi_u32 ret;

    /*gpio5按键控制电机速度*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_5, HI_IO_FUNC_GPIO_5_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio5 fan set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_5, HI_GPIO_DIR_IN);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

    /*gpio6电机模块led*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_6, HI_IO_FUNC_GPIO_6_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_6, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    
    /*gpio7 电机模块红外传感*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_7, HI_IO_FUNC_GPIO_7_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_7, HI_GPIO_DIR_IN);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

    /*
    ret = hi_io_set_func(HI_IO_NAME_GPIO_11, HI_IO_FUNC_GPIO_11_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");
    */
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

hi_void motor_pwm_init(hi_void)
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

hi_void motor_pwm_start(unsigned int duty)
{
    int ret = 0;  
    DBG("motor start \r\n");

    if(duty == 0){
        ret = hi_pwm_stop(HI_PWM_PORT_PWM2); 
        if(ret != 0){ 
            printf("hi_pwm_start failed ret : %#x \r\n",ret); 
        }
    }

    ret = hi_pwm_start(HI_PWM_PORT_PWM2, duty*(PWM_CLK_FREQ/freq)/100, PWM_CLK_FREQ/freq); 
    if(ret != 0){ 
        printf("hi_pwm_start failed ret : %#x \r\n",ret); 
    }
}

int infrared_ctrl(hi_void)
{
    hi_u32 ret;
    int temp = 0;
    hi_gpio_value gpio_val_7 = HI_GPIO_VALUE0;

    ret = hi_gpio_get_input_val(HI_GPIO_IDX_7, &gpio_val_7);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_get_input_val ret:%d\r\n", ret);
        return 0;
    }

    //printf("----- gpio input val is:%d. -----\r\n", gpio_val_7);

    if(gpio_val_7 == 1){
        hi_gpio_set_ouput_val(HI_GPIO_IDX_6,HI_GPIO_VALUE1);
        ret = hi_pwm_stop(HI_PWM_PORT_PWM2);
        if(ret != 0){ 
            printf("hi_pwm_start failed ret : %#x \r\n",ret); 
        }
        temp = 1;
    }else{
        hi_gpio_set_ouput_val(HI_GPIO_IDX_6,HI_GPIO_VALUE0);
    }
    return temp;
}

hi_void gpio_getval(hi_void)
{
    hi_u32 ret;
    int temp;
    static int key = 0;
    hi_gpio_value gpio_val_1 = HI_GPIO_VALUE1;
 
    temp = infrared_ctrl();
    ret = hi_gpio_get_input_val(HI_GPIO_IDX_5, &gpio_val_1);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_get_input_val ret:%d\r\n", ret);
        return;
    }

    //printf("----- gpio input val is:%d. -----\r\n", gpio_val_1);

    if(gpio_val_1 == 0){
        sleep(1);
        if(gpio_val_1 == 0){
            key++;
        }

        switch(key){
        case 0:
            break;

        case 1:
            motor_pwm_start(1);
            break;

        case 2:
            motor_pwm_start(2);
            break;

        case 3:
            motor_pwm_start(3);
            break;

        default:
            printf("invalid mode \r\n");
        }

        if(key >= 4 || key == 0 || temp == 1){
            key = 0;
            ret = hi_pwm_stop(HI_PWM_PORT_PWM2);
            if(ret != 0){
                printf("hi_pwm_stop failed \r\n");
            }
        }
    }
    //printf("key : %d \r\n",key);


}

hi_void motor_demo(hi_void)
{    
    int ret;
    motor_gpio_io_init();
    //SHT3X_ReadSerialNumber();
    SHT3X_init();
    motor_pwm_init();

    hi_spi_deinit(HI_SPI_ID_0);
    screen_spi_master_init(0);

    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr,
        MonitorOledTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor oled task failed [%d]\r\n", ret);
        return;
    }
   
    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr,
        MonitorMotorTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor motor task failed [%d]\r\n", ret);
        return;
    }   
    
    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr,
        MonitorShtTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor motor task failed [%d]\r\n", ret);
        return;
    }

    return;
}
