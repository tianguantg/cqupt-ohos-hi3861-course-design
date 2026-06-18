#ifndef __SMOKE_MODULE_H__
#define __SMOKE_MODULE_H__

#include <hi_types_base.h>
#include <hi_io.h>
#include <hi_early_debug.h>
#include <hi_gpio.h>
#include <hi_task.h>

hi_void smoke_adc_demo(hi_void);
hi_void gpio_io_init(hi_void);
hi_void pwm_init(hi_void);
hi_void pwm_start(unsigned int duty);

#endif
