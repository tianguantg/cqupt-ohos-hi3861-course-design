#include <fingerprint_uart_task.h>
//#include <app_demo_io_gpio.h>
#include <string.h>
#include <stdio.h>
//#include <hilink.h>
#include <time.h>

//#include "alltypes.h"
#include "dbg.h"
#include "fingerprint_module.h"

#define ACK_DATA_LEN
#define UART_READ_OK 1

hi_u32 p_uart_demo_task_id = 0;

void parse_verify_fingerprint_ack(hi_u8 *ptr_uart_buff)
{
    unsigned int score = 0;
    if(ptr_uart_buff[9] == 0x00){
        score |= ptr_uart_buff[13];
        score = score << 8;
        score |= ptr_uart_buff[14];

        if(ptr_uart_buff[10] == 0x05){
            if(score >= 80){
                printf("======= success to match fingerprint ======= \r\n");
                motor_control(3);
                sleep(3);
                motor_control(1);
            }else{
                printf("score too low \r\n");
                return ;
            }
        }
    }else{
        printf("======= failed to match fingerprint =======\r\n");
        return;
    }
}

void parse_login_fingerprint_ack(hi_u8 *ptr_uart_buff)
{
    extern int flag;
    extern int id;
    if(ptr_uart_buff[9] == 0x00){
        if(ptr_uart_buff[10] == 0x06){
            if(ptr_uart_buff[11] == 0xF2){
                flag = 0;
                led_control(4);
                save_login_id(id);
                printf("======= login success ======= \r\n");
            }else{
                flag = 0;
                printf("failed! repeated login \r\n");
            }
        }
    }else{
        printf("login failed \r\n");
    }
}

void parse_empty_fingerprint_ack(hi_u8 *ptr_uart_buff)
{
    if(ptr_uart_buff[9] == 0x00){
        led_control(2);
        deleteid();
        printf("clear success \r\n");
    }else{
        printf("clear failed \r\n");
    }
}

int parse_frame_data(hi_u8 * ptr_uart_buff,unsigned int len)
{
    printf("before  parse ack type:%d \r\n",ptr_uart_buff[8]);

    switch(ptr_uart_buff[8])
    {
        case VERIFY_FINGERPRINT:
            parse_verify_fingerprint_ack(ptr_uart_buff);
            break;

        case LOGIN_FINGERPRINT:
            parse_login_fingerprint_ack(ptr_uart_buff);
            break;

        case EMPTY_FINGERPRINT:
            parse_empty_fingerprint_ack(ptr_uart_buff);
            break;

        default:
            printf("ack buff valied \r\n");
            break;
        
    }
}

hi_s32 uart_read_certain_len_t(hi_u8 * uart_buff,int readLen)
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

static hi_void *fingerprint_uart_demo_task(hi_void *param)
{
    //hi_unref_param(param);
    hi_u8 *uart_buff = NULL;
    
    uart_buff= malloc(UART_BUFF_SIZE); 
    hi_u16 check_sum = 0;

    unsigned short fram_dat_len = 0;
    
    for(;uart_buff == NULL;){
        uart_buff= malloc(UART_BUFF_SIZE);
    }

    printf("Initialize uart demo successfully, please enter some datas via DEMO_UART_NUM port...\n");

    for (;;) {
        hi_uart_read(DEMO_UART_NUM, uart_buff,1);
        printf("uart_buff[0]:%#x \r\n",uart_buff[0]);
        if( uart_buff[0] == 0x55){
            printf("fignerprint module power on! \r\n");
        }else if(uart_buff[0] == 0xEF){
            hi_uart_read(DEMO_UART_NUM, uart_buff+1,1);
            if(uart_buff[1] == 0x01){
                uart_read_certain_len_t(&uart_buff[2],7);
                printf("uart_buff[7]:%#x \r\n",uart_buff[7]);
                fram_dat_len = 0;
                fram_dat_len |= uart_buff[7];
                fram_dat_len = fram_dat_len << 8;
                printf("uart_buff[8]:%#x \r\n",uart_buff[8]);
                fram_dat_len |= uart_buff[8];
                printf("fram_dat_len :%d\r\n",fram_dat_len);

                uart_read_certain_len_t(uart_buff+9,fram_dat_len);
                printf("uart:  ");
                for(int i = 0;i<fram_dat_len+9;i++){
                    printf("%#x  ",uart_buff[i]);
                }
                printf("\r\n");
                parse_frame_data(uart_buff,fram_dat_len+9);
            }
        }else{
            printf("uart read invalid byte \r\n"); 
        }
        fram_dat_len = 0;
        memset(uart_buff,0,UART_BUFF_SIZE);
    }
    printf("uart task finished \r\n");

    free(uart_buff);
    return HI_NULL;
}
/*
 * This demo simply shows how to read datas from UART2 port and then echo back.
 */
hi_void fingerprint_uart_demo(hi_void)
{
    hi_u32 ret;
    hi_uart_attribute uart_attr = {
        .baud_rate = 57600, /* baud_rate: 115200 */
        //.baud_rate = 115200, /* baud_rate: 115200 */
        //.baud_rate = 74880, /* baud_rate: 74880 微盾的专用波特率*/
        .data_bits = 8,      /* data_bits: 8bits */
        .stop_bits = 2,
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
    ret = hi_task_create(&p_uart_demo_task_id, &attr, fingerprint_uart_demo_task, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Falied to create uart demo task!\n");
    }
}
