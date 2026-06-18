/*
 * qihang_comfort_fan.c
 *
 * OpenHarmony 3.2 / Hi3861 / ISSedu Qihang KP-IOT
 * Personalized thermal-comfort simulated fan v6 with Wi-Fi and MQTT.
 *
 * Hardware mapping based on the repository examples:
 *   Fan PWM2 : GPIO2
 *   Button   : GPIO5, active low
 *   Status LED: GPIO6
 *   OLED RST : GPIO8
 *   OLED MOSI: GPIO9
 *   OLED CLK : GPIO10
 *   OLED DC  : GPIO11
 *   OLED CS  : GPIO12
 *   SHT30 SDA: GPIO13 (I2C0)
 *   SHT30 SCL: GPIO14 (I2C0)
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#include "ohos_init.h"
#include "hi_types_base.h"
#include "hi_task.h"
#include "hi_timer.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_i2c.h"
#include "hi_pwm.h"

#include "wifi_device.h"
#include "wifi_event.h"
#include "wifi_device_config.h"

#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
#include "lwip/netifapi.h"

#include "MQTTClient.h"
#include "network_config.h"

#define APP_TAG "[COMFORT_FAN]"
#define NET_TAG "[COMFORT_NET]"

#define SHT30_ADDR_WRITE       ((hi_u8)(0x44U << 1))
#define SHT30_ADDR_READ        ((hi_u8)((0x44U << 1) | 0x01U))
#define SHT30_SAMPLE_PERIOD_MS 1000U

#define FAN_PWM_PORT           HI_PWM_PORT_PWM2
#define FAN_PWM_CLOCK_HZ       160000000U
/* Match the repository motor example: approximately 2.442 kHz. */
#define FAN_PWM_FREQUENCY_HZ   2442U
#define FAN_PWM_PERIOD_COUNT   (FAN_PWM_CLOCK_HZ / FAN_PWM_FREQUENCY_HZ)

#define FAN_PWM_GPIO           HI_GPIO_IDX_2
#define BUTTON_GPIO            HI_GPIO_IDX_5
#define STATUS_LED_GPIO        HI_GPIO_IDX_6

#define OLED_RST_GPIO          HI_GPIO_IDX_8
#define OLED_MOSI_GPIO         HI_GPIO_IDX_9
#define OLED_CLK_GPIO          HI_GPIO_IDX_10
#define OLED_DC_GPIO           HI_GPIO_IDX_11
#define OLED_CS_GPIO           HI_GPIO_IDX_12

#define OLED_WIDTH             128U
#define OLED_HEIGHT            64U
#define OLED_PAGE_COUNT        (OLED_HEIGHT / 8U)
#define OLED_COLUMN_OFFSET     2U

#define TASK_STACK_SIZE        6144U
#define TASK_PRIORITY          25U
#define BUTTON_POLL_MS         50U
#define BUTTON_LONG_TICKS      30U

#define NETWORK_TASK_STACK_SIZE 12288U
#define NETWORK_TASK_PRIORITY   26U
#define MQTT_KEEPALIVE_SECONDS  30
#define MQTT_CONNECT_TIMEOUT    10
#define MQTT_PUBLISH_PERIOD_MS  1000U
#define NETWORK_RETRY_MS        5000U
#define WIFI_CONNECT_TIMEOUT_MS 30000U
#define WIFI_IP_TIMEOUT_MS      30000U

/* Physical motor defaults to low-duty hold and pulses on small speed changes. */
#define ACTUATOR_LOW_DUTY      1U
#define ACTUATOR_MEDIUM_DUTY   2U
#define ACTUATOR_HIGH_DUTY     3U
#define ACTUATOR_PULSE_MS      300U
#define ACTUATOR_PULSE_DELTA   5U
#define VIRTUAL_SPEED_STEP     5U

#define PREF_MIN              (-25)
#define PREF_MAX               25
#define PREF_STEP              5

typedef enum {
    FAN_MODE_OFF = 0,
    FAN_MODE_QUIET,
    FAN_MODE_AUTO
} FanMode;

typedef enum {
    FAN_LEVEL_OFF = 0,
    FAN_LEVEL_LOW,
    FAN_LEVEL_MEDIUM,
    FAN_LEVEL_HIGH
} FanLevel;

typedef struct {
    char ch;
    hi_u8 col[5];
} FontGlyph;

static const FontGlyph g_font[] = {
    {' ', {0x00,0x00,0x00,0x00,0x00}},
    {'%', {0x23,0x13,0x08,0x64,0x62}},
    {'+', {0x08,0x08,0x3E,0x08,0x08}},
    {'-', {0x08,0x08,0x08,0x08,0x08}},
    {'.', {0x00,0x60,0x60,0x00,0x00}},
    {':', {0x00,0x36,0x36,0x00,0x00}},
    {'?', {0x02,0x01,0x51,0x09,0x06}},
    {'0', {0x3E,0x51,0x49,0x45,0x3E}},
    {'1', {0x00,0x42,0x7F,0x40,0x00}},
    {'2', {0x42,0x61,0x51,0x49,0x46}},
    {'3', {0x21,0x41,0x45,0x4B,0x31}},
    {'4', {0x18,0x14,0x12,0x7F,0x10}},
    {'5', {0x27,0x45,0x45,0x45,0x39}},
    {'6', {0x3C,0x4A,0x49,0x49,0x30}},
    {'7', {0x01,0x71,0x09,0x05,0x03}},
    {'8', {0x36,0x49,0x49,0x49,0x36}},
    {'9', {0x06,0x49,0x49,0x29,0x1E}},
    {'A', {0x7E,0x11,0x11,0x11,0x7E}},
    {'B', {0x7F,0x49,0x49,0x49,0x36}},
    {'C', {0x3E,0x41,0x41,0x41,0x22}},
    {'D', {0x7F,0x41,0x41,0x22,0x1C}},
    {'E', {0x7F,0x49,0x49,0x49,0x41}},
    {'F', {0x7F,0x09,0x09,0x09,0x01}},
    {'G', {0x3E,0x41,0x49,0x49,0x7A}},
    {'H', {0x7F,0x08,0x08,0x08,0x7F}},
    {'I', {0x00,0x41,0x7F,0x41,0x00}},
    {'J', {0x20,0x40,0x41,0x3F,0x01}},
    {'K', {0x7F,0x08,0x14,0x22,0x41}},
    {'L', {0x7F,0x40,0x40,0x40,0x40}},
    {'M', {0x7F,0x02,0x0C,0x02,0x7F}},
    {'N', {0x7F,0x04,0x08,0x10,0x7F}},
    {'O', {0x3E,0x41,0x41,0x41,0x3E}},
    {'P', {0x7F,0x09,0x09,0x09,0x06}},
    {'Q', {0x3E,0x41,0x51,0x21,0x5E}},
    {'R', {0x7F,0x09,0x19,0x29,0x46}},
    {'S', {0x46,0x49,0x49,0x49,0x31}},
    {'T', {0x01,0x01,0x7F,0x01,0x01}},
    {'U', {0x3F,0x40,0x40,0x40,0x3F}},
    {'V', {0x1F,0x20,0x40,0x20,0x1F}},
    {'W', {0x3F,0x40,0x38,0x40,0x3F}},
    {'X', {0x63,0x14,0x08,0x14,0x63}},
    {'Y', {0x07,0x08,0x70,0x08,0x07}},
    {'Z', {0x61,0x51,0x49,0x45,0x43}},
};

static hi_u8 g_oled_buffer[OLED_WIDTH * OLED_PAGE_COUNT];
static hi_u32 g_task_id = 0;
static hi_u32 g_network_task_id = 0;

static volatile int g_preference_bias = 0;
static volatile FanMode g_fan_mode = FAN_MODE_OFF;
static volatile unsigned int g_virtual_speed = 0U;
static volatile int g_motor_hold_enabled = 1;
static volatile FanLevel g_virtual_level = FAN_LEVEL_OFF;
static volatile unsigned int g_last_pulse_speed = 0U;
static unsigned long long g_energy_used = 0;
static unsigned long long g_energy_baseline = 0;

static volatile int g_sensor_ok = 0;
static volatile int g_temperature_x10 = 0;
static volatile int g_humidity_x10 = 0;
static volatile unsigned int g_last_saving = 0U;
static volatile int g_wifi_connected = 0;
static volatile int g_mqtt_connected = 0;
static volatile int g_publish_now = 0;

static int g_wifi_net_id = -1;
static int g_wifi_event_registered = 0;
static int g_wifi_enabled = 0;
static struct netif *g_wifi_netif = NULL;
static MQTTClient g_mqtt_client = NULL;

static char g_state_topic[128];
static char g_command_topic[128];
static char g_event_topic[128];

static void GpioWrite(hi_gpio_idx gpio, hi_gpio_value value)
{
    (void)hi_gpio_set_ouput_val(gpio, value);
}

static void OledDelay(void)
{
    hi_udelay(1);
}

static void OledWriteByte(hi_u8 value, hi_bool isData)
{
    int bit;

    GpioWrite(OLED_CS_GPIO, HI_GPIO_VALUE0);
    GpioWrite(OLED_DC_GPIO, isData ? HI_GPIO_VALUE1 : HI_GPIO_VALUE0);

    for (bit = 0; bit < 8; ++bit) {
        GpioWrite(OLED_CLK_GPIO, HI_GPIO_VALUE0);
        GpioWrite(OLED_MOSI_GPIO,
            (value & 0x80U) ? HI_GPIO_VALUE1 : HI_GPIO_VALUE0);
        OledDelay();
        GpioWrite(OLED_CLK_GPIO, HI_GPIO_VALUE1);
        OledDelay();
        value <<= 1;
    }

    GpioWrite(OLED_CLK_GPIO, HI_GPIO_VALUE0);
    GpioWrite(OLED_CS_GPIO, HI_GPIO_VALUE1);
}

static void OledCommand(hi_u8 command)
{
    OledWriteByte(command, HI_FALSE);
}

static void OledData(hi_u8 data)
{
    OledWriteByte(data, HI_TRUE);
}

static void OledGpioInit(void)
{
    (void)hi_io_set_func(HI_IO_NAME_GPIO_8, HI_IO_FUNC_GPIO_8_GPIO);
    (void)hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO);
    (void)hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_GPIO);
    (void)hi_io_set_func(HI_IO_NAME_GPIO_11, HI_IO_FUNC_GPIO_11_GPIO);
    (void)hi_io_set_func(HI_IO_NAME_GPIO_12, HI_IO_FUNC_GPIO_12_GPIO);

    (void)hi_gpio_set_dir(OLED_RST_GPIO, HI_GPIO_DIR_OUT);
    (void)hi_gpio_set_dir(OLED_MOSI_GPIO, HI_GPIO_DIR_OUT);
    (void)hi_gpio_set_dir(OLED_CLK_GPIO, HI_GPIO_DIR_OUT);
    (void)hi_gpio_set_dir(OLED_DC_GPIO, HI_GPIO_DIR_OUT);
    (void)hi_gpio_set_dir(OLED_CS_GPIO, HI_GPIO_DIR_OUT);

    GpioWrite(OLED_CS_GPIO, HI_GPIO_VALUE1);
    GpioWrite(OLED_CLK_GPIO, HI_GPIO_VALUE0);
    GpioWrite(OLED_MOSI_GPIO, HI_GPIO_VALUE0);
    GpioWrite(OLED_DC_GPIO, HI_GPIO_VALUE0);

    GpioWrite(OLED_RST_GPIO, HI_GPIO_VALUE0);
    hi_sleep(20);
    GpioWrite(OLED_RST_GPIO, HI_GPIO_VALUE1);
    hi_sleep(20);
}

static void OledInit(void)
{
    OledGpioInit();

    OledCommand(0xAE);
    OledCommand(0xD5);
    OledCommand(0x80);
    OledCommand(0xA8);
    OledCommand(0x3F);
    OledCommand(0xD3);
    OledCommand(0x00);
    OledCommand(0x40);
    OledCommand(0xAD);
    OledCommand(0x8B);
    OledCommand(0xA1);
    OledCommand(0xC8);
    OledCommand(0xDA);
    OledCommand(0x12);
    OledCommand(0x81);
    OledCommand(0x80);
    OledCommand(0xD9);
    OledCommand(0x22);
    OledCommand(0xDB);
    OledCommand(0x35);
    OledCommand(0xA4);
    OledCommand(0xA6);
    OledCommand(0xAF);

    memset(g_oled_buffer, 0, sizeof(g_oled_buffer));
}

static const hi_u8 *FindGlyph(char ch)
{
    unsigned int i;

    if (ch >= 'a' && ch <= 'z') {
        ch = (char)(ch - 'a' + 'A');
    }

    for (i = 0; i < (sizeof(g_font) / sizeof(g_font[0])); ++i) {
        if (g_font[i].ch == ch) {
            return g_font[i].col;
        }
    }

    for (i = 0; i < (sizeof(g_font) / sizeof(g_font[0])); ++i) {
        if (g_font[i].ch == '?') {
            return g_font[i].col;
        }
    }

    return g_font[0].col;
}

static void OledClearBuffer(void)
{
    memset(g_oled_buffer, 0, sizeof(g_oled_buffer));
}

static void OledDrawChar(unsigned int x, unsigned int page, char ch)
{
    const hi_u8 *glyph;
    unsigned int i;
    unsigned int index;

    if (x + 5U >= OLED_WIDTH || page >= OLED_PAGE_COUNT) {
        return;
    }

    glyph = FindGlyph(ch);
    index = page * OLED_WIDTH + x;

    for (i = 0; i < 5U; ++i) {
        g_oled_buffer[index + i] = glyph[i];
    }
    g_oled_buffer[index + 5U] = 0x00;
}

static void OledDrawString(unsigned int x, unsigned int page, const char *text)
{
    while (text != NULL && *text != '\0' && x + 5U < OLED_WIDTH) {
        OledDrawChar(x, page, *text);
        x += 6U;
        ++text;
    }
}

static void OledFlush(void)
{
    unsigned int page;
    unsigned int column;

    for (page = 0; page < OLED_PAGE_COUNT; ++page) {
        OledCommand((hi_u8)(0xB0U + page));
        OledCommand((hi_u8)(0x00U + (OLED_COLUMN_OFFSET & 0x0FU)));
        OledCommand((hi_u8)(0x10U + ((OLED_COLUMN_OFFSET >> 4) & 0x0FU)));

        for (column = 0; column < OLED_WIDTH; ++column) {
            OledData(g_oled_buffer[page * OLED_WIDTH + column]);
        }
    }
}

static hi_u8 Sht30Crc(const hi_u8 *data, unsigned int length)
{
    hi_u8 crc = 0xFFU;
    unsigned int i;
    unsigned int bit;

    for (i = 0; i < length; ++i) {
        crc ^= data[i];
        for (bit = 0; bit < 8U; ++bit) {
            if ((crc & 0x80U) != 0U) {
                crc = (hi_u8)((crc << 1) ^ 0x31U);
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

static int Sht30Init(void)
{
    hi_u32 ret;

    (void)hi_io_set_func(HI_IO_NAME_GPIO_13, HI_IO_FUNC_GPIO_13_I2C0_SDA);
    (void)hi_io_set_func(HI_IO_NAME_GPIO_14, HI_IO_FUNC_GPIO_14_I2C0_SCL);
    (void)hi_io_set_pull(HI_IO_NAME_GPIO_13, HI_IO_PULL_UP);
    (void)hi_io_set_pull(HI_IO_NAME_GPIO_14, HI_IO_PULL_UP);

    (void)hi_i2c_deinit(HI_I2C_IDX_0);
    ret = hi_i2c_init(HI_I2C_IDX_0, 100000U);
    if (ret != HI_ERR_SUCCESS) {
        printf(APP_TAG " i2c init failed:0x%x\r\n", ret);
        return -1;
    }
    return 0;
}

static int Sht30Read(int *temperatureX10, int *humidityX10)
{
    hi_u8 command[2] = {0x24U, 0x00U};
    hi_u8 receive[6] = {0};
    hi_i2c_data data;
    hi_u32 ret;
    hi_u16 rawTemperature;
    hi_u16 rawHumidity;

    if (temperatureX10 == NULL || humidityX10 == NULL) {
        return -1;
    }

    memset(&data, 0, sizeof(data));
    data.send_buf = command;
    data.send_len = sizeof(command);
    ret = hi_i2c_write(HI_I2C_IDX_0, SHT30_ADDR_WRITE, &data);
    if (ret != HI_ERR_SUCCESS) {
        return -2;
    }

    hi_sleep(20);

    memset(&data, 0, sizeof(data));
    data.receive_buf = receive;
    data.receive_len = sizeof(receive);
    ret = hi_i2c_read(HI_I2C_IDX_0, SHT30_ADDR_READ, &data);
    if (ret != HI_ERR_SUCCESS) {
        return -3;
    }

    if (Sht30Crc(receive, 2U) != receive[2] ||
        Sht30Crc(&receive[3], 2U) != receive[5]) {
        return -4;
    }

    rawTemperature = (hi_u16)(((hi_u16)receive[0] << 8) | receive[1]);
    rawHumidity = (hi_u16)(((hi_u16)receive[3] << 8) | receive[4]);

    *temperatureX10 =
        (int)(((int64_t)1750 * rawTemperature) / 65535) - 450;
    *humidityX10 =
        (int)(((int64_t)1000 * rawHumidity) / 65535);

    return 0;
}

static int FanInit(void)
{
    hi_u32 ret;

    (void)hi_io_set_func(HI_IO_NAME_GPIO_2, HI_IO_FUNC_GPIO_2_PWM2_OUT);

    (void)hi_pwm_deinit(FAN_PWM_PORT);
    ret = hi_pwm_init(FAN_PWM_PORT);
    if (ret != HI_ERR_SUCCESS) {
        printf(APP_TAG " pwm init failed:0x%x\r\n", ret);
        return -1;
    }

    ret = hi_pwm_set_clock(PWM_CLK_160M);
    if (ret != HI_ERR_SUCCESS) {
        printf(APP_TAG " pwm clock failed:0x%x\r\n", ret);
        return -2;
    }

    (void)hi_pwm_stop(FAN_PWM_PORT);

    (void)hi_io_set_func(HI_IO_NAME_GPIO_6, HI_IO_FUNC_GPIO_6_GPIO);
    (void)hi_gpio_set_dir(STATUS_LED_GPIO, HI_GPIO_DIR_OUT);
    GpioWrite(STATUS_LED_GPIO, HI_GPIO_VALUE0);

    return 0;
}


static void MotorStop(void)
{
    (void)hi_pwm_stop(FAN_PWM_PORT);
    GpioWrite(STATUS_LED_GPIO, HI_GPIO_VALUE0);
}

static int MotorSetDuty(unsigned int percent)
{
    hi_u32 ret;
    hi_u16 dutyCount;

    if (percent == 0U) {
        MotorStop();
        return 0;
    }
    if (percent > 100U) {
        percent = 100U;
    }

    dutyCount = (hi_u16)((percent * FAN_PWM_PERIOD_COUNT) / 100U);
    if (dutyCount == 0U) {
        dutyCount = 1U;
    }

    ret = hi_pwm_start(FAN_PWM_PORT, dutyCount,
        (hi_u16)FAN_PWM_PERIOD_COUNT);
    if (ret != HI_ERR_SUCCESS) {
        printf(APP_TAG " pwm start failed:0x%x\r\n", ret);
        MotorStop();
        return -1;
    }

    GpioWrite(STATUS_LED_GPIO, HI_GPIO_VALUE1);
    return 0;
}

static const char *FanLevelName(FanLevel level)
{
    switch (level) {
        case FAN_LEVEL_LOW:
            return "LOW";
        case FAN_LEVEL_MEDIUM:
            return "MEDIUM";
        case FAN_LEVEL_HIGH:
            return "HIGH";
        case FAN_LEVEL_OFF:
        default:
            return "OFF";
    }
}

static FanLevel MapVirtualLevel(unsigned int virtualSpeed)
{
    if (virtualSpeed == 0U) {
        return FAN_LEVEL_OFF;
    }
    if (virtualSpeed <= 35U) {
        return FAN_LEVEL_LOW;
    }
    if (virtualSpeed <= 60U) {
        return FAN_LEVEL_MEDIUM;
    }
    return FAN_LEVEL_HIGH;
}

static unsigned int LevelDuty(FanLevel level)
{
    switch (level) {
        case FAN_LEVEL_LOW:
            return ACTUATOR_LOW_DUTY;
        case FAN_LEVEL_MEDIUM:
            return ACTUATOR_MEDIUM_DUTY;
        case FAN_LEVEL_HIGH:
            return ACTUATOR_HIGH_DUTY;
        case FAN_LEVEL_OFF:
        default:
            return 0U;
    }
}

static void ApplyMotorPolicy(unsigned int virtualSpeed);

static void ActuatorPulse(FanLevel level, unsigned int virtualSpeed)
{
    unsigned int duty;

    MotorStop();
    if (level == FAN_LEVEL_OFF) {
        printf(APP_TAG " actuator=OFF; virtual fan=0%%\r\n");
        return;
    }

    duty = LevelDuty(level);
    printf(APP_TAG
        " actuator pulse: %s duty=%u%% duration=%ums\r\n",
        FanLevelName(level), duty, ACTUATOR_PULSE_MS);

    if (MotorSetDuty(duty) == 0) {
        hi_sleep(ACTUATOR_PULSE_MS);
    }
    MotorStop();

    printf(APP_TAG
        " actuator stopped; virtual fan remains %u%%\r\n",
        virtualSpeed);
}

static int ShouldPulseFeedback(FanLevel previousLevel, FanLevel nextLevel,
    unsigned int previousPulseSpeed, unsigned int nextSpeed)
{
    unsigned int delta;

    if (nextLevel != previousLevel) {
        return 1;
    }
    if (nextSpeed == 0U) {
        return previousPulseSpeed != 0U;
    }
    if (previousPulseSpeed == 0U) {
        return 1;
    }

    delta = nextSpeed > previousPulseSpeed ?
        nextSpeed - previousPulseSpeed : previousPulseSpeed - nextSpeed;
    return delta >= ACTUATOR_PULSE_DELTA;
}

static void ApplyMotorPolicy(unsigned int virtualSpeed)
{
    if (virtualSpeed == 0U || g_motor_hold_enabled == 0) {
        MotorStop();
        return;
    }

    (void)MotorSetDuty(ACTUATOR_LOW_DUTY);
}


static void ButtonInit(void)
{
    (void)hi_io_set_func(HI_IO_NAME_GPIO_5, HI_IO_FUNC_GPIO_5_GPIO);
    (void)hi_io_set_pull(HI_IO_NAME_GPIO_5, HI_IO_PULL_UP);
    (void)hi_gpio_set_dir(BUTTON_GPIO, HI_GPIO_DIR_IN);
}

static const char *FanModeName(FanMode mode)
{
    switch (mode) {
        case FAN_MODE_QUIET:
            return "QUIET";
        case FAN_MODE_AUTO:
            return "AUTO";
        case FAN_MODE_OFF:
        default:
            return "OFF";
    }
}

static void CycleFanMode(void)
{
    if (g_fan_mode == FAN_MODE_OFF) {
        g_fan_mode = FAN_MODE_QUIET;
    } else if (g_fan_mode == FAN_MODE_QUIET) {
        g_fan_mode = FAN_MODE_AUTO;
    } else {
        g_fan_mode = FAN_MODE_OFF;
        g_virtual_speed = 0U;
        g_virtual_level = FAN_LEVEL_OFF;
        MotorStop();
    }

    printf(APP_TAG " mode=%s\r\n", FanModeName(g_fan_mode));
}


static void ToggleEcoPreference(void)
{
    g_preference_bias = (g_preference_bias == 0) ? -10 : 0;
    printf(APP_TAG " eco preference=%d\r\n", g_preference_bias);
}

static void PollButton(void)
{
    static hi_bool wasPressed = HI_FALSE;
    static unsigned int heldTicks = 0U;
    hi_gpio_value value = HI_GPIO_VALUE1;

    if (hi_gpio_get_input_val(BUTTON_GPIO, &value) != HI_ERR_SUCCESS) {
        return;
    }

    if (value == HI_GPIO_VALUE0) {
        if (heldTicks < 200U) {
            ++heldTicks;
        }
        wasPressed = HI_TRUE;
        return;
    }

    if (wasPressed == HI_TRUE) {
        if (heldTicks >= BUTTON_LONG_TICKS) {
            ToggleEcoPreference();
        } else if (heldTicks >= 2U) {
            CycleFanMode();
        }
    }

    wasPressed = HI_FALSE;
    heldTicks = 0U;
}

static unsigned int CalculateTargetSpeed(int temperatureX10, int humidityX10)
{
    int target;

    /*
     * Virtual fan curve for classroom demonstration.
     * It produces useful state changes near ordinary room temperature.
     */
    if (temperatureX10 < 260) {
        target = 0;
    } else if (temperatureX10 < 275) {
        target = 30;
    } else if (temperatureX10 < 290) {
        target = 45;
    } else if (temperatureX10 < 310) {
        target = 60;
    } else {
        target = 75;
    }

    if (humidityX10 > 750) {
        target += 10;
    } else if (humidityX10 > 650) {
        target += 5;
    }

    target += g_preference_bias;

    if (target < 0) {
        target = 0;
    }
    if (target > 100) {
        target = 100;
    }

    return (unsigned int)target;
}


static unsigned int SmoothSpeed(unsigned int current, unsigned int target)
{
    if (target > current + VIRTUAL_SPEED_STEP) {
        return current + VIRTUAL_SPEED_STEP;
    }
    if (current > target + VIRTUAL_SPEED_STEP) {
        return current - VIRTUAL_SPEED_STEP;
    }
    return target;
}


static void FormatFixed1(char *buffer, unsigned int length, int valueX10)
{
    int absoluteValue;

    if (buffer == NULL || length == 0U) {
        return;
    }

    absoluteValue = valueX10;
    if (absoluteValue < 0) {
        absoluteValue = -absoluteValue;
        (void)snprintf(buffer, length, "-%d.%d",
            absoluteValue / 10, absoluteValue % 10);
    } else {
        (void)snprintf(buffer, length, "%d.%d",
            absoluteValue / 10, absoluteValue % 10);
    }
}

static unsigned int CalculateSaving(void)
{
    unsigned long long usedPercent;

    if (g_energy_baseline == 0ULL) {
        return 0U;
    }

    usedPercent = (g_energy_used * 100ULL) / g_energy_baseline;
    if (usedPercent >= 100ULL) {
        return 0U;
    }
    return (unsigned int)(100ULL - usedPercent);
}

static void UpdateEnergyIndex(unsigned int speed)
{
    unsigned long long s = speed;

    /*
     * Relative index, not a calibrated watt-hour meter.
     * Fan power is approximated by speed^3 for comparative testing.
     */
    g_energy_used += s * s * s;
    g_energy_baseline += 100ULL * 100ULL * 100ULL;
}


static void CopyCString(char *destination, size_t destinationSize,
    const char *source)
{
    size_t length;

    if (destination == NULL || destinationSize == 0U) {
        return;
    }

    destination[0] = '\0';
    if (source == NULL) {
        return;
    }

    length = strlen(source);
    if (length >= destinationSize) {
        length = destinationSize - 1U;
    }

    (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static void WifiConnectionChanged(int state, WifiLinkedInfo *info)
{
    (void)info;

    if (state == WIFI_STATE_AVAILABLE) {
        g_wifi_connected = 1;
        printf(NET_TAG " Wi-Fi link connected\r\n");
    } else {
        g_wifi_connected = 0;
        g_mqtt_connected = 0;
        printf(NET_TAG " Wi-Fi link disconnected\r\n");
    }
}

static void WifiScanStateChanged(int state, int size)
{
    (void)state;
    (void)size;
}

static WifiEvent g_wifi_event = {
    .OnWifiConnectionChanged = WifiConnectionChanged,
    .OnWifiScanStateChanged = WifiScanStateChanged,
    .OnHotspotStateChanged = NULL,
    .OnHotspotStaJoin = NULL,
    .OnHotspotStaLeave = NULL
};

static int WaitForWifiIp(unsigned int timeoutMs)
{
    unsigned int elapsed = 0U;
    ip4_addr_t ipAddress;
    ip4_addr_t netmask;
    ip4_addr_t gateway;

    g_wifi_netif = netifapi_netif_find("wlan0");
    while (g_wifi_netif == NULL && elapsed < timeoutMs) {
        hi_sleep(200U);
        elapsed += 200U;
        g_wifi_netif = netifapi_netif_find("wlan0");
    }

    if (g_wifi_netif == NULL) {
        printf(NET_TAG " wlan0 not found\r\n");
        return -1;
    }

    (void)netifapi_dhcp_start(g_wifi_netif);
    elapsed = 0U;

    while (elapsed < timeoutMs) {
        (void)netifapi_netif_get_addr(g_wifi_netif,
            &ipAddress, &netmask, &gateway);

        if (!ip4_addr_isany_val(ipAddress)) {
            printf(NET_TAG " DHCP ready, IP=%s\r\n",
                ip4addr_ntoa(&ipAddress));
            return 0;
        }

        hi_sleep(250U);
        elapsed += 250U;
    }

    printf(NET_TAG " DHCP timeout\r\n");
    return -1;
}

static int ConnectWifi(void)
{
    WifiDeviceConfig configuration;
    WifiErrorCode error;
    unsigned int elapsed = 0U;

    if (g_wifi_connected) {
        return WaitForWifiIp(WIFI_IP_TIMEOUT_MS);
    }

    memset(&configuration, 0, sizeof(configuration));
    CopyCString(configuration.ssid, sizeof(configuration.ssid), CF_WIFI_SSID);
    CopyCString(configuration.preSharedKey,
        sizeof(configuration.preSharedKey), CF_WIFI_PASSWORD);
    configuration.securityType =
        CF_WIFI_PASSWORD[0] == '\0' ? WIFI_SEC_TYPE_OPEN : WIFI_SEC_TYPE_PSK;

    if (!g_wifi_event_registered) {
        error = RegisterWifiEvent(&g_wifi_event);
        if (error != WIFI_SUCCESS) {
            printf(NET_TAG " RegisterWifiEvent failed:%d\r\n", error);
            return -1;
        }
        g_wifi_event_registered = 1;
    }

    if (!g_wifi_enabled) {
        error = EnableWifi();
        if (error != WIFI_SUCCESS) {
            printf(NET_TAG " EnableWifi failed:%d\r\n", error);
            return -1;
        }
        g_wifi_enabled = 1;
    }

    if (g_wifi_net_id < 0) {
        error = AddDeviceConfig(&configuration, &g_wifi_net_id);
        if (error != WIFI_SUCCESS) {
            printf(NET_TAG " AddDeviceConfig failed:%d\r\n", error);
            return -1;
        }
    }

    printf(NET_TAG " connecting SSID=%s netId=%d\r\n",
        CF_WIFI_SSID, g_wifi_net_id);

    error = ConnectTo(g_wifi_net_id);
    if (error != WIFI_SUCCESS) {
        printf(NET_TAG " ConnectTo failed:%d\r\n", error);
        return -1;
    }

    while (!g_wifi_connected && elapsed < WIFI_CONNECT_TIMEOUT_MS) {
        hi_sleep(200U);
        elapsed += 200U;
    }

    if (!g_wifi_connected) {
        printf(NET_TAG " Wi-Fi connection timeout\r\n");
        return -1;
    }

    return WaitForWifiIp(WIFI_IP_TIMEOUT_MS);
}

static void MqttConnectionLost(void *context, char *cause)
{
    (void)context;
    g_mqtt_connected = 0;
    printf(NET_TAG " MQTT connection lost:%s\r\n",
        cause == NULL ? "unknown" : cause);
}

static void NormalizeCommand(char *command)
{
    size_t readIndex;
    size_t writeIndex = 0U;

    if (command == NULL) {
        return;
    }

    for (readIndex = 0U; command[readIndex] != '\0'; ++readIndex) {
        unsigned char value = (unsigned char)command[readIndex];

        if (value == ' ' || value == '\r' || value == '\n' ||
            value == '\t' || value == '"' || value == '{' ||
            value == '}' || value == ':') {
            continue;
        }

        command[writeIndex++] = (char)toupper(value);
    }

    command[writeIndex] = '\0';
}

static void SetRemoteMode(FanMode mode)
{
    g_fan_mode = mode;

    if (mode == FAN_MODE_OFF) {
        g_virtual_speed = 0U;
        g_virtual_level = FAN_LEVEL_OFF;
        MotorStop();
    }

    printf(NET_TAG " remote mode=%s\r\n", FanModeName(mode));
}

static void ApplyRemoteCommand(const char *payload)
{
    char command[96];
    int nextPreference;

    CopyCString(command, sizeof(command), payload);
    NormalizeCommand(command);

    printf(NET_TAG " command=%s\r\n", command);

    if (strstr(command, "ECO_OFF") != NULL ||
        strstr(command, "ECO=OFF") != NULL ||
        strstr(command, "ECOOFF") != NULL ||
        strstr(command, "ECO0") != NULL) {
        g_preference_bias = 0;
        printf(NET_TAG " remote eco=OFF\r\n");
    } else if (strstr(command, "ECO_ON") != NULL ||
        strstr(command, "ECO=ON") != NULL ||
        strstr(command, "ECOON") != NULL ||
        strstr(command, "ECO1") != NULL) {
        g_preference_bias = -10;
        printf(NET_TAG " remote eco=ON\r\n");
    } else if (strstr(command, "MOTOR_ON") != NULL ||
        strstr(command, "MOTOR=ON") != NULL ||
        strstr(command, "MOTORON") != NULL) {
        g_motor_hold_enabled = 1;
        ApplyMotorPolicy(g_virtual_speed);
        printf(NET_TAG " remote motor hold=ON\r\n");
    } else if (strstr(command, "MOTOR_OFF") != NULL ||
        strstr(command, "MOTOR=OFF") != NULL ||
        strstr(command, "MOTOROFF") != NULL) {
        g_motor_hold_enabled = 0;
        MotorStop();
        printf(NET_TAG " remote motor hold=OFF\r\n");
    } else if (strstr(command, "QUIET") != NULL) {
        SetRemoteMode(FAN_MODE_QUIET);
    } else if (strstr(command, "AUTO") != NULL) {
        SetRemoteMode(FAN_MODE_AUTO);
    } else if (strstr(command, "OFF") != NULL) {
        SetRemoteMode(FAN_MODE_OFF);
    } else if (strstr(command, "MORE") != NULL) {
        nextPreference = g_preference_bias + PREF_STEP;
        if (nextPreference > PREF_MAX) {
            nextPreference = PREF_MAX;
        }
        g_preference_bias = nextPreference;
        printf(NET_TAG " remote preference=%d\r\n", g_preference_bias);
    } else if (strstr(command, "LESS") != NULL) {
        nextPreference = g_preference_bias - PREF_STEP;
        if (nextPreference < PREF_MIN) {
            nextPreference = PREF_MIN;
        }
        g_preference_bias = nextPreference;
        printf(NET_TAG " remote preference=%d\r\n", g_preference_bias);
    } else if (strstr(command, "STATUS") == NULL) {
        printf(NET_TAG
            " unknown command; use OFF/QUIET/AUTO/ECO_ON/ECO_OFF/MORE/LESS/MOTOR_ON/MOTOR_OFF/STATUS\r\n");
    }

    g_publish_now = 1;
}

static int MqttMessageArrived(void *context, char *topicName,
    int topicLength, MQTTClient_message *message)
{
    char payload[96];
    int copyLength;

    (void)context;
    (void)topicLength;

    if (message == NULL) {
        return 1;
    }

    copyLength = message->payloadlen;
    if (copyLength < 0) {
        copyLength = 0;
    }
    if ((size_t)copyLength >= sizeof(payload)) {
        copyLength = (int)sizeof(payload) - 1;
    }

    if (copyLength > 0 && message->payload != NULL) {
        (void)memcpy(payload, message->payload, (size_t)copyLength);
    }
    payload[copyLength] = '\0';

    printf(NET_TAG " RX topic=%s payload=%s\r\n",
        topicName == NULL ? "" : topicName, payload);
    ApplyRemoteCommand(payload);

    MQTTClient_freeMessage(&message);
    if (topicName != NULL) {
        MQTTClient_free(topicName);
    }
    return 1;
}

static void BuildMqttTopics(void)
{
    (void)snprintf(g_state_topic, sizeof(g_state_topic),
        "comfortfan/%s/state", CF_DEVICE_ID);
    (void)snprintf(g_command_topic, sizeof(g_command_topic),
        "comfortfan/%s/cmd", CF_DEVICE_ID);
    (void)snprintf(g_event_topic, sizeof(g_event_topic),
        "comfortfan/%s/event", CF_DEVICE_ID);
}

static void DisconnectMqtt(void)
{
    if (g_mqtt_client != NULL) {
        if (MQTTClient_isConnected(g_mqtt_client)) {
            (void)MQTTClient_disconnect(g_mqtt_client, 1000);
        }
        MQTTClient_destroy(&g_mqtt_client);
        g_mqtt_client = NULL;
    }

    g_mqtt_connected = 0;
}

static int PublishText(const char *topic, const char *text, int retained);

static int ConnectMqtt(void)
{
    MQTTClient_connectOptions options =
        MQTTClient_connectOptions_initializer;
    int result;

    DisconnectMqtt();

    result = MQTTClient_create(&g_mqtt_client, CF_MQTT_URI,
        CF_DEVICE_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (result != MQTTCLIENT_SUCCESS) {
        printf(NET_TAG " MQTTClient_create failed:%d\r\n", result);
        g_mqtt_client = NULL;
        return -1;
    }

    result = MQTTClient_setCallbacks(g_mqtt_client, NULL,
        MqttConnectionLost, MqttMessageArrived, NULL);
    if (result != MQTTCLIENT_SUCCESS) {
        printf(NET_TAG " MQTTClient_setCallbacks failed:%d\r\n", result);
        DisconnectMqtt();
        return -1;
    }

    options.keepAliveInterval = MQTT_KEEPALIVE_SECONDS;
    options.cleansession = 1;
    options.connectTimeout = 5;
    options.MQTTVersion = MQTTVERSION_3_1_1;
    if (CF_MQTT_USERNAME[0] != '\0') {
        options.username = CF_MQTT_USERNAME;
    }
    if (CF_MQTT_PASSWORD[0] != '\0') {
        options.password = CF_MQTT_PASSWORD;
    }

    printf(NET_TAG " connecting MQTT=%s client=%s\r\n",
        CF_MQTT_URI, CF_DEVICE_ID);

    result = MQTTClient_connect(g_mqtt_client, &options);
    if (result != MQTTCLIENT_SUCCESS) {
        printf(NET_TAG " MQTT connect failed:%d\r\n", result);
        DisconnectMqtt();
        return -1;
    }

    result = MQTTClient_subscribe(g_mqtt_client, g_command_topic, 1);
    if (result != MQTTCLIENT_SUCCESS) {
        printf(NET_TAG " MQTT subscribe failed:%d\r\n", result);
        DisconnectMqtt();
        return -1;
    }

    g_mqtt_connected = 1;
    g_publish_now = 1;
    printf(NET_TAG " MQTT connected; subscribed=%s\r\n",
        g_command_topic);
    (void)PublishText(g_event_topic, "online", 1);
    return 0;
}

static int PublishText(const char *topic, const char *text, int retained)
{
    MQTTClient_message message = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int result;

    if (!g_mqtt_connected || g_mqtt_client == NULL ||
        !MQTTClient_isConnected(g_mqtt_client)) {
        return -1;
    }

    message.payload = (void *)text;
    message.payloadlen = (int)strlen(text);
    message.qos = 0;
    message.retained = retained;

    result = MQTTClient_publishMessage(g_mqtt_client,
        topic, &message, &token);
    if (result != MQTTCLIENT_SUCCESS) {
        printf(NET_TAG " publish failed:%d\r\n", result);
        g_mqtt_connected = 0;
        return -1;
    }

    return 0;
}

static void PublishState(void)
{
    char json[320];
    int temperature = g_temperature_x10;
    int humidity = g_humidity_x10;
    unsigned int speed = g_virtual_speed;
    FanMode mode = g_fan_mode;
    FanLevel level = g_virtual_level;
    int temperatureFraction =
        temperature < 0 ? -(temperature % 10) : temperature % 10;

    if (!g_sensor_ok) {
        (void)snprintf(json, sizeof(json),
            "{\"device\":\"%s\",\"sensor_ok\":0,"
            "\"mode\":\"%s\",\"virtual_speed\":%u,"
            "\"level\":\"%s\",\"eco\":%d,\"motor_hold\":%d,"
            "\"save\":%u,\"wifi\":%d,\"mqtt\":%d}",
            CF_DEVICE_ID,
            FanModeName(mode),
            speed,
            FanLevelName(level),
            g_preference_bias < 0 ? 1 : 0,
            g_motor_hold_enabled ? 1 : 0,
            g_last_saving,
            g_wifi_connected ? 1 : 0,
            g_mqtt_connected ? 1 : 0);
    } else {
        (void)snprintf(json, sizeof(json),
            "{\"device\":\"%s\",\"sensor_ok\":1,"
            "\"temperature\":%d.%d,\"humidity\":%d.%d,"
            "\"mode\":\"%s\",\"virtual_speed\":%u,"
            "\"level\":\"%s\",\"preference\":%d,"
            "\"eco\":%d,\"motor_hold\":%d,\"save\":%u,"
            "\"wifi\":%d,\"mqtt\":%d}",
            CF_DEVICE_ID,
            temperature / 10,
            temperatureFraction,
            humidity / 10,
            humidity % 10,
            FanModeName(mode),
            speed,
            FanLevelName(level),
            g_preference_bias,
            g_preference_bias < 0 ? 1 : 0,
            g_motor_hold_enabled ? 1 : 0,
            g_last_saving,
            g_wifi_connected ? 1 : 0,
            g_mqtt_connected ? 1 : 0);
    }

    if (PublishText(g_state_topic, json, 1) == 0) {
        printf(NET_TAG " TX %s\r\n", json);
    }
}

static hi_void *ComfortNetworkTask(hi_void *parameter)
{
    unsigned int publishElapsed = MQTT_PUBLISH_PERIOD_MS;

    (void)parameter;

    BuildMqttTopics();
    printf(NET_TAG " network task started\r\n");
    printf(NET_TAG " state topic=%s\r\n", g_state_topic);
    printf(NET_TAG " command topic=%s\r\n", g_command_topic);

    while (1) {
        if (!g_wifi_connected) {
            DisconnectMqtt();
            if (ConnectWifi() != 0) {
                hi_sleep(NETWORK_RETRY_MS);
                continue;
            }
        }

        if (!g_mqtt_connected || g_mqtt_client == NULL ||
            !MQTTClient_isConnected(g_mqtt_client)) {
            if (ConnectMqtt() != 0) {
                hi_sleep(NETWORK_RETRY_MS);
                continue;
            }
        }

        MQTTClient_yield();

        if (g_publish_now ||
            publishElapsed >= MQTT_PUBLISH_PERIOD_MS) {
            g_publish_now = 0;
            publishElapsed = 0U;
            PublishState();
        }

        hi_sleep(100U);
        publishElapsed += 100U;
    }

    return HI_NULL;
}

static void UpdateDisplay(int sensorOk, int temperatureX10,
    int humidityX10, unsigned int virtualSpeed, FanMode mode,
    FanLevel level)
{
    char temperature[12];
    char humidity[12];
    char line[32];
    unsigned int saving;

    OledClearBuffer();
    (void)snprintf(line, sizeof(line), "OH FAN W:%d M:%d",
        g_wifi_connected ? 1 : 0,
        g_mqtt_connected ? 1 : 0);
    OledDrawString(0U, 0U, line);

    if (!sensorOk) {
        OledDrawString(0U, 2U, "SHT30 ERROR");
        OledDrawString(0U, 4U, "CHECK I2C");
        OledDrawString(0U, 6U, "VIRTUAL:000%");
        OledFlush();
        return;
    }

    FormatFixed1(temperature, sizeof(temperature), temperatureX10);
    FormatFixed1(humidity, sizeof(humidity), humidityX10);

    (void)snprintf(line, sizeof(line), "T:%sC H:%s%%",
        temperature, humidity);
    OledDrawString(0U, 2U, line);

    (void)snprintf(line, sizeof(line), "%s V:%03u%% %s",
        FanModeName(mode), virtualSpeed, FanLevelName(level));
    OledDrawString(0U, 4U, line);

    saving = CalculateSaving();
    (void)snprintf(line, sizeof(line), "ECO:%s SAVE:%02u%%",
        g_preference_bias < 0 ? "ON" : "OFF", saving);
    OledDrawString(0U, 6U, line);

    OledFlush();
}


static hi_void *ComfortFanTask(hi_void *parameter)
{
    unsigned int tick = 0U;
    unsigned int targetSpeed = 0U;
    unsigned int nextSpeed = 0U;
    FanLevel nextLevel = FAN_LEVEL_OFF;
    int temperatureX10 = 0;
    int humidityX10 = 0;
    int sensorResult;

    (void)parameter;

    printf(APP_TAG " simulated fan v6 task started\r\n");
    printf(APP_TAG " boot mode=OFF; motor hold defaults to ON\r\n");
    printf(APP_TAG " short press: OFF->QUIET->AUTO->OFF\r\n");
    printf(APP_TAG " long press: toggle ECO preference\r\n");
    printf(APP_TAG
        " motor policy: hold low duty; pulse on level or 5%% speed changes\r\n");

    while (1) {
        PollButton();

        ++tick;
        if (tick >= (SHT30_SAMPLE_PERIOD_MS / BUTTON_POLL_MS)) {
            tick = 0U;
            sensorResult = Sht30Read(&temperatureX10, &humidityX10);

            if (sensorResult == 0) {
                g_sensor_ok = 1;
                g_temperature_x10 = temperatureX10;
                g_humidity_x10 = humidityX10;

                if (g_fan_mode == FAN_MODE_OFF) {
                    targetSpeed = 0U;
                } else if (g_fan_mode == FAN_MODE_QUIET) {
                    targetSpeed = 25U;
                    if (g_preference_bias < 0) {
                        int quietTarget = (int)targetSpeed + g_preference_bias;
                        targetSpeed = quietTarget > 0 ?
                            (unsigned int)quietTarget : 0U;
                    }
                } else {
                    targetSpeed =
                        CalculateTargetSpeed(temperatureX10, humidityX10);
                }

                nextSpeed = SmoothSpeed(g_virtual_speed, targetSpeed);
                nextLevel = MapVirtualLevel(nextSpeed);

                if (ShouldPulseFeedback(g_virtual_level, nextLevel,
                    g_last_pulse_speed, nextSpeed)) {
                    ActuatorPulse(nextLevel, nextSpeed);
                    g_last_pulse_speed = nextSpeed;
                    g_virtual_level = nextLevel;
                }
                ApplyMotorPolicy(nextSpeed);

                g_virtual_speed = nextSpeed;
                UpdateEnergyIndex(nextSpeed);
                g_last_saving = CalculateSaving();

                printf(APP_TAG
                    " T=%d.%dC H=%d.%d%% mode=%s virtual=%u%% level=%s eco=%s save=%u%%\r\n",
                    temperatureX10 / 10,
                    temperatureX10 < 0 ? -(temperatureX10 % 10) :
                        temperatureX10 % 10,
                    humidityX10 / 10,
                    humidityX10 % 10,
                    FanModeName(g_fan_mode),
                    g_virtual_speed,
                    FanLevelName(g_virtual_level),
                    g_preference_bias < 0 ? "ON" : "OFF",
                    CalculateSaving());

                UpdateDisplay(1, temperatureX10, humidityX10,
                    g_virtual_speed, g_fan_mode, g_virtual_level);
            } else {
                MotorStop();
                g_sensor_ok = 0;
                g_virtual_speed = 0U;
                g_virtual_level = FAN_LEVEL_OFF;
                g_last_pulse_speed = 0U;
                ApplyMotorPolicy(0U);
                g_publish_now = 1;
                printf(APP_TAG " SHT30 read error=%d\r\n", sensorResult);
                UpdateDisplay(0, 0, 0, 0U, FAN_MODE_OFF,
                    FAN_LEVEL_OFF);
            }
        }

        hi_sleep(BUTTON_POLL_MS);
    }

    return HI_NULL;
}


static void ComfortFanEntry(void)
{
    hi_task_attr attr;
    hi_u32 ret;

    printf(APP_TAG " initializing simulated fan v6 + MQTT\r\n");

    memset(&attr, 0, sizeof(attr));

    (void)hi_gpio_init();
    OledInit();
    ButtonInit();

    if (Sht30Init() != 0) {
        OledClearBuffer();
        OledDrawString(0U, 1U, "I2C INIT ERROR");
        OledFlush();
    }

    if (FanInit() != 0) {
        OledClearBuffer();
        OledDrawString(0U, 1U, "PWM INIT ERROR");
        OledFlush();
    }

    MotorStop();
    g_fan_mode = FAN_MODE_OFF;
    g_virtual_speed = 0U;
    g_virtual_level = FAN_LEVEL_OFF;

    OledClearBuffer();
    OledDrawString(0U, 1U, "SIM FAN MQTT V6");
    OledDrawString(0U, 3U, "MODE: OFF");
    OledDrawString(0U, 5U, "PRESS KEY");
    OledFlush();

    attr.stack_size = TASK_STACK_SIZE;
    attr.task_prio = TASK_PRIORITY;
    attr.task_name = (hi_char *)"comfort_fan";

    ret = hi_task_create(&g_task_id, &attr, ComfortFanTask, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf(APP_TAG " task create failed:0x%x\r\n", ret);
        OledClearBuffer();
        OledDrawString(0U, 2U, "TASK ERROR");
        OledFlush();
        return;
    }

    memset(&attr, 0, sizeof(attr));
    attr.stack_size = NETWORK_TASK_STACK_SIZE;
    attr.task_prio = NETWORK_TASK_PRIORITY;
    attr.task_name = (hi_char *)"comfort_net";

    ret = hi_task_create(&g_network_task_id, &attr,
        ComfortNetworkTask, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf(NET_TAG " task create failed:0x%x\r\n", ret);
    }
}

SYS_RUN(ComfortFanEntry);
