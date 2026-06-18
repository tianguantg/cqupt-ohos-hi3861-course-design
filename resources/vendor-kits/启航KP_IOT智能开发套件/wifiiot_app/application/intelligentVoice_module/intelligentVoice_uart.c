#include <intelligentVoice_uart.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "dbg.h"
#include "intelligentVoice_module.h"

#define ACK_DATA_LEN
#define UART_READ_OK 1

hi_u32 v_uart_demo_task_id = 0;

hi_s32 uart_read_certain_len_v(hi_u8 * uart_buff,int readLen)
{
    hi_u8 *uart_buff_ptr = uart_buff;
    hi_s32 len = 0;
    hi_s32 count = 0;

    if(uart_buff_ptr == NULL || readLen == 0){
        printf("invalid param \r\n"); 
    }

    DBG("ready to read readLen :%d \r\n",readLen); 
    for( ;count < readLen; count += len ){
        len = hi_uart_read(DEMO_UART_NUM, uart_buff_ptr+count,(readLen - count)); 
        if(len){
            continue; 
        }else{
            printf("Read nothing!\r\n");
            hi_sleep(1000); 
        }
        DBG("hi_uart_read len: %d \r\n",len);
    }
    DBG("after read count :%d \r\n",count);
    return UART_READ_OK;
}

static hi_void *intelligentVoice_uart_demo_task(hi_void *param)
{
    hi_u8 *uart_buff = NULL;
    
    uart_buff= malloc(UART_BUFF_SIZE); 

    for(;uart_buff == NULL;){
        uart_buff= malloc(UART_BUFF_SIZE);
    }

    printf("Initialize uart demo successfully, please enter some datas via DEMO_UART_NUM port...\n");

    for (;;) {
        hi_uart_read(DEMO_UART_NUM, uart_buff,1);
        if( uart_buff[0] == 0xAA){
            uart_read_certain_len_v(&uart_buff[1],1);
            if(uart_buff[1] == 0x04){
                uart_read_certain_len_v(&uart_buff[2],1);
               if(uart_buff[2] == 0x00){
                    intelligentVoice_led_control(1);
               }else if(uart_buff[2] == 0x01){
                    intelligentVoice_led_control(0);
               }
            }else{
                printf("voice type invalid \r\n");
            }
        }else{
            printf("uart read invalid byte \r\n");
        }      
            memset(uart_buff,0,UART_BUFF_SIZE);
    }
    printf("uart task finished \r\n");

    free(uart_buff);
    return HI_NULL;
}


hi_void intelligentVoice_uart_demo(hi_void)
{
    hi_u32 ret;
    hi_uart_attribute uart_attr = {
        .baud_rate = 9600, 
        .data_bits = 8, 
        .stop_bits = 1,
        .parity = 0,
    };

    /* Initialize uart driver */
    ret = hi_uart_init(DEMO_UART_NUM, &uart_attr, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Failed to init uart! Err code = %d\n", ret);
        return;
    }

    /* Create a task to handle uart communication */
    hi_task_attr attr = {0};
    attr.stack_size = UART_DEMO_TASK_STAK_SIZE;
    attr.task_prio = UART_DEMO_TASK_PRIORITY;
    attr.task_name = (hi_char*)"uart_demo";
    ret = hi_task_create(&v_uart_demo_task_id, &attr, intelligentVoice_uart_demo_task, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Falied to create uart demo task!\n");
    }
}
