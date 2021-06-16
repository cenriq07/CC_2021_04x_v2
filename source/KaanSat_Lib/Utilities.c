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
 * Utilities.c
 *
 *  Created on: 16/01/2021
 *      Author: Enrique
 *
 */

#include "KaanSat_Lib/Utilities.h"

/* ------------------ VARIABLES ------------------*/
char command[COMM_SIZE] = {};
char tramaAPI[COMM_SIZE] = {};
uint32 buff_size = 0;
uint32 buff_sizeAPI = 0;
char FSW_STATE_TEMP = '9';

char CMD_KEY[LONG_CMD_KEY] = {};

/* ------------------- TELEMETRY -----------------------*/
                    /* CONTAINER */
int PACKET_COUNT = 0;
char cPACKET_COUNT[6] = "PC";

char MODE = 'F';

char SP1_RELEASED = 'N';
char SP2_RELEASED = 'N';

extern uint8 STATE = LAUNCH;
char *FSW_STATE[] = {"PRELAUNCH","LAUNCH","DEPLOYMENT","SP1_RELEASE","SP2_RELEASE","LANDING"};

int SP1_PC = 0;
char cSP1_PC[6] = "P1";

int SP2_PC = 0;
char cSP2_PC[6] = "P2";

//       Sensors

float ALTITUDE_BAR = 0.0;
char cALTITUDE_BAR[8] = "h";

float ALTITUDE_INIT = 0.0;

float PRESS_BAR = 999999.0;
char cPRESS_BAR[8] = "PR";

float TEMPERATURE = 0.0;
char cTEMPERATURE[8] = "T";

float VOLT_BATT = 0.0;
char cVOLT_BATT[4] = "V";

int GPS_TIME = 113020;
char cGPS_TIME[8] = "TM";
char cLAT[12] = "20.6593477" ;
char cLON[12] = "-100.38697" ;
char cALT[7] = "HG" ;
char cNSATS[3] = "3" ;

                    /* PAYLOADS */

char SP1_MISSION_TIME[10] = "X";
char SP1_PACKET_COUNT[4] = "X";
char SP1_PACKET_TYPE[3] = "X";
char SP1_ALTITUDE[8] = "X";
char SP1_TEMPERATURE[6] = "X";
char SP1_ROTATION_RATE[6] = "X";

char SP2_MISSION_TIME[10] = "X";
char SP2_PACKET_COUNT[4] = "X";
char SP2_PACKET_TYPE[3] = "X";
char SP2_ALTITUDE[8] = "X";
char SP2_TEMPERATURE[6] = "X";
char SP2_ROTATION_RATE[6] = "X";

/*---------------- COMMAND VARIABLES ----------------*/
bool telemetry_ON = false;
bool SP_ON = 0;
bool SIM_ON = false;
int ENABLE_SIM=0;
int STATE_INDEX = 0;
bool LAND = false;
int H=0, M=0, S=0;
char cH[3]="hhx", cM[3]="mmx", cS[3]="ssx";
bool SP1X_ON = false;
bool SP2X_ON = false;
bool R1 = false;
bool R2 = false;
int sciControl = 0;
bool toggle_sim = 0;
bool ejectCom = false;
char ECHO[5] = "ECHO";
/* ------------ TELEMETRY FORMAT -------------------*/
static const char* FORMAT = "/*1714,%s,%s,%s,%s,C,%c,%c,%c,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,1714,%s,%s,S1,%s,%s,%s,1714,%s,%s,S2,%s,%s,%s*/";
//static const char* FORMAT = "1714,%s:%s:%s,%s,C,%c,%c,%c,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s;1714,%s,%s,S1,%s,%s,%s;1714,%s,%s,S2,%s,%s,%s;";
//                              1    2  3  4  5 6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21  22  23 24 25 26 27 28  29  30 31 32 33 34 35

/* ------------------ FUNCTIONS --------------------*/
void createTelemetryPacket()
{
    char zeroH[2] = "0";
    char zeroM[2] = "0";
    char zeroS[2] = "0";

    ftoa(PACKET_COUNT, cPACKET_COUNT, 0);

    ftoa(H, cH, 0);
    ftoa(M, cM, 0);
    ftoa(S, cS, 0);

    ftoa(PRESS_BAR, cPRESS_BAR, 3);
    ftoa(TEMPERATURE, cTEMPERATURE, 1);

    if(H<10)
    {
        strcat(zeroH, cH);
        strcpy(cH,zeroH);
    }
    if(M<10)
    {
        strcat(zeroM, cM);
        strcpy(cM,zeroM);
    }
    if(S<10)
    {
        strcat(zeroS, cS);
        strcpy(cS,zeroS);
    }

    ftoa(VOLT_BATT, cVOLT_BATT, 1);

    ftoa(SP1_PC, cSP1_PC, 0);
    ftoa(SP2_PC, cSP2_PC, 0);

    ftoa(ALTITUDE_BAR, cALTITUDE_BAR, 1);
    ftoa(TEMPERATURE, cTEMPERATURE, 1);

    ftoa(GPS_TIME, cGPS_TIME, 0);

    buff_size = sprintf(command,
                        FORMAT,                     /* <TELEMETRY_FORMAT> */
                        cH,cM,cS,                   /* 2,3,4<MISSION_TIME> */
                        cPACKET_COUNT,              /* 5<PACKET_COUNT> */
                                                    //* 6<PACKET_TYPE> */
                        MODE,                       /* 7<MODE> */
                        SP1_RELEASED,               /* 8<SP1_RELEASED> */
                        SP2_RELEASED,               /* 9<SP2_RELEASED> */
                        cALTITUDE_BAR,              /* 10<ALTITUD> */
                        cTEMPERATURE,               /* 11<TEMP> */
                        cVOLT_BATT,                 /* 12<VOLTAGE> */
                        cGPS_TIME,                  /* 13<GPS_TME> */
                        cLAT,                       /* 14<GPS_LATITUDE> */
                        cLON,                       /* 15<GPS_LONGITUDE> */
                        cALT,                       /* 16<GPS_ALTITUDE> */
                        cNSATS,                     /* 17<GPS_SATS> */
                        FSW_STATE[STATE],           /* 18<SOFTWARE_STATE> */
                        cSP1_PC,                    /* 19<SP1_PACKET_COUNT> */
                        cSP2_PC,                    /* 20<SP2_PACKET_COUNT> */
                        ECHO,                       /* 21 <CMD_ECHO> */
                                                    //* 22 TEAM ID */
                        SP1_MISSION_TIME,           /* 23<MISSION_TIME> */
                        SP1_PACKET_COUNT,           /* 24<PACKET_COUNT> */
                                                    //* 25<PACKET_TYPE> */
                        SP1_ALTITUDE,               /* 26<ALTITUD> */
                        SP1_TEMPERATURE,            /* 27<TEMP> */
                        SP1_ROTATION_RATE,          /* 28<SP_ROTATION_RATE> */
                                                    //* 29 TEAM ID */
                        SP2_MISSION_TIME,           /* 30<MISSION_TIME> */
                        SP2_PACKET_COUNT,           /* 31<PACKET_COUNT> */
                                                    //* 32<PACKET_TYPE> */
                        SP2_ALTITUDE,               /* 33<ALTITUD> */
                        SP2_TEMPERATURE,            /* 34<TEMP> */
                        SP2_ROTATION_RATE           /* 35<SP_ROTATION_RATE> */
                        );
}

void getTime()
{
    if(S<60)
        S++;

    if(S==60)
    {
        S = 0;
        if(M<60)
            M++;

        if(M == 60)
        {
            M = 0;
            if(H<24)
                H++;

            if(H == 24)
                H = 0;
        }
    }
}

void updateState(int State)
{
    switch(State)
    {
        case PRELAUNCH:
            telemetry_ON = false;
            STATE_INDEX = 0;
            LAND = false;
            SERVO_PAYLOAD.duty = SPOS_ZERO;
            break;
        case LAUNCH:
            telemetry_ON = true;
            LAND = false;
            SERVO_PAYLOAD.duty = SPOS_ZERO;
            break;
        case DEPLOYMENT:
            telemetry_ON = true;
            LAND = true;
            SERVO_PAYLOAD.duty = SPOS_ZERO;
            break;
        case SP1_RELEASE:
            telemetry_ON = true;
            LAND = true;
            SERVO_PAYLOAD.duty = SPOS_SP1;
            break;
        case SP2_RELEASE:
            telemetry_ON = true;
            LAND= true;
            SERVO_PAYLOAD.duty = SPOS_SP2;
            break;
        case LANDING:
            telemetry_ON = false;
            LAND = true;
            SERVO_PAYLOAD.duty = SPOS_SP2;
            break;
    }
}

void updateAltitude (portTickType xSensorsTime, float presion_u[])
{
    int i=0;
    float press_i=0, press_new=0, x=0, xT=0, var=0, desv=0, a=0, b=0;
    if (toggle_sim==1)
    {
        sciControl = 0;
        ALTITUDE_INIT = 0.0;
        vTaskDelayUntil(&xSensorsTime, 2000/portTICK_RATE_MS);
        toggle_sim=0;
        for (i=0;i<=9;i++)
        {
            presion_u[i]= PRESS_BAR; //PRIMER ACOMODO DE PRESIONES
            if (toggle_sim) break;
            vTaskDelayUntil(&xSensorsTime, T_SENSORS);
        }
        if (!toggle_sim)
        {
            for (i=0;i<=9;i++)
            {
                press_i += presion_u[i];
            }
            press_i = press_i/(float)10;
            ALTITUDE_INIT = getAltitude(press_i,TEMPERATURE);
            sciControl = 1;
        }
    }
    else
    {
        press_new = PRESS_BAR; //CAMBIO
        for (i=0;i<10;i++)
        {
            x += presion_u [i]; //Media
        }
        xT = x/(float)10;

        for (i=0;i<10;i++)
        {
            var += pow ((presion_u [i] - xT),2.00); //Varianza
        }
        desv= sqrt(var/9); //Desviación estándar

        if (STATE==SP2_RELEASE && desv<50)
        {
            STATE++;
        }

        a = xT-3*desv; //Limite inf
        b = xT+3*desv;  //Limite sup

        if ((press_new<a)||(press_new>b))
        {
            /*m = presion_u[(*cont)] - presion_u[(*cont)-1];
            n = presion_u[0] - presion_u[9];
            if ((m<-2100)||(n<-2100))
            {
                ALTITUDE_BAR = getAltitude(PRESS_BAR);
            }*/
        }
        else
        {
            ALTITUDE_BAR = getAltitude(PRESS_BAR,TEMPERATURE);
        }
        for (i=0;i<9;i++)
        {
            presion_u[i]=presion_u[i+1];
        }
        presion_u[9]=press_new;
    }
}

void GenerateGPSData()
{
    float alt = 1910;
    alt += (rand() % 10) / 10;
    ftoa(alt, cALT, 0);
    //cLAT[8] = (rand() % 10) + '0';
    cLAT[9] = (rand() % 10) + '0';
    //cLON[8] = (rand() % 10) + '0';
    cLON[9] = (rand() % 10) + '0';
    GPS_TIME = (H * 10 * 1000) + (M * 100) + S;
}


float getAltitude(float P, float T)
{
    return Rug*(T+273.15)*log(P0/P);
}

bool sciSendData(uint32 numOfDat, char* charDat, bool CR)
{
    sciSend(scilinREG, numOfDat, (uint8 *)charDat);
    if (CR)
        sciSend(scilinREG, 0x02, (unsigned char *)"\r\n");
    return true;
}

static void reverse(char *s, size_t s_len)
{
    size_t i, j;
    char swap;

    for (i = 0, j = s_len - 1; i < j; ++i, --j)
    {
        swap = s[i];
        s[i] = s[j];
        s[j] = swap;
    }
}

size_t sltoa(char *s, long int n)
{
    size_t i = 0;
    long int sign_mask;
    unsigned long int nn;

    sign_mask = n >> sizeof(long int) * 8 - 1;
    nn = (n + sign_mask) ^ sign_mask;
    do {
        s[i++] = nn % 10 + '0';
    } while (nn /= 10);

    s[i] = '-';
    i += sign_mask & 1;
    s[i] = '\0';

    reverse(s, i);
    return (i);
}

void ftoa(float n, char *res, int afterpoint)
{
    int ipart = (int)n, fpart = 0;
    /* Convertir parte entera */
    int i = sltoa(res, ipart );
    /* Agregar parte decimal */
    if(afterpoint > 0)
    {
        res[i] = '.';
        fpart = (int)(( n - (float)ipart ) * pow(10, afterpoint ));
        sltoa(&res[i+1], abs(fpart) );
    }
}

void hacernada(uint32 id)
{
    while(id)
    {
        id--;
    }
}
