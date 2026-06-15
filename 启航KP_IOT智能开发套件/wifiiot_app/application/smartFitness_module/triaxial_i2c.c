#include <stdio.h>
#include "triaxial_i2c.h"
#include "math.h"

unsigned char read_reg_data(unsigned char addr)
{
    int ret = -1;
    char temp;
    unsigned char sendbuf[1] = {0};
    unsigned char rcvbuf[1] = {0};

    hi_i2c_data triaxial_i2c_data = { 0 };
    triaxial_i2c_data.send_buf = sendbuf;
    triaxial_i2c_data.send_len = sizeof(sendbuf);
    triaxial_i2c_data.receive_buf = rcvbuf;
    triaxial_i2c_data.receive_len = sizeof(rcvbuf);

    
    sendbuf[0] = addr;
    ret = hi_i2c_write(0,IIC_Addr,&triaxial_i2c_data);
    if(ret != 0){
        printf("hi_i2c_write  failed ret :%#x \r\n",ret);
        return ret;
    }
    sendbuf[0] = addr;
    ret = hi_i2c_read(0,IIC_Addr | 0x01,&triaxial_i2c_data);
    if(ret != 0){
        printf("hi_i2c_read x_l failed ret :%#x \r\n",ret);
        return ret;
    }

    temp = rcvbuf[0];
    return temp;
}

hi_void triaxail_i2c_init()
{
    int ret = -1;
    int temp;
    unsigned char sendbuf[2] = {0};
    unsigned char rcvbuf[1] = {0};

    hi_i2c_data triaxial_i2c_data = { 0 };
    triaxial_i2c_data.send_buf = sendbuf;
    triaxial_i2c_data.send_len = sizeof(sendbuf);
    triaxial_i2c_data.receive_buf = rcvbuf;
    triaxial_i2c_data.receive_len = sizeof(rcvbuf);

    temp = read_reg_data(0x0f);
    printf("temp:%x\r\n",temp);

    sendbuf[0] = 0x20;
    sendbuf[1] = 0x77;
    ret = hi_i2c_write(0,IIC_Addr,&triaxial_i2c_data);
    if(ret != 0){
        printf("hi_i2c_write  failed ret :%#x \r\n",ret);
        return ret;
    }

    temp = read_reg_data(CTRL_REG1_Addr);
    printf("reg1:%x\r\n",temp);

    
    sendbuf[0] = 0x21;
    sendbuf[1] = 0x04;
    ret = hi_i2c_write(0,IIC_Addr,&triaxial_i2c_data);
    if(ret != 0){
        printf("hi_i2c_write  failed ret :%#x \r\n",ret);
        return ret;
    }
 
    temp = read_reg_data(CTRL_REG2_Addr);
    printf("reg2:%x\r\n",temp);
    
    sendbuf[0] = 0x23;
    sendbuf[1] = 0x88;
    ret = hi_i2c_write(0,IIC_Addr,&triaxial_i2c_data);
    if(ret != 0){
        printf("hi_i2c_write  failed ret :%#x \r\n",ret);
        return ret;
    }

    temp = read_reg_data(CTRL_REG3_Addr);
    printf("reg4:%x\r\n",temp);
    sleep(1);
    printf("triaxial init success \r\n");
}

void Read_XYZ(void)
{
    signed short int x =0,y = 0,z = 0;
    int i = 0;
    int Temp = 0;
    unsigned int Value;

    float R_x,R_y,R_z,accd,test;
    static int step = 0;
    static int num = 0;
    static int Value_t = 0;

    for(int i = 0; i < 8;i++){

        x +=(signed short int) (read_reg_data(LIS3DH_OUT_X_L) |(read_reg_data(LIS3DH_OUT_X_H) << 8)); 
        y +=(signed short int) (read_reg_data(LIS3DH_OUT_Y_L) |(read_reg_data(LIS3DH_OUT_Y_H) << 8)); 
        z +=(signed short int) (read_reg_data(LIS3DH_OUT_Z_L) |(read_reg_data(LIS3DH_OUT_Z_H) << 8)); 
        x >>= 6;
        y >>= 6;
        z >>= 6;

    }

    x /= 8;
    y /= 8;
    z /= 8;
    x -= 1;
    y += 2;
    z -= 32;
    printf("x:%d y:%d z:%d \r\n",x,y,z);
   
    Value =  sqrt(x * x	+ y * y + z * z );


    Temp = Value - Value_t ;
    printf("Value: %d  Value_t:%d Temp:%d \r\n",Value,Value_t,Temp);

    
    if(Temp < -30 && Value > 50 && z > -20 && num > 1){
        step ++;
        num = 0;
        Value_t = Value;
        triaxial_pwm_start(50);
        printf("step = %d\r\n",step);
    }else{
        num++;
        triaxial_pwm_start(0);
    }
        Value_t = Value;
   
    printf("=============\r\n");

}


