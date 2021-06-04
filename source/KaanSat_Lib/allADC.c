/*
 * allADC.c
 *
 *  Created on: 04/06/2021
 *      Author: Enrique
 */

#include "KaanSat_Lib/allADC.h"


float getAdcVoltage(int AdcConv)
{
    float v0;
    v0 = (AdcConv*vs)/(pow(2,resolution)-1);
    return v0;
}


void readAllAdc(ADC_channel *ADCvalues)
{
    int i_adc = 0;
    int ch_count = 0;
    adcData_t adc_data[numSensors];
    adcStartConversion(adcREG1,adcGROUP1);                      // comenzamos la conversión  y la guarda en adcREG1
    while((adcIsConversionComplete(adcREG1,adcGROUP1))==0);
    ch_count = adcGetData(adcREG1, adcGROUP1, &adc_data[0]);    // Se almacena el número de conversiones realizadas
    while(ch_count)
    {
        ADCvalues[i_adc].ADC_conv = (adc_data[i_adc].value);
        ADCvalues[i_adc].id = (adc_data[i_adc].id);
        i_adc++;
        ch_count--;
    }
}
