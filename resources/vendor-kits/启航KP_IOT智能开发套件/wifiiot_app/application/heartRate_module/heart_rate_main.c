//#include "stm32f103c8t6.h"
//#include "mbed.h"
#include "algorithm.h"
#include "MAX30102.h"
#include "heart_rate_module.h"

#define MAX_BRIGHTNESS 255

int hr_valid = 0;
int sp02_valid = 0;
int aun_ir_buffer[500]; //IR LED sensor data
int n_ir_buffer_length;    //data length
int aun_red_buffer[500];    //Red LED sensor data
int n_sp02; //SPO2 value
char ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
int n_heart_rate;   //heart rate value
char  ch_hr_valid;    //indicator to show if the heart rate calculation is valid
char uch_dummy;

//Serial pc(SERIAL_TX, SERIAL_RX);    //initializes the serial port, TX-PA2, RX-PA3

//PwmOut pwmled(PB_3);  //initializes the pwm output PB3 that connects to the LED
//DigitalIn INT(PB_7);  //pin PB7 connects to the interrupt output pin of the MAX30102
//DigitalOut led(PC_13); //PC13 connects to the on board user LED


// the setup routine runs once when you press reset:
int test_main() { 
    int un_min, un_max, un_prev_data;  //variables to calculate the on-board LED brightness that reflects the heartbeats
    int i;
    int n_brightness;
    float f_temp;
    int ret;
    
    maxim_max30102_reset(); //resets the MAX30102
    // initialize serial communication at 115200 bits per second:
    //pc.baud(115200);
    //pc.format(8,SerialBase::None,1);
    sleep(1);
    
    //read and clear status register
    maxim_max30102_read_reg(0,&uch_dummy);
    
    //wait until the user presses a key
//    while(pc.readable()==0)
//    {
//        pc.printf("\x1B[2J");  //clear terminal program screen
//        pc.printf("Press any key to start conversion\n\r");
//        wait(1);
//    }
//    uch_dummy=getchar();
    
    maxim_max30102_init();  //initializes the MAX30102
        
    n_brightness=0;
    un_min=0x3FFFF;
    un_max=0;
  
    n_ir_buffer_length=500; //buffer length of 100 stores 5 seconds of samples running at 100sps
    
    //read the first 500 samples, and determine the signal range
    for(i=0;i<n_ir_buffer_length;i++)
    {

        ret = hr_int();
        while(ret == 1){
            ret = hr_int(); 
        }   //wait until the interrupt pin asserts

        maxim_max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));  //read from MAX30102 FIFO
            
        if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];    //update signal min
        if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];    //update signal max
  
    }
    un_prev_data=aun_red_buffer[i];
    
    
    //calculate heart rate and SpO2 after first 500 samples (first 5 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
    
    //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
    while(1)
    {
        i=0;
        un_min=0x3FFFF;
        un_max=0;
        
        //dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
        for(i=100;i<500;i++)
        {
            aun_red_buffer[i-100]=aun_red_buffer[i];
            aun_ir_buffer[i-100]=aun_ir_buffer[i];
            
            //update the signal min and max
            if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];
            if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];
        }
        
        //take 100 sets of samples before calculating the heart rate.
        for(i=400;i<500;i++)
        {
            ret = 0;
            un_prev_data=aun_red_buffer[i-1];
            
            ret = hr_int();
            while(ret == 1){
                ret = hr_int();
            }
            //while(INT.read()==1);
            maxim_max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));
        
            if(aun_red_buffer[i]>un_prev_data)//just to determine the brightness of LED according to the deviation of adjacent two AD data
            {
                f_temp=aun_red_buffer[i]-un_prev_data;
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness-=(int)f_temp;
                if(n_brightness<0)
                    n_brightness=0;
            }
            else
            {
                f_temp=un_prev_data-aun_red_buffer[i];
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness+=(int)f_temp;
                if(n_brightness>MAX_BRIGHTNESS)
                    n_brightness=MAX_BRIGHTNESS;
            }
            
            /*
            pwmled.write(1-(float)n_brightness/256);//pwm control led brightness
						if(n_brightness<120)
							led=1;
						else
							led=0;
            */

            //send samples and calculation result to terminal program through UART
            /*
            pc.printf("red=");
            pc.printf("%i", aun_red_buffer[i]);
            pc.printf(", ir=");
            pc.printf("%i", aun_ir_buffer[i]);
            pc.printf(", HR=%i, ", n_heart_rate); 
            pc.printf("HRvalid=%i, ", ch_hr_valid);
            pc.printf("SpO2=%i, ", n_sp02);
            pc.printf("SPO2Valid=%i\n\r", ch_spo2_valid);
            */
            

        }
        maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
            printf("red=");
            printf("%i", aun_red_buffer[i]);
            printf(", ir=");
            printf("%i", aun_ir_buffer[i]);
            printf(", HR=%i, ", n_heart_rate); 
            printf("HRvalid=%i, ", ch_hr_valid);
            printf("SpO2=%i, ", n_sp02);
            printf("SPO2Valid=%i\n\r", ch_spo2_valid);

            if(n_heart_rate < 120 && ch_hr_valid == 1){
                hr_valid = n_heart_rate;
            }else{
                hr_valid = 0;
            }

            if(n_sp02 <= 100 && ch_spo2_valid == 1){
                sp02_valid = n_sp02;
            }else{
                sp02_valid = 0;
            }
    }
}
 
