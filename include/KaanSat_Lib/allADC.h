/*
 * allADC.h
 *
 *  Created on: 04/06/2021
 *      Author: Enrique
 */

#ifndef INCLUDE_KAANSAT_LIB_ALLADC_H_
#define INCLUDE_KAANSAT_LIB_ALLADC_H_

#include "adc.h"
#include "math.h"

#define numSensors  1
#define vs          3.3                       /* Voltaje de referencia ADC*/
#define resolution  12                      /* Resolución del ADC*/

typedef struct
{
    int id;                                 /* Su posición en el arreglo*/
    int ADC_conv;                           /* La lectura del ADC*/
}   ADC_channel;

ADC_channel MyChannel[1];                   /* Arreglo de estructuras para lectura ADC*/
int valorAdc_0;                             /*ADIN0 Battery*/

void readAllAdc(ADC_channel *ADCvalues);
float getAdcVoltage(int AdcConv);

#endif /* INCLUDE_KAANSAT_LIB_ALLADC_H_ */
