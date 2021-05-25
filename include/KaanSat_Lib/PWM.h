/*
 * PWM.h
 *
 *  Created on: 15 nov. 2020
 *      Author: Enrique
 */

#ifndef INCLUDE_SATLIB_PWM_H_
#define INCLUDE_SATLIB_PWM_H_

#include "esm.h"
#include "het.h"

// TODO: Duty cycle -> Sentido correcto
#define STOP    750
#define CW      500
#define CCW     1000

static const uint32 s_het1pwmPolarity[8U] =
{
    3U,
    3U,
    3U,
    3U,
    3U,
    3U,
    3U,
    3U,
};

enum
{
    stop,forward,right,left,
};

hetSIGNAL_t Servo;

void pwmSetSignal10e3(hetRAMBASE_t * hetRAM, uint32 pwm, hetSIGNAL_t signal);

#endif /* INCLUDE_SATLIB_PWM_H_ */
