#include <stdio.h>
#include <hi_flash.h>

#include "gps_data_process.h"

typedef struct gps_data_info{
    hi_u32  start_addr;
    hi_u32  gps_data_addr;
    hi_u32  write_addr;
    hi_u32  read_addr;
    hi_u32  partition_size;
}gps_data_info_t;

static hi_u32 g_usr_gps_partition_addr = 0;
static hi_u32 g_usr_gps_partition_size = 0;

#define MAX_GPS_DATA_LOCATION_LEN 128

static int get_gps_flash_info(gps_data_info_t * flash_info,hi_u32 info_size)
{
    int ret = 0;
    if(flash_info == NULL){
        printf("invalid para flash_info\r\n");
        return GPS_DATA_ERR;
    }

    ret = hi_flash_read(g_usr_gps_partition_addr,info_size,flash_info);
    if(ret != 0){
        printf("get gps flash inf hi flash read falied\r\n"); 
        return GPS_DATA_ERR; 
    }

    if(flash_info->start_addr == 0xFFFFFFFF){
        flash_info->start_addr = g_usr_gps_partition_addr;
        /* align 64 up high addr */
        flash_info->gps_data_addr = (sizeof(gps_data_info_t) + (MAX_GPS_DATA_LOCATION_LEN - 1))&(~(MAX_GPS_DATA_LOCATION_LEN - 1));
        flash_info->write_addr = flash_info->gps_data_addr;
        flash_info->read_addr = flash_info->gps_data_addr;
        /* align 64 down low addr*/
        flash_info->partition_size = (g_usr_gps_partition_size)&(~(MAX_GPS_DATA_LOCATION_LEN-1));
        printf("is 64 align : %d \r\n",!((flash_info->partition_size - flash_info->gps_data_addr)%64));
    }

    printf("get_gps_flash_info addr of  start : %d,write : %d,read : %d,size : %d \r\n",
        flash_info->start_addr,flash_info->write_addr,flash_info->read_addr,flash_info->partition_size);

    return GPS_DATA_OK;
}

//hi_flash_read(const hi_u32 flash_offset,const hi_u32 size, hi_u8 * ram_data)
//
//hi_flash_write(const hi_u32 flash_offset,hi_u32 size,const hi_u8 * ram_data,hi_bool do_erase)
//offset addr ,sizeof data to write 

#define GPS_DATA_CYCL_QUEUE(flash_info) (&(((unsigned char *)(flash_info->start_addr))[sizeof(gps_data_info_t)]))
#define GPS_QUEUE  GPS_DATA_CYCL_QUEUE(flash_info)

#define is_QUEUE_FULL(flash_info)  (((flash_info->write_addr)+MAX_GPS_DATA_LOCATION_LEN)%(flash_info->partition_size) == (flash_info->read_addr))
#define is_GPS_QUEUE_FULL()  is_QUEUE_FULL(flash_info)

#define is_QUEUE_EMPTY(flash_info) ((flash_info->read_addr)==flash_info->write_addr)
#define is_GPS_QUEUE_EMPTY() is_QUEUE_EMPTY(flash_info)

#define GPS_ENQUEUE(gps_data,datalen) do {\
    if((datalen <= MAX_GPS_DATA_LOCATION_LEN)){\
        if(is_GPS_QUEUE_FULL()){\
            printf("queue full\r\n");\
            flash_info->read_addr+=MAX_GPS_DATA_LOCATION_LEN;\
        }\
        ret = hi_flash_write(flash_info->start_addr+flash_info->write_addr,MAX_GPS_DATA_LOCATION_LEN,gps_data,true);\
        if(ret != 0){\
            printf("GPS_ENQUEUE hi_flash_write falied %#x \r\n",ret);\
            return GPS_DATA_ERR;\
        }\
        flash_info->write_addr = (flash_info->write_addr+MAX_GPS_DATA_LOCATION_LEN)%(flash_info->partition_size - flash_info->gps_data_addr);\
        printf("GPS_ENQUEUE executed \r\n");\
        printf("flash_info->write_addr+MAX_GPS_DATA_LOCATION_LEN : %d \r\n",flash_info->write_addr+MAX_GPS_DATA_LOCATION_LEN);\
        printf("flash_info->partition_size - flash_info->gps_data_addr : %d \r\n",flash_info->partition_size - flash_info->gps_data_addr);\
        printf("after enqueue : write_addr :%d \r\n",flash_info->write_addr);\
    }\
}while(0)

#define GPS_DEQUEUE(ram_data)  do {\
    if(!is_GPS_QUEUE_EMPTY()){\
            ret = hi_flash_read(flash_info->start_addr+flash_info->read_addr,MAX_GPS_DATA_LOCATION_LEN,ram_data);\
            if(ret != 0){\
                printf("GPS_DEQUEUE hi_flash_read falied %#x \r\n",ret);\
                return GPS_DATA_ERR;\
            }\
            flash_info->read_addr = (flash_info->read_addr+MAX_GPS_DATA_LOCATION_LEN)%(flash_info->partition_size - flash_info->gps_data_addr);\
    }\
}while(0)

/* 20K 0x1F 5000 ~ 0x1F 6FFF */

static int save_gps_flash_info(gps_data_info_t *flash_info)
{
    //return hi_flash_write(flash_info->start_addr,sizeof(gps_data_info_t),flash_info,false);
    return hi_flash_write(flash_info->start_addr,sizeof(gps_data_info_t),flash_info,true);
}

