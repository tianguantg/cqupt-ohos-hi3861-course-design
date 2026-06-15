#include <gps_uart.h>
#include <hi_types_base.h>
#include <hi_io.h>
#include <hi_early_debug.h>
#include <hi_gpio.h>
#include <hi_task.h>
#include "gps_data_process.h"

hi_u32 g_gps_uart_demo_task_id = 0;
hi_u32 g_read_task_id = 0;

#define MAX_GPS_DATA_LEN  196


static hi_void *gps_uart_demo_task(hi_void *param)
{
    static hi_u32 read_pos = 0;
    hi_u8 first_uart_buff[UART_BUFF_SIZE] = {0};
    hi_u8 second_uart_buff[UART_BUFF_SIZE] = {0};
    hi_u8 *uart_buff_ptr = first_uart_buff;
    hi_unref_param(param);
    printf("Initialize uart demo successfully, please enter some datas via DEMO_UART_NUM port...\n");

    for (;;) {
        //hi_u32 len = hi_uart_read(DEMO_UART_NUM, uart_buff_ptr, UART_BUFF_SIZE);
        hi_u32 len = hi_uart_read(DEMO_UART_NUM, uart_buff_ptr + read_pos, 1); //read onebyte
        if (len > 0) {

        } else {
            printf("Read nothing!\n");
            hi_sleep(1000); /* sleep 1000ms */
        }
            process_gps_data(uart_buff_ptr,&read_pos);
    }

    hi_task_delete(g_gps_uart_demo_task_id);
    g_gps_uart_demo_task_id = 0;

    return HI_NULL;
}

/*   */
static hi_void *gps_flash_read_task(hi_void *param)
{
    (void) param;
    int ret = 0;
    static int gpio_val = 0;
    static unsigned int gpio_read_cunt = 0;

    ret = hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_10, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }

    for(;;){

        ret = hi_gpio_get_input_val(HI_GPIO_IDX_10, &gpio_val);
        if (ret != HI_ERR_SUCCESS) {
            printf("===== ERROR ===== gpio -> hi_gpio_get_input_val ret:%d\r\n", ret);
            return;
        }

        if(gpio_val == HI_GPIO_VALUE1 && gpio_read_cunt >= 5){
            
            hi_mux_pend(g_gps_mutex,HI_SYS_WAIT_FOREVER);
            if(g_gps_uart_demo_task_id != 0){
                hi_task_delete(g_gps_uart_demo_task_id);
                g_gps_uart_demo_task_id = 0;
            }
            gps_data_read_flash();
            hi_mux_post(g_gps_mutex);
            return  HI_NULL;
        }else if(gpio_val == HI_GPIO_VALUE1){
            gpio_read_cunt++;
            printf("read gpio val 1,gpio_read_cunt :%d \r\n",gpio_read_cunt);
        }else{
            gpio_read_cunt = 0; 
        }

        hi_sleep(2000);
    }
}

hi_u32 g_gps_mutex = 0;
/*
 * This demo simply shows how to read datas from UART2 port and then echo back.
 */
hi_void gps_uart_demo(hi_void)
{
    hi_u32 ret;
    hi_uart_attribute uart_attr = {
        .baud_rate = 9600, /* baud_rate: 115200 */
        .data_bits = 8,      /* data_bits: 8bits */
        .stop_bits = 1,
        .parity = 0,
    };

    /* Initialize uart driver */
    ret = hi_uart_init(DEMO_UART_NUM, &uart_attr, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Failed to init uart! Err code = %d\n", ret);
        return;
    }
    ret = hi_mux_create(&g_gps_mutex);
    if(ret != 0){
        printf("hi_mux_create failed \r\n");
        return ;
    }
    /* Create a task to handle uart communication */
    hi_task_attr attr = {0};
    attr.stack_size = UART_DEMO_TASK_STAK_SIZE;
    attr.task_prio = UART_DEMO_TASK_PRIORITY;
    attr.task_name = (hi_char*)"uart_demo";
    ret = hi_task_create(&g_gps_uart_demo_task_id, &attr, gps_uart_demo_task, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Falied to create uart demo task!\n");
    }

    hi_task_attr read_task_attr = {0};
    ret = hi_task_create(&g_read_task_id, &attr, gps_flash_read_task, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Falied to create uart demo task!\n");
    }
}

