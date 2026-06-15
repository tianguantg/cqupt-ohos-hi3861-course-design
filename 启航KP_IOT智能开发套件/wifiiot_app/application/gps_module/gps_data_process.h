#ifndef _GPS_DATA_PROCESS_H_
#define _GPS_DATA_PROCESS_H_

#ifdef __cplusplus
extern "C"{
#endif 

//extern hi_u32 g_usr_partition_addr;
//extern hi_u32 g_usr_partition_size;

#define GPS_DATA_FRAME_HEAD "$GPGGA,"
#define GPS_DATA_OK 0
#define GPS_DATA_ERR -1
#define GPS_DATA_NOT_END -2
#define GPS_DATA_ERR_HEAD -3
#define GPS_DATA_TAIL_HEAD "$GPVTG,"
#define GPS_DATA_LOCALTION_HEAD "$GPGGA,"
#define GPS_DATA_LOCALTION_TAIL "$GPGLL,"


#define GPS_LOG(fmt,...) \
    do {\
        printf(fmt,##__VA_ARGS__);\
    }while(0)
/*
printf("FILE:%s,FUNC:%s,LINE:%d\r\n",__FILE__,__func__,__LINE__);\
printf(#fmt,##__VA_ARGS__);\
        printf(fmt,##__VA_ARGS__);\
    }while(0)
*/

#define log_printf(...)

extern int process_gps_data(hi_u8 * gps_data_buff,hi_u32 * read_pos);

extern int gps_data_read_flash();

#ifdef __cplusplus
}
#endif

#endif /* _GPS_DATA_PROCESS_H_ */
