#ifndef __KP_BASE_TYPE_H__
#define __KP_BASE_TYPE_H__

#include <stdio.h>

/* 基本数据类型 */
typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
typedef unsigned long long      u64;
typedef unsigned long           ulong;
typedef char                    s8;
typedef short                   s16;
typedef int                     s32;
typedef long long               s64;
typedef long                    slong;

#undef ERROR
#define ERROR (-1)

#define KP_ERR_SUCCESS  0
#define KP_ERR_FAILURE  (-1)

#ifndef NULL
#define NULL ((void *)0)
#endif

#define KP_U16_MAX            0xFFFF
#define KP_U32_MAX            0xFFFFFFFF
#define KP_U64_MAX            0xFFFFFFFFFFFFFFFFUL

#include <unistd.h>
#define msleep(x)               usleep((x)*1000)

typedef enum {
    GPIO_OUT_LOW = 0,      /**< Low level.CNcomment:低电平CNend*/
    GPIO_OUT_HIGH           /**< High level.CNcomment:高电平CNend*/
} gpio_out_value;

typedef enum {
    LED_OFF = 0,
    LED_ON,
    LED_SPARK
} led_state;

#define KP_DEBUG(fmt, args...)  do {                \
		printf("[KP][%s][%d]", __FILE__, __LINE__); \
	    printf(fmt, ##args);                        \
	    printf("\r\n");                             \
    } while (0)


#endif