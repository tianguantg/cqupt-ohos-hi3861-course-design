#include "oled.h"
#include "gui.h"
#include "test.h"
void test_led_screen(void)
{	
    //delay_init();	    	       //延时函数初始化	  
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级	
    printf("before oled clear \r\n");
    OLED_Init();			         //初始化OLED  
    OLED_Clear(0);             //清屏（全黑）
    printf("after oled clear \r\n");
    while(1) 
    {	
        
        printf("before main page clear \r\n");
        /*
        TEST_BMP();              //BMP单色图片显示测试
        TEST_LINE();
        TEST_Chinese();          //中文显示测试
        printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        OLED_Clear(0); 
        */
        TEST_Menu2();            //菜单2显示测试
        printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        OLED_Clear(0); 

        printf("main loop ......\r\n");
    }
}

void display_hr(void)
{	
    //delay_init();	    	       //延时函数初始化	  
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级	
    printf("before oled clear \r\n");
    OLED_Init();			         //初始化OLED  
    OLED_Clear(0);             //清屏（全黑）
    printf("after oled clear \r\n");
    while(1) 
    {	
        
        //printf("before main page clear \r\n");
        TEST_CURVE();            //HR曲线显示
        //printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        OLED_Clear(0); 

        //printf("main loop ......\r\n");
    }
}
