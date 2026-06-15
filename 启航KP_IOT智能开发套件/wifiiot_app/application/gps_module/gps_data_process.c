#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <hi_flash.h>
#include "hi_uart.h"
#include "gps_uart.h"
#include "hi_types_base.h"
#include "hi_partition_table.h"

#include "gps_data_process.h"
#include <gps_data_store_queue.c>
/*
void __attribute__((format(printf,1,2))) log_printf(const char * fmt,...)
{
    va_list args;
    va_start(args,fmt);

#ifdef SELF_DBG 
    printf("FILE:%s,FUNC:%s,LINE:%d\r\n",__FILE__,__func__,__LINE__);
#endif

    vprintf(fmt,args);

    va_end(args);
}
*/

/* */
int gps_data_read_flash()
{
    int ret = 0;
    char temp[MAX_GPS_DATA_LOCATION_LEN] = {0};
    gps_data_info_t gps_flash_info;
    gps_data_info_t * flash_info = &gps_flash_info;

    ret = hi_get_usr_partition_table(&g_usr_gps_partition_addr,&g_usr_gps_partition_size);
    if(ret != 0){
        printf("get usr partition table failed \r\n"); 
        return GPS_DATA_ERR; 
    }

    get_gps_flash_info(&gps_flash_info,sizeof(gps_flash_info));

    printf("reading gps data\r\n");
    for(;!is_GPS_QUEUE_EMPTY();){ 
        GPS_DEQUEUE(temp);
        printf("%s\r\n",temp);
    }
    
    return save_gps_flash_info(&gps_flash_info);
}

int gps_data_write_flash(gps_data_info_t * flash_info,hi_u8 * location_start,hi_u32 data_size)
{
    int ret = 0;
    char temp[MAX_GPS_DATA_LOCATION_LEN] = {0};

    if(data_size > MAX_GPS_DATA_LOCATION_LEN){
        printf("data_size overflow\r\n");
        return -1;
    }

    printf("addr of start : %d,write : %d,read : %d,size : %d \r\n",
        flash_info->start_addr,flash_info->write_addr,flash_info->read_addr,flash_info->partition_size);

    location_start[data_size+1] = 0;
    printf("gps location data : %s \r\n",location_start);

    strcpy(temp,location_start);

    /* write gps data */
    GPS_ENQUEUE(temp,data_size);

    hi_sleep(100);
    /* write gps_data_info */
    ret = save_gps_flash_info(flash_info);
    if(ret != 0){
        printf("save_gps_flash_info falied %#x \r\n",ret);
        return GPS_DATA_ERR;
    }
   return  GPS_DATA_OK;
}

int gps_data_save_flash(hi_u32 usr_partition_addr,hi_u32 partition_size,hi_u8 * location_start,hi_u32 data_size)
{
    static gps_data_info_t gps_flash_info;
    if(data_size <= 0){
        printf("invalid para data_size\r\n");
        return GPS_DATA_ERR;
    }
    
    get_gps_flash_info(&gps_flash_info,sizeof(gps_flash_info));

    hi_mux_pend(g_gps_mutex,HI_SYS_WAIT_FOREVER);
    gps_data_write_flash(&gps_flash_info,location_start,data_size);
    hi_mux_post(g_gps_mutex);

    return GPS_DATA_OK;
}

int save_gps_location_info(hi_u8 * location_start, hi_u8 * location_end)
{
    hi_u32 ret = 0;
    hi_u32 data_size = location_end - location_start;
    
    /* 20K 0x1F 5000 ~ 0x1F 6FFF */
    ret = hi_get_usr_partition_table(&g_usr_gps_partition_addr,&g_usr_gps_partition_size);
    if(ret != 0){
        printf("get usr partition table failed \r\n"); 
        return GPS_DATA_ERR; 
    }

    return gps_data_save_flash(g_usr_gps_partition_addr,g_usr_gps_partition_size,location_start,data_size);
}

#define PIONTS_FILITE_TO_SAVE 5

_Bool is_gps_location_info_valid(hi_u8 * location_start, hi_u8 * location_end)
{
    static int filiter_cunt = 0;

    if(location_start == NULL || location_end == NULL)
        return false;
    
    //if(strchr(location_start,'N')&&strchr(location_start,'E')){
    if(strstr(location_start,",N")&&\
        strstr(location_start,",E")&&\
        (filiter_cunt++ >= PIONTS_FILITE_TO_SAVE)){
        printf("location valid , ready to save\r\n");
        filiter_cunt = 0;
        return true;
    }
   
    printf("invalid location\r\n"); 

    return false;
}

int get_location_info2save(hi_u8 * gps_data_frame)
{
    hi_u8 * gps_location_start = NULL;
    hi_u8 * gps_location_end = NULL;
    
    gps_location_start = strstr(gps_data_frame,GPS_DATA_LOCALTION_HEAD);
    gps_location_end = strstr(gps_data_frame,GPS_DATA_LOCALTION_TAIL) - 1;
    
    if(gps_location_start == NULL||gps_location_end == NULL){
        return GPS_DATA_ERR;
    }

    if(!is_gps_location_info_valid(gps_location_start,gps_location_end)){
        return GPS_DATA_ERR;
    }

    return save_gps_location_info(gps_location_start,gps_location_end);
}

int process_gps_data(hi_u8 * gps_data_buff,hi_u32 * read_pos)
{
    hi_u32 ret = 0;
    hi_u8 * ptr_ret = NULL;

    log_printf("processing gps data \r\n");

    if(gps_data_buff[0] != '$'){
        log_printf("invalid frame start\r\n");
        return GPS_DATA_ERR_HEAD;
    }else{
        if(*read_pos >= strlen(GPS_DATA_FRAME_HEAD)){
            ret = strncmp(gps_data_buff,GPS_DATA_FRAME_HEAD,strlen(GPS_DATA_FRAME_HEAD));
            if(ret != 0){ /* invalid head */
                memset(gps_data_buff,0,*read_pos);
                *read_pos = 0;
                return GPS_DATA_ERR_HEAD;
            }

            /* GPS_LOG("frame start detect\r\n"); */

            if(gps_data_buff[*read_pos] != '\n'){
                (*read_pos) ++;
                log_printf("* read_pos %d \r\n",*read_pos);
                return GPS_DATA_NOT_END;
            }else{/* one secontion complete */
                log_printf("one frame complete,in gps_data_buff :\r\n%s \r\n",gps_data_buff);

                ptr_ret = strstr(gps_data_buff,GPS_DATA_TAIL_HEAD);
                if(ptr_ret != NULL){ /* One frame of gps data end */
                    printf("frame complete,in gps_data_buff :\r\n%s \r\n",gps_data_buff);
                    hi_uart_write_immediately(HI_UART_IDX_2,gps_data_buff,strlen(gps_data_buff));
                    
                    get_location_info2save(gps_data_buff);
                   
                    memset(gps_data_buff,0,*read_pos);
                    *read_pos = 0;
                    return GPS_DATA_OK;
                } 
            }
        }
        (*read_pos) ++;
        log_printf("process return \r\n");
        return GPS_DATA_NOT_END;
    }

}

