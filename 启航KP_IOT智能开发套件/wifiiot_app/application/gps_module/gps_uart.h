#ifndef _GPS_UART_H_
#define _GPS_UART_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <hi_types_base.h>
#include <hi_early_debug.h>
#include <hi_task.h>
#include <hi_uart.h>
//#include <app_demo_uart.h>

//#define WRITE_BY_INT
#define UART_DEMO_TASK_STAK_SIZE 2048
#define UART_DEMO_TASK_PRIORITY  25
#define DEMO_UART_NUM            HI_UART_IDX_1

//max onece readLen is 16
//#define UART_BUFF_SIZE           32
#define UART_BUFF_SIZE           512

extern hi_u32 g_gps_mutex;
hi_void gps_uart_demo(hi_void);

#ifdef __cplusplus
}
#endif

#endif /* _GPS_UART_H_ */
