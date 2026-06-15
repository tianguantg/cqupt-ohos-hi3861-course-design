//#include "mbed.h"
#include <stdio.h>
#include <unistd.h>
#include "dbg.h"
#include "MAX30102.h"
#include "heart_rate_module.h"
#include "hi_i2c.h"

//I2C i2c(I2C_SDA, I2C_SCL);//SDA-PB9,SCL-PB8

void rcv_buf(unsigned char * buf,unsigned int len)
{
    if(buf == NULL)
        return ;

    DBG("in buf : \r\n");
    for(int i = 0; i < len; i++){
        printf("%#x ",*buf++); 
    }
    printf(" \r\n");
}

int maxim_max30102_write_reg(char uch_addr, char uch_data)
{
    int ret = -1;
    unsigned char sendbuf[2] = {0};
    unsigned char rcvbuf[2] = {0};

    hi_i2c_data i2c_write_data = { 0 };
    i2c_write_data.send_buf = sendbuf;
    i2c_write_data.send_len = sizeof(sendbuf);
    i2c_write_data.receive_buf = rcvbuf;
    i2c_write_data.receive_len = sizeof(rcvbuf);

    printf("addr:%x,data:%x \r\n",uch_addr,uch_data);
    sendbuf[0] = uch_addr;
    sendbuf[1] = uch_data;
    ret = hi_i2c_write(0,I2C_WRITE_ADDR,&i2c_write_data);
    if(ret != 0){
        DBG("hi_i2c_write failed ret :%#x \r\n",ret);
        return false; 
    }
    return true;
}

int maxim_max30102_read_reg(char uch_addr, char *puch_data)
{  
    int ret = -1;
    unsigned char sendbuf[1] = {0};
    unsigned char rcvbuf[6] = {0};

    hi_i2c_data i2c_data = { 0 };
    i2c_data.send_buf = sendbuf;
    i2c_data.send_len = sizeof(sendbuf);
    i2c_data.receive_buf = rcvbuf;
    i2c_data.receive_len = sizeof(rcvbuf);

    sendbuf[0] = uch_addr;
    ret = hi_i2c_write(0,I2C_WRITE_ADDR,&i2c_data);
    if(ret != 0){
        DBG("hi_i2c_write failed ret :%#x \r\n",ret);
        return false; 
    }
    ret = hi_i2c_read(0, I2C_READ_ADDR, &i2c_data);
    if(ret != 0){
        DBG("hi_i2c_read failed ret :%#x \r\n",ret);
        return false;
    }
    /*
    for(int i = 0;i < 6;i++){
        printf("rcv[%d]:%x  ",i,rcvbuf[i]);
    }
    printf(" \r\n");
    */
    //rcv_buf(rcvbuf,sizeof(rcvbuf));
    *puch_data=(char) rcvbuf;

    return true;
}

int maxim_max30102_init()
{

    if(!maxim_max30102_write_reg(REG_INTR_ENABLE_1,0xc0)) // INTR setting
        return false;
    if(!maxim_max30102_write_reg(REG_INTR_ENABLE_2,0x00))
        return false;
    if(!maxim_max30102_write_reg(REG_FIFO_WR_PTR,0x00))  //FIFO_WR_PTR[4:0]
        return false;
    if(!maxim_max30102_write_reg(REG_OVF_COUNTER,0x00))  //OVF_COUNTER[4:0]
        return false;
    if(!maxim_max30102_write_reg(REG_FIFO_RD_PTR,0x00))  //FIFO_RD_PTR[4:0]
        return false;
    if(!maxim_max30102_write_reg(REG_FIFO_CONFIG,0x0f))  //sample avg = 1, fifo rollover=false, fifo almost full = 17
        return false;
    if(!maxim_max30102_write_reg(REG_MODE_CONFIG,0x03))   //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
        return false;
    if(!maxim_max30102_write_reg(REG_SPO2_CONFIG,0x27))  // SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)
        return false;
    if(!maxim_max30102_write_reg(REG_LED1_PA,0x24))   //Choose value for ~ 7mA for LED1
        return false;
    if(!maxim_max30102_write_reg(REG_LED2_PA,0x24))   // Choose value for ~ 7mA for LED2
        return false;
    if(!maxim_max30102_write_reg(REG_PILOT_PA,0x7f))   // Choose value for ~ 25mA for Pilot LED
        return false;
    return true;  
}

int maxim_max30102_read_fifo(int *pun_red_led, int *pun_ir_led)
{ 
    int un_temp;
    int ret = -1;
    *pun_red_led=0;
    *pun_ir_led=0;
    unsigned char uch_temp;

    unsigned char sendbuf[1] = {0};
    unsigned char rcvbuf[6] = {0};

    hi_i2c_data i2c_data = { 0 };
    i2c_data.send_buf = sendbuf;
    i2c_data.send_len = 1;
    i2c_data.receive_buf = rcvbuf;
    i2c_data.receive_len = sizeof(rcvbuf);
    //read and clear status register
    maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_temp);
    maxim_max30102_read_reg(REG_INTR_STATUS_2, &uch_temp);

    sendbuf[0]=REG_FIFO_DATA;
    ret = hi_i2c_write(0,I2C_WRITE_ADDR,&i2c_data);
    if(ret != 0){
        DBG("hi_i2c_write failed ret :%#x \r\n",ret);
        return false; 
    }
    ret = hi_i2c_read(0, I2C_READ_ADDR, &i2c_data);
    if(ret != 0){
        DBG("hi_i2c_read failed ret :%#x \r\n",ret);
        return false;
    }

    un_temp=(unsigned char) rcvbuf[0];
    un_temp<<=16;
    *pun_red_led+=un_temp;
    un_temp=(unsigned char) rcvbuf[1];
    un_temp<<=8;
    *pun_red_led+=un_temp;
    un_temp=(unsigned char) rcvbuf[2];
    *pun_red_led+=un_temp;

    un_temp=(unsigned char) rcvbuf[3];
    un_temp<<=16;
    *pun_ir_led+=un_temp;
    un_temp=(unsigned char) rcvbuf[4];
    un_temp<<=8;
    *pun_ir_led+=un_temp;
    un_temp=(unsigned char) rcvbuf[5];
    *pun_ir_led+=un_temp;
    *pun_red_led&=0x03FFFF;  //Mask MSB [23:18]
    *pun_ir_led&=0x03FFFF;  //Mask MSB [23:18]

    return true;
}

int maxim_max30102_reset()
{    
    if(!maxim_max30102_write_reg(REG_MODE_CONFIG,0x40)){
        DBG("max30102 reset failed \r\n");
        return false;
    }else{
        DBG("max30102 reset successed \r\n");
        return true;    
    }
}
