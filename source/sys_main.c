/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/*----------------- OS Libraries --------------------*/
#include "FreeRTOS.h"
#include "os_task.h"
#include "sys_core.h"
#include "gio.h"
#include "adc.h"
#include "sci.h"
#include "het.h"
/*------------- KA'AN SAT Libraries -----------------*/
#include "KaanSat_Lib/Utilities.h"
#include "KaanSat_Lib/Commands.h"
#include "KaanSat_Lib/PWM.h"
#include "KaanSat_Lib/BMP280.h"
#include "KaanSat_Lib/allADC.h"
#include "lin.h"
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */

static char receivedData[2];
int i = 0;
//#define MICROSD     TRUE
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
void rtiNotification(uint32 notification);
void sciNotification(sciBASE_t *sci, unsigned flags);
void vMicroSD(void *pvParameters);
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    gioInit();
    hetInit();
    adcInit();
    spiInit();

    /* ------------------- SCI CONFIG -------------------*/
     sciInit();
     sciEnableNotification(scilinREG, SCI_RX_INT);
     _enable_IRQ();
     _enable_interrupt_();
     sciReceive(scilinREG, 1, ( unsigned char *)receivedData);

    /* ------------------- SD READER -------------------*/

    /* ------------------- TASKS -------------------*/
    xTaskCreate(vTelemetry,"T. Container",1000, NULL, 1, &xTelemetryHandle);
    xTaskCreate(vSensors,"Sensors",configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vMissionOperations,"Sat Ops",configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();
    while(1);

/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
/*---------------------------------- TELEMETRY  ------------------------------*/
void vTelemetry(void *pvParameters)
{
    portTickType xTelemetryTime;
    xTelemetryTime = xTaskGetTickCount();

    while(1)
    {
        if(telemetry_ON)
        {
            createTelemetryPacket();
            sciSendData(buff_size, command, 0);
            getTime();
            PACKET_COUNT++;
        }
        if(!telemetry_ON)
        {
            sciSendData(sprintf(command,"WAITING\n"),command, 0);
        }
        GPS_TIME++;
        vTaskDelayUntil(&xTelemetryTime, T_TELEMETRY);
    }
}
/*-------------------------------------- SENSORS ------------------------------------*/
void vSensors(void *pvParameters)
{
    portTickType xSensorsTime;
    xSensorsTime = xTaskGetTickCount();
    toggle_sim = 1;
    float presion_u[10];

    while(1)
    {
        /* ----------------| Battery |------------------ */
        readAllAdc(MyChannel);
        valorAdc_0 = MyChannel[0].ADC_conv;
        VOLT_BATT = getAdcVoltage(valorAdc_0);
        /* ----------------| BMP280 |------------------- */

        ComandoSPI[0]=((0x7F & 0xF4)<<8)|0x00AB;
        spiSendAndGetData(spiREG_BMP, &SPI1_data_configCh2,(uint32) 1, ComandoSPI,DatoSPI01);
        hacernada(500000);

        ComandoSPI[0]=((0x7F & 0xF5)<<8)|0x0014;
        spiSendAndGetData(spiREG_BMP, &SPI1_data_configCh2,(uint32) 1, ComandoSPI,DatoSPI01);
        hacernada(100000);

        CAlibracion_BMP280(spiREG_BMP,SPI1_data_configCh2);
        LEERTempYpresRAW_bmp280(spiREG_BMP, SPI1_data_configCh2, 100, Pres_bmp280, Temp_bmp280,Alt_bmp280);

        if(!SIM_ON)
            PRESS_BAR = Pres_bmp280[1];

        TEMPERATURE = Temp_bmp280[1];

        updateAltitude(xSensorsTime, presion_u);

        vTaskDelayUntil(&xSensorsTime, T_SENSORS);
    }
}
/*---------------------------------- MISSIONS OPERATIONS ------------------------------*/
void vMissionOperations(void *pvParameters)
{
    portTickType xOpsTime;
    xOpsTime = xTaskGetTickCount();

    pwmStart(hetRAM1, PWM_PAYLOAD);
    SERVO_PAYLOAD.period = 20000;
    pwmSetDuty(hetRAM1, PWM_PAYLOAD, 7U);
//    SERVO_PAYLOAD.duty = SPOS_ZERO;

    float ALTITUDE_STATES[5] = {45, 700, 500, 400, 50};

    while(1)
    {
        if(sciControl == 1)
        {
            if(LAND == false && (ALTITUDE_BAR >= ALTITUDE_INIT + ALTITUDE_STATES[STATE_INDEX]))
            {
                STATE_INDEX++;
                STATE = STATE_INDEX;
                LAND = true;
    //            updateState(STATE);
            }
            if(LAND == true && (ALTITUDE_BAR <= ALTITUDE_INIT + ALTITUDE_STATES[STATE_INDEX]))
            {
                if(STATE != LANDING)
                    STATE_INDEX++;

                STATE = STATE_INDEX;
    //            updateState(STATE);

                if(STATE == SP1_RELEASE)
                {
//                    SERVO_PAYLOAD.duty = SPOS_SP1;
                    pwmSetDuty(hetRAM1, PWM_PAYLOAD, 9U);
                }
                if(STATE == SP2_RELEASE)
                {
//                    SERVO_PAYLOAD.duty = SPOS_SP2;
                    pwmSetDuty(hetRAM1, PWM_PAYLOAD, 12U);
                }
//                pwmSetSignal10e3(hetRAM1, PWM_PAYLOAD, SERVO_PAYLOAD);
            }

        }

//        pwmSetSignal10e3(hetRAM1, PWM_PAYLOAD, SERVO_PAYLOAD);
        vTaskDelayUntil(&xOpsTime, T_OPERATIONS);
    }
}
/*---------------------------------- SCI NOTIFICATION ------------------------------*/
void sciNotification(sciBASE_t *sci, unsigned flags )
{
    sciReceive(scilinREG, 1, (unsigned char *)&receivedData);
    getCommand(receivedData[0]);
}

/* USER CODE END */
