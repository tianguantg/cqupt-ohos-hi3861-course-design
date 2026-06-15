# 启航KP_IOT OLED显示模块
本示例将演示如何利用启航KP_IOT主控板和OLED显示模块上进行案例开发。

## 概述
oled模块显示尺寸为1.3寸，配有7个引脚，采用四线制spi通讯方式，驱动IC是SH1106，分辨率为128x64，显示颜色为蓝色。该oled模块采用四线制spi通讯方式，硬件配置4个引脚，CS（片选），DC（命令/数据选择），MOSI（spi写数据），CLK（spi时钟）。按照spi工作时序来控制4个引脚就可以完成spi数据传输。

**4线制spi写模式的工作时序：**  
*  CSX为从机片选，仅当CSX为低电平时，芯片才会被使能。
*  D/CX为芯片数据/命令控制引脚，当D/CX为低电平时写命令，高电平时写数据。
*  SCL为总线时钟，SCL每个上升沿上送1bit数据。  
*  SDA为spi传输的数据，一次传输8bit数据，数据格式高位在前，先传输。

**对于spi通信而言，数据传输是有时序的，即时钟相位（CPHA）与时钟极性（CPOL）的组合：**

*  CPOL的高低决定串行同步时钟空闲状态的电平，CPOL=0，为低电平。
*  CPHA的高低决定串行同步时钟是在第一个时钟跳变沿还是第二个时钟跳变沿的数据被采集，CPHA=0时，第一个边沿的数据被采集。


## 主要试验步骤

* 步骤1 首先是spi的复用属于功能的复用，在app/wifiiot_app/init/app_io_init.c文件中进行复用。

* 步骤2 将三个对应的io复用成输出形式
```
hi_void oled_gpio_io_init(void)
{
    hi_u32 ret;
    
    ret = hi_io_set_func(HI_IO_NAME_GPIO_8, HI_IO_FUNC_GPIO_8_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_8, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");
    ret = hi_io_set_func(HI_IO_NAME_GPIO_11, HI_IO_FUNC_GPIO_11_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");
   
    ret = hi_gpio_set_dir(HI_GPIO_IDX_11, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");
    ret = hi_io_set_func(HI_IO_NAME_GPIO_12, HI_IO_FUNC_GPIO_12_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");
    ret = hi_gpio_set_dir(HI_GPIO_IDX_12, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");
}
```

* 步骤3 io复用完成后开始写显示功能的代码，显示功能的入口是led_screen_main.c中的test_led_screen();，我们在该函数中完成自己想要显示的内容，显示之前需要对oled进行初始化：

```
void test_led_screen(void)
{   
    //delay_init();                //延时函数初始化      
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //设置NVIC中断分组2:2位抢占优先级，2位响应优先级  
    printf("before oled clear \r\n");
    OLED_Init();                     //初始化OLED  
    OLED_Clear(0);             //清屏（全黑）
    printf("after oled clear \r\n");
    while(1) 
    {   
        
        printf("before main page clear \r\n");
       
        TEST_BMP();              //BMP单色图片显示测试
        // TEST_LINE();
        // TEST_Chinese();          //中文显示测试
        // printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        // OLED_Clear(0); 
       
        // TEST_Menu2();            //菜单2显示测试
        // printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        // OLED_Clear(0); 

        printf("main loop ......\r\n");
    }
}
```
* 步骤4  oled的初始化主要是复位oled然后初始化SSD1306，SSD1306是oled模块上驱动显示的芯片，初始化完成后我们就可以进行显示了。

```
void OLED_Init(void)
{
    //OLED_Init_GPIO(); //初始化GPIO
    //printf("%s,%s,%d \r\n",__FILE__,__func__,__LINE__);
    delay_ms(200); 
    OLED_Reset();     //复位OLED
    printf("%s,%s,%d \r\n",__FILE__,__func__,__LINE__);

/**************初始化SSD1306*****************/ 
    OLED_WR_Byte(0xAE,OLED_CMD); /*display off*/
    OLED_WR_Byte(0x00,OLED_CMD); /*set lower column address*/
    OLED_WR_Byte(0x10,OLED_CMD); /*set higher column address*/
    OLED_WR_Byte(0x40,OLED_CMD); /*set display start line*/ 
    OLED_WR_Byte(0xB0,OLED_CMD); /*set page address*/
    OLED_WR_Byte(0x81,OLED_CMD); /*contract control*/ 
    OLED_WR_Byte(0xFF,OLED_CMD); /*128*/
    OLED_WR_Byte(0xA1,OLED_CMD); /*set segment remap*/ 
    OLED_WR_Byte(0xA6,OLED_CMD); /*normal / reverse*/
    OLED_WR_Byte(0xA8,OLED_CMD); /*multiplex ratio*/ 
    OLED_WR_Byte(0x3F,OLED_CMD); /*duty = 1/64*/
    OLED_WR_Byte(0xC8,OLED_CMD); /*Com scan direction*/
    OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/ 
    OLED_WR_Byte(0x00,OLED_CMD);
    OLED_WR_Byte(0xD5,OLED_CMD); /*set osc division*/ 
    OLED_WR_Byte(0x80,OLED_CMD);
    OLED_WR_Byte(0xD9,OLED_CMD); /*set pre-charge period*/ 
    OLED_WR_Byte(0XF1,OLED_CMD);
    OLED_WR_Byte(0xDA,OLED_CMD); /*set COM pins*/ 
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD); /*set vcomh*/ 
    OLED_WR_Byte(0x30,OLED_CMD);
    OLED_WR_Byte(0x8D,OLED_CMD); /*set charge pump disable*/ 
    OLED_WR_Byte(0x14,OLED_CMD);
    OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*
    printf("%s,%s,%d \r\n",__FILE__,__func__,__LINE__);
}  
```
oled模块的工作原理其实就是在一个128*64分辨率大小的地方写数据，显示的图片，文字其实都是采用专用工具将图片、文字转换成相应的数组，oled的控制器在这些数组对应的点位上描绘出点就达到了显示图片文字的功能。不过读取图片、文字数组的函数是不同的，这个需要我们来完成。这一部分函数在gui.c文件中声明，该文件中还包括绘点，绘字符串，绘线段，绘长方形等等，我们可以根据自己的需要调用不同的函数来实现我们想要的画面，图片和文字案例参考具体操作文档。

* 步骤5 在主函数中调用在led_screen_main.c文件的test_led_screen（）中调用图片显示
```
void test_led_screen(void)
{   
    //delay_init();                //延时函数初始化      
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //设置NVIC中断分组2:2位抢占优先级，2位响应优先级  
    printf("before oled clear \r\n");
    OLED_Init();                     //初始化OLED  
    OLED_Clear(0);             //清屏（全黑）
    printf("after oled clear \r\n");
    while(1) 
    {   
        printf("before main page clear \r\n");
        TEST_BMP();              //BMP单色图片显示测试
        // TEST_LINE();
        // TEST_Chinese();          //中文显示测试
        // printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        // OLED_Clear(0); 
        // TEST_Menu2();            //菜单2显示测试
        // printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        // OLED_Clear(0); 
        printf("main loop ......\r\n");
    }
}
```
* 步骤6  在主函数中调用，图片、文字显示功能都实现之后在oled_module.c文件的oled任务中去调用test_led_screen()即可实现图片、文字的显示。

```
void test_led_screen(void)
{   
    //delay_init();                //延时函数初始化      
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //设置NVIC中断分组2:2位抢占优先级，2位响应优先级  
    printf("before oled clear \r\n");
    OLED_Init();                     //初始化OLED  
    OLED_Clear(0);             //清屏（全黑）
    printf("after oled clear \r\n");
    while(1) 
    {   
        
        printf("before main page clear \r\n");
       
        // TEST_BMP();              //BMP单色图片显示测试
        // TEST_LINE();
        TEST_Chinese();          //中文显示测试
        printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        OLED_Clear(0); 
       
        // TEST_Menu2();            //菜单2显示测试
        // printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        // OLED_Clear(0); 

        printf("main loop ......\r\n");
    }
}
```
* 步骤7  在主函数中调用，图片、文字显示功能都实现之后在oled_module.c文件的oled任务中去调用test_led_screen()即可实现图片、文字的显示。

```
void test_led_screen(void)
{   
    //delay_init();                //延时函数初始化      
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //设置NVIC中断分组2:2位抢占优先级，2位响应优先级  
    printf("before oled clear \r\n");
    OLED_Init();                     //初始化OLED  
    OLED_Clear(0);             //清屏（全黑）
    printf("after oled clear \r\n");
    while(1) 
    {   
        
        printf("before main page clear \r\n");
       
        // TEST_BMP();              //BMP单色图片显示测试
        // TEST_LINE();
        TEST_Chinese();          //中文显示测试
        printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        OLED_Clear(0); 
       
        // TEST_Menu2();            //菜单2显示测试
        // printf("test dbg %s,%s,%d \r\n",__FILE__,__func__,__LINE__);
        // OLED_Clear(0); 

        printf("main loop ......\r\n");
    }
}
```
* 步骤8  在主函数中调用，图片、文字显示功能都实现之后在oled_module.c文件的oled任务中去调用test_led_screen()即可实现图片、文字的显示。

```c
"21_KP_LCD_example:lcd_test",
```
### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过屏幕显示结果也可以通过串口助手查看具体日志。