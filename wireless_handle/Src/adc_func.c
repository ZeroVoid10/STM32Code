#include "adc_func.h"
#include "nrf24l01.h"
#include "string.h"
uint32_t ADC_Value[150];
int adc_flag = 0;

float adc_num = 0;
float adc_power = 0;
int adc_power_off_flag = 0;
struct adc_rocker adc_rocker_value;
void adc_exe()
{
    if(adc_flag == 0) return;
    //uint32_t ad[5] = {0};
    /*
    for(int i = 0; i < 150;)
            {//yzx
            ad[0] += ADC_Value[i++];
            ad[1] += ADC_Value[i++];
            ad[2] += ADC_Value[i++];
            ad[3] += ADC_Value[i++];
            ad[4] += ADC_Value[i++];
            }
    adc_power = ad[0]/30/2.98-2205/2.98;//2503  2205
    if(adc_power<=0)
    {
        adc_power = 0;
        //adc_power_off_flag = 1;
    }
    Lcd_Show_Int(215,0,12,0,(int)adc_power);
    */
        //data��0��5�ֱ���ad1��λ��λ��ad2��λ��λ��ad3��λ��λ
        uint16_t adc_data[4];
        uint8_t temp[10] = {0};
        for(int i = 0; i < 4; i++)
        {
            /*
            ad[i+1] /= 30;
           // uprintf("ad %d = %d\r\n",i,ad[i]);
            ad[i+1] /= 16;
            if(ad[i+1] > 255) ad[i+1] = 255;
            adc_data[i] = (uint8_t)ad[i+1];
            */
           adc_data[i] = (*((uint16_t *)&adc_rocker_value+1+i)-2048)*256/4096;
           //adc_data[i] = (uint16_t)(((*(ADC_Value+1+i))-2048)*256/4096);
        }
        //can_send_msg(324,adc_data,8);//ҡ��id324
        memcpy(temp,(uint16_t *)&adc_data,8);
        //NRF_Install_TX_Data(temp, 10);
        nrf_send_data(temp, 10);
        //nrf_send(adc_data,7);//nrf��һλ��R
    adc_flag = 0;
}