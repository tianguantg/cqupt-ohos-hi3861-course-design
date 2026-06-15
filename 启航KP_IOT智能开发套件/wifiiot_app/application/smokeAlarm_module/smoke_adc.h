#ifndef __SMOKE_ADC_H__
#define __SMOKE_ADC_H__

#include <hi_types_base.h>

#define APP_DEMO_ADC
#ifdef APP_DEMO_ADC

#include <hi_adc.h>
#include <hi_stdlib.h>
#include <hi_early_debug.h>

#define ADC_TEST_LENGTH  128
#define VLT_MIN 100

hi_void smoke_adc_gather(hi_void);
#endif
#endif
