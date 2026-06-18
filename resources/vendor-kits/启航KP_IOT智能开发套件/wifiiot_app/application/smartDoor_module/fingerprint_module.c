#include <stdio.h>
#include <unistd.h>
#include "hi_timer.h"
#include "hi_gpio.h"
#include "hi_pwm.h"
#include "dbg.h"
#include "fingerprint_module.h"
#include "fingerprint_uart_task.h"
#include "hi_uart.h"
#include "hi_fs.h"

#define LOGIN_BUFF_LEN 17

int flag = 0;
int id = 0;

hi_u8 verify_buff[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x32,0x03,0xFF,0xFF,0x00,0x07,0x02,0x43}; //自动验证指令
hi_u8 empty_buff[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11}; //清空指纹库指令

static unsigned int g_MonitorTask;
const hi_task_attr MonitorTaskAttr_t = {
    .task_prio = 25,
    .stack_size = 4096,
    .task_name = "MonitorTask",
};

void *MonitorVerifyTask(void * para) /* 验证 */
{
    while(1){
        verify_gpio_getval();
    }
    return NULL;
}

void *MonitorLoginTask(void * para) /* 注册 */
{
    while(1){
        login_gpio_getval();
    }
    return NULL;
}

hi_void fingerprint_gpio_io_init(void)
{
    hi_u32 ret;
    
    /*gpio2 验证指纹触发信号*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_2, HI_IO_FUNC_GPIO_2_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio2 set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_2, HI_GPIO_DIR_IN);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

#if 1
    /*gpio7 舵机控制*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_7, HI_IO_FUNC_GPIO_7_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_7, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
#endif
    /*gpio13 led*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_13, HI_IO_FUNC_GPIO_13_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_13, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    
    /*gpio14 注册指纹触发信号*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_14, HI_IO_FUNC_GPIO_14_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_14, HI_GPIO_DIR_IN);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
}   

hi_void motor_control(int duty)
{
    int ret;
    int count = 0;
    int tm;

    hi_gpio_set_ouput_val(HI_GPIO_IDX_7,HI_GPIO_VALUE0);

    while(count <= 50){
        if(duty == 1){
            hi_gpio_set_ouput_val(HI_GPIO_IDX_7,HI_GPIO_VALUE1);
            hi_udelay(1000);
            hi_gpio_set_ouput_val(HI_GPIO_IDX_7,HI_GPIO_VALUE0);
            hi_udelay(19000);
            printf("count1:%d \r\n",count);
        }else if(duty == 2){
            hi_gpio_set_ouput_val(HI_GPIO_IDX_7,HI_GPIO_VALUE1);
            hi_udelay(1500);
            hi_gpio_set_ouput_val(HI_GPIO_IDX_7,HI_GPIO_VALUE0);
            hi_udelay(18500);
            printf("count2:%d \r\n",count);
        }else if(duty == 3){
            hi_gpio_set_ouput_val(HI_GPIO_IDX_7,HI_GPIO_VALUE1);
            hi_udelay(2000);
            hi_gpio_set_ouput_val(HI_GPIO_IDX_7,HI_GPIO_VALUE0);
            hi_udelay(18000);
            printf("count3:%d \r\n",count);
        }else{
            printf("duty error \r\n");
        }
        count++;
    }
    
}

hi_void led_control(int duty)
{
    int ret;

        if(duty == 1){
            hi_gpio_set_ouput_val(HI_GPIO_IDX_13,HI_GPIO_VALUE1);
            sleep(2);
            hi_gpio_set_ouput_val(HI_GPIO_IDX_13,HI_GPIO_VALUE0);
            printf("login success !!!!! \r\n");
        }else if(duty == 2){
            hi_gpio_set_ouput_val(HI_GPIO_IDX_13,HI_GPIO_VALUE1);
            usleep(500000);
            hi_gpio_set_ouput_val(HI_GPIO_IDX_13,HI_GPIO_VALUE0);
            usleep(500000);
            hi_gpio_set_ouput_val(HI_GPIO_IDX_13,HI_GPIO_VALUE1);
            usleep(500000);
            hi_gpio_set_ouput_val(HI_GPIO_IDX_13,HI_GPIO_VALUE0);
            printf("clear success !!!!! \r\n");
        }else if(duty == 3){
            hi_gpio_set_ouput_val(HI_GPIO_IDX_13,HI_GPIO_VALUE1);
        }else if(duty == 4){
            hi_gpio_set_ouput_val(HI_GPIO_IDX_13,HI_GPIO_VALUE0);
        }else{
            printf("duty error \r\n");
        }
}

hi_u16 get_check_sum(unsigned char * ptr_ack_buff,\
    hi_u8 * ptr_check_sum,unsigned int len)
{
    int i = 6;
    hi_u16 check_sum = 0;

    DBG("get check_sum \r\n");
    for( ; i < len-2; i++){
        check_sum += ptr_ack_buff[i];
    }
    
    if(ptr_check_sum != NULL){
        ptr_check_sum[2] = (check_sum & 0xff00) >> 8;
        ptr_check_sum[3] = check_sum & 0x00ff ;
    }

    return check_sum;
}

void construct_frame_head(unsigned char * ptr_buff)
{ 
    frame_head_t * ptr_head = (frame_head_t *) ptr_buff;
   
    DBG("construct login frame head \r\n");
    
    ptr_head->start[0] = 0xEF;
    ptr_head->start[1] = 0x01;
    ptr_head->addr[0] = 0xFF;
    ptr_head->addr[1] = 0xFF;
    ptr_head->addr[2] = 0xFF;
    ptr_head->addr[3] = 0xFF;
    ptr_head->inden = 0x01;
    ptr_head->data_len[0] = 0x00;
    ptr_head->data_len[1] = 0x08;
    ptr_head->frame_type = 0x31;
    printf("construct frame head successfully \r\n");
}

void construct_frame_tail(unsigned char *ptr_ack_buff,\
    unsigned char * ptr_ack_buff_tail,unsigned int len)
{ 
    frame_tail_t * ptr_tail = (frame_tail_t *) ptr_ack_buff_tail;
    ptr_tail->param[0] = 0x00;
    ptr_tail->param[1] = 0x7A;
    get_check_sum(ptr_ack_buff,ptr_ack_buff_tail,len);
    DBG("construct ack frame tail \r\n");
}

void save_login_id(int id_t)
{ 
    int fd = 0;
    int ret = 0;
    char buff[1] = {0};
    if((fd = hi_open("idsave",HI_FS_O_CREAT|HI_FS_O_TRUNC|HI_FS_O_RDWR))<= 0){
        printf("hi_open failed [fd :%d]\r\n",fd); 
        return -1;
    }

    printf("save id :%d \r\n",id_t);
    buff[0] = id_t;
    if(hi_write(fd,buff,1) < 0){
        printf("write id failed [fd :%d]\r\n",fd); 
        return -1;
    }

    ret = hi_close(fd);
    if(ret < 0){
        printf("hi_close failed \r\n");
    } 

}

int get_login_id(hi_void)
{ 
    int fd = 0;
    int ret = 0;
    char buff[1] = {0};
    //if((fd = hi_open("idsave",HI_FS_O_CREAT|HI_FS_O_TRUNC|HI_FS_O_RDWR))<= 0){
    if((fd = hi_open("idsave",HI_FS_O_RDWR))<= 0){
        printf("read idsave hi_open failed [fd :%d]\r\n",fd); 
        return -1;
    }

    ret = hi_read(fd,buff,1);
    if(ret < 0){
        printf("read id failed \r\n");
    }

    printf("read id :%d \r\n",buff[0]);

    return buff[0];
}

int deleteid(void)
{
    //delete id file
    if(hi_unlink("idsave") < 0){
        printf("hi_unlink failed \r\n");
        return -1;
    }
    
    return 0;
}

hi_void login_gpio_getval(hi_void)
{
    hi_u32 ret;
    hi_u8 login_buff[17] = {0};
    int login_flag = 0;
    int delete_flag = 0;
    int rd_id;
    login_cmd_t *ptr = (login_cmd_t *) login_buff;

    hi_gpio_value login_gpio_val = HI_GPIO_VALUE1;
 
    ret = hi_gpio_get_input_val(HI_GPIO_IDX_14, &login_gpio_val);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_get_input_val ret:%d\r\n", ret);
        return;
    }

    if(login_gpio_val == 0){
        login_flag = 1;
        sleep(3);
        ret = hi_gpio_get_input_val(HI_GPIO_IDX_14, &login_gpio_val);
        if (ret != HI_ERR_SUCCESS) {
            printf("===== ERROR ===== gpio -> hi_gpio_get_input_val ret:%d\r\n", ret);
            return;
        }
        if(login_gpio_val == 0){
            delete_flag = 1;
            login_flag = 0;
        }

    }

    if(login_flag  == 1){
        flag = 1;
        construct_frame_head(login_buff);       
        id = get_login_id() + 1;
        
        printf("login id :%d \r\n",id);

        ptr->login_id[0] = 0;
        ptr->login_id[1] = id;

        ptr->times = 0x02;
        construct_frame_tail(login_buff,&ptr->frame_tail,LOGIN_BUFF_LEN);
        hi_uart_write_immediately(DEMO_UART_NUM,login_buff,sizeof(login_buff));
        printf("login_buff:");
        for(int i = 0;i < 17;i++){
            printf("%x  ",login_buff[i]);
        }
        printf(" \r\n");
        login_flag = 0;
        led_control(3);
        printf("login ++++++++++\r\n");
    }

    if(delete_flag == 1){
        hi_uart_write_immediately(DEMO_UART_NUM,empty_buff,sizeof(empty_buff));
        delete_flag = 0;
        printf("delete +++++++++++\r\n");
    }
        
}

hi_void verify_gpio_getval(hi_void)
{
    hi_u32 ret;

    hi_gpio_value verify_gpio_val = HI_GPIO_VALUE0;
 
    ret = hi_gpio_get_input_val(HI_GPIO_IDX_2, &verify_gpio_val);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_get_input_val ret:%d\r\n", ret);
        return;
    }

    if(verify_gpio_val == 1){
        sleep(1);
        if(verify_gpio_val == 1 && flag == 0)
        {
            printf("start verify \r\n");
            ret = hi_uart_write_immediately(DEMO_UART_NUM,verify_buff,sizeof(verify_buff));
            if(ret < 0){
                printf("send verify buff failed,ret : %d \r\n",ret);
            }else{
                printf("send verify buff successfully,ret :%d \r\n",ret);
            }
        }
    }
}

hi_void fingerprint_demo(hi_void)
{
    int ret;
    fingerprint_uart_demo();
    fingerprint_gpio_io_init();

    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr_t,
        MonitorVerifyTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor verify task failed [%d]\r\n", ret);
        return;
    }
    
    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr_t,
        MonitorLoginTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor login task failed [%d]\r\n", ret);
        return;
    }
   
}
