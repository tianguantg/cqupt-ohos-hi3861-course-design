#ifndef __TRIAXIAL_I2C_H__
#define __TRIAXIAL_I2C_H__

#include <hi_types_base.h>
#include <hi_i2c.h>
#include <hi_early_debug.h>
#include <hi_stdlib.h>

#define 	    IIC_Addr   					    0x32
#define 	    CTRL_REG1_Addr   				0x20
#define 	    CTRL_REG2_Addr   			    0x21
#define 	    CTRL_REG3_Addr   			    0x23
#define 		LIS3DH_OUT_X_L					0x28
#define 		LIS3DH_OUT_X_H					0x29
#define 		LIS3DH_OUT_Y_L					0x2A
#define 		LIS3DH_OUT_Y_H					0x2B
#define 		LIS3DH_OUT_Z_L					0x2C
#define 		LIS3DH_OUT_Z_H					0x2D

// 重力加速度常量 //
#define  	  GRAVITY_EARTH         	9.80665f 
// ±2 G 分辨率 //
#define 		GSENSOR_RESOLUTION    	16384

#define    TRUE   1
#define    FALSE	0

typedef enum{
 NO_ERROR = 0x00, // no error
 ACK_ERROR = 0x01, // no acknowledgment error
 CHECKSUM_ERROR = 0x02, // checksum mismatch error
 TIMEOUT_ERROR = 0x04, // timeout error
 PARM_ERROR = 0x80, // parameter out of range error
}etError;

typedef struct{
	
	unsigned char flag;
	unsigned char num;
	unsigned short step;
	float    Value;
}TR_R;

extern TR_R  g_r;

#endif
