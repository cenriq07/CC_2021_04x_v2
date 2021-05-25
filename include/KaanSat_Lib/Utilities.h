/*
 * KA'AN SAT TEAM PROPRIETARY INFORMATION - kaansat@soyunaq.mx
 *
 * Property of KA'AN SAT TEAM from Universidad Aeronautica en Queretaro,
 * Unauthorized reproduction and/or distribution in other universities
 * is strictly prohibited.
 *
 * Redistribution of code between KA'AN SAT TEAM members is permitted,
 * under the following conditions:
 *
 *  Redistribution of source code is under the KAANSAT TEAM name.
 *
 *  Any part of the software will not be used by other teams or
 *  organizations.
 *
 * -------------------------------------------------------------------
 *                           FILE DESCRIPTION
 * -------------------------------------------------------------------
 * Utilities.h
 *
 *  Created on: 16/01/2021
 *      Author: Enrique
 */

#ifndef INCLUDE_KAANSAT_LIB_UTILITIES_H_
#define INCLUDE_KAANSAT_LIB_UTILITIES_H

/* --------------- LIBRARIES ---------------*/
#include "FreeRTOS.h"
#include "os_task.h"
#include "sys_core.h"
#include "sci.h"
#include "spi.h"
#include "het.h"
#include "adc.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "strings.h"

/* ------------------- DECLARATIONS ------------------*/
#define COMM_SIZE       200
#define T_TELEMETRY     1000/portTICK_RATE_MS
#define T_SENSORS       1000/portTICK_RATE_MS
#define T_OPERATIONS    2000/portTICK_RATE_MS
#define LONG_CMD_KEY    100
#define LONG_SP_PARAM   4
#define TEAM_NUMBER     "1714"

#define spiREG_BMP      spiREG1
#define spiREG_SD       spiREG4
#define spiPORT_SD      spiPORT4

#define P0              101325      //   Pa
#define Rair            8.31432     //   N.m/(mol.K)
#define ug              0.2841408   //   Producto u*g ; [u = 0.0289644 kg/mol]*[g = 9.81 m/s^2]
#define Rug             29.261271   //   R/ug

#define PWM_PAYLOAD     pwm0
#define PWM_CAMERA      pwm1
#define SPOS_ZERO       250         // 0°
#define SPOS_SP1        505         // 45°
#define SPOS_SP2        750         // 90°

#define DH 0x0013A200
#define DL_ET 0x40DB0B6E
//#define DL_ET 0x40E5421A

hetSIGNAL_t SERVO_PAYLOAD;
hetSIGNAL_t SERVO_CAMERA;

enum STATES
{
    PRELAUNCH, LAUNCH, DEPLOYMENT, SP1_RELEASE, SP2_RELEASE, LANDING
};
/*------------------ TASKS -------------------------*/

void vTelemetry(void *pvParameters);
void vSensors(void *pvParameters);
void vMissionOperations(void *pvParameters);

xTaskHandle xWTStartHandle;
xTaskHandle xTelemetryHandle;

/* -------------- VARIABLES ----------------*/
extern char command[COMM_SIZE];
extern char tramaAPI[COMM_SIZE];
extern uint32 buff_size;
extern uint32 buff_sizeAPI;
extern char FSW_STATE_TEMP;

extern char CMD_KEY[LONG_CMD_KEY];
extern bool ejectCom;
/* -------------- TELEMETRY ----------------*/
                /* CONTAINER */

extern int PACKET_COUNT;
extern char cPACKET_COUNT[6];

extern char MODE;

extern char SP1_RELEASED;
extern char SP2_RELEASED;

extern uint8 STATE;
extern char *FSW_STATE[];

extern int SP1_PC;
extern char cSP1_PC[6];

extern int SP2_PC;
extern char cSP2_PC[6];

//      Sensors

extern float ALTITUDE_BAR;
extern char cALTITUDE_BAR[8];

extern float ALTITUDE_INIT;

extern float PRESS_BAR;
extern char cPRESS_BAR[8];

extern float TEMPERATURE;
extern char cTEMPERATURE[8];

extern float VOLT_BATT;
extern char cVOLT_BATT[4];
extern int GPS_TIME;
extern char cGPS_TIME[8];
extern char cLAT[10];
extern char cLON[10];
extern char cALT[7];
extern char cNSATS[3];

                        /* PAYLOADS */

extern char SP1_MISSION_TIME[9];
extern char SP1_PACKET_COUNT[4];
extern char SP1_PACKET_TYPE[3];
extern char SP1_ALTITUDE[8];
extern char SP1_TEMPERATURE[5];
extern char SP1_ROTATION_RATE[5];

extern char SP2_MISSION_TIME[9];
extern char SP2_PACKET_COUNT[4];
extern char SP2_PACKET_TYPE[3];
extern char SP2_ALTITUDE[8];
extern char SP2_TEMPERATURE[5];
extern char SP2_ROTATION_RATE[5];
/*---------------- COMMAND VARIABLES ----------------*/
extern bool telemetry_ON;
extern bool SP_ON;
extern bool SIM_ON;
extern int ENABLE_SIM;
extern int STATE_INDEX;
extern bool LAND;
extern int H, M, S;
extern char cH[3], cM[3], cS[3];
extern bool SP1X_ON;
extern bool SP2X_ON;
extern bool R1;
extern bool R2;
extern int sciControl;
extern bool toggle_sim;
extern char ECHO[5];
/* ------------------ FUNCTIONS --------------------*/

void createTelemetryPacket();
bool sciSendData(uint32 numOfDat, char* charDat, bool CR);

void getTime();
void updateState(int State);
void updateAltitude (portTickType xSensorsTime, float presion_u[]);
float getAltitude(float P, float T);

static void reverse(char *s, size_t s_len);
size_t sltoa(char *s, long int n);
void ftoa(float n, char *res, int afterpoint);


#endif /* INCLUDE_KAANSAT_LIB_UTILITIES_H_ */
