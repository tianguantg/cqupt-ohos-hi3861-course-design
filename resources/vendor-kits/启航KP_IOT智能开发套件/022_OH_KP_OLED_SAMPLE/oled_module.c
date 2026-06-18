#include <stdio.h>
#include <unistd.h>
#include <cmsis_os2.h>
#include <ohos_init.h>
#include "iot_spi.h"
#include "oled_module.h"
#include "oled_GME12864.h"
#include "oled_gui.h"


void SpiOledTask(void)
{
    OLED_Init();               //初始化OLED  

	/* 清屏（全黑） */
    OLED_Clear(0);
    printf("oled clear \r\n");

	/* 循环OLED显示并清屏 */
    while(1)
    {
        printf("start display \r\n");
        /*
        TEST_BMP();              //BMP单色图片显示测试
        TEST_LINE();             //绘制线条测试
        TEST_Chinese();          //中文显示测试
        printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        OLED_Clear(0); 

        TEST_Menu2();            //菜单2显示测试
        TEST_MainPage();
        */
        TEST_BMP();              //BMP单色图片显示测试
        OLED_Clear(0);
		printf("stop display \r\n");
     }
}

void oled_module_demo(void)
{
	/* 初始化OLED  */
	OledIoInit();

	/* 设置线程属性 */
    osThreadAttr_t attr;
    attr.name = "SpiOledTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 20;

    /* 创建按键控制风扇线程 */
    osThreadId_t threadID1 = osThreadNew((osThreadFunc_t)SpiOledTask, NULL, &attr);
    if (threadID1 == NULL)
    {
        printf("Falied to create SpiOledTask!\r\n");
    }
}
APP_FEATURE_INIT(oled_module_demo);

