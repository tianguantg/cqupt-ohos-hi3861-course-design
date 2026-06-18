#ifndef __FINGERPRINT_MODULE_H__
#define __FINGERPRINT_MODULE_H__

#include <hi_types_base.h>
#include <hi_io.h>
#include <hi_early_debug.h>
#include <hi_gpio.h>
#include <hi_task.h>
#include <hi_uart.h>

#define VERIFY_FINGERPRINT 0x08
#define LOGIN_FINGERPRINT 0x05
#define EMPTY_FINGERPRINT 0x03

typedef struct frame_head{
    hi_u8   start[2];
    hi_u8   addr[4];
    hi_u8   inden;
    hi_u8   data_len[2];
    hi_u8   frame_type;
}frame_head_t;

typedef struct frame_tail{
    hi_u8   param[2];
    hi_u8   check_sum[2];
}frame_tail_t;

typedef struct login_cmd{
   frame_head_t frame_head;
   hi_u8 login_id[2];
   hi_u8 times;
   frame_tail_t frame_tail;
}login_cmd_t;

hi_void fingerprint_demo(hi_void);
void save_login_id(int id_t);
int get_login_id(hi_void);
#endif  
