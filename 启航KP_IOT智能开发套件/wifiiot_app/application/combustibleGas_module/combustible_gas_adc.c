#include <stdio.h>
#include "dbg.h"
#include "combustible_gas_adc.h"
#include "hi_gpio.h"
#include "combustible_gas_module.h"

hi_u16 gas_adc_buf[ADC_TEST_LENGTH] = { 0 };

hi_void gas_convert_to_voltage(hi_u32 data_len)
{
    hi_u32 i;
    float vlt_max = 0;
    float vlt_min = VLT_MIN;
    hi_u16 vlt;
    for (i = 0; i < data_len; i++) {
        vlt = gas_adc_buf[i];
        float voltage = (float)vlt * 1.8 * 4 / 4096.0;  /* vlt * 1.8 * 4 / 4096.0: Convert code into voltage */
        vlt_max = (voltage > vlt_max) ? voltage : vlt_max;
    }
    printf("vlt_max:%.3f \n", vlt_max);

    if(vlt_max > 2.5){
        printf("gas alarm \r\n");
        gas_pwm_start(50);
    }else{
        gas_pwm_start(0);
    }
}

hi_void gas_adc_gather(hi_void)
{
    hi_u32 ret, i;
    hi_u16 data;  /* 10 */
    printf("ADC Test Start\n");

    memset_s(gas_adc_buf, sizeof(gas_adc_buf), 0x0, sizeof(gas_adc_buf));
    for (hi_u8 em = 0; em < HI_ADC_EQU_MODEL_BUTT; em++) {
        for (i = 0; i < ADC_TEST_LENGTH; i++) {
            ret = hi_adc_read(HI_ADC_CHANNEL_3, &data, (hi_adc_equ_model_sel)em, HI_ADC_CUR_BAIS_DEFAULT, 0);
            if (ret != HI_ERR_SUCCESS) {
                printf("ADC Read Fail\n");
                return;
            }
            gas_adc_buf[i] = data;
        }
        gas_convert_to_voltage(ADC_TEST_LENGTH);
    }
    printf("ADC Test Average Mode End\n");

}
