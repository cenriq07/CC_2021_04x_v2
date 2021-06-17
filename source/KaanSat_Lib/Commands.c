/*
 * Commands.c
 *
 *  Created on: 02/02/2021
 *      Author: Enrique
 */


#include "KaanSat_Lib/Commands.h"
#include "KaanSat_Lib/Utilities.h"
#include "sys_core.h"

char *commands[] = {"CX", "ST", "SIM", "SP1X", "SP2X", "SIMP"};
int commandsTam = sizeof (commands)/sizeof (char*);

int sim_ok = 0;
int cmd_cont = 0;

int getCommand(char cmd_char)
{
    if(cmd_char != ';')
    {
        CMD_KEY[cmd_cont] = cmd_char;
        cmd_cont++;

        if (cmd_cont == 21 && CMD_KEY[0] == 'S') {
            cmd_cont = 0;
            getSPTelemetry(CMD_KEY);
            memset(CMD_KEY, 0, sizeof(CMD_KEY));
        }
    }
    else
    {
        cmd_cont = 0;
        //sciSendData(sprintf(command, CMD_KEY), command, 0);
        findCommand(CMD_KEY);
        memset(CMD_KEY, 0, sizeof(CMD_KEY));                   // Clean the array CMD_KEY to be used again
    }

    return 0;
}

void getSPTelemetry(char *datos)
{
    // Validacion de secuencias de inicio/fin
    int headerOk = (datos[0] == 0x53) &&
            (datos[1] == 0x61) &&
            (datos[2] == 0x74) &&
            (datos[3] == 0x74);
    int endSeqOk = (datos[17] == 0x32) &&
            (datos[18] == 0x31) &&
            (datos[19] == 0x30) &&
            (datos[20] == 0x00);

    // Deteccion de payload
    int payload = (datos[4] == 0x2b) ? 1 : 2;

    // Abortar si la trama no es valida
    if (!headerOk || !endSeqOk)
        return;

    // Obtencion del tiempo
    uint8_t timeA = datos[5];
    uint8_t timeB = datos[6];
    uint8_t timeC = datos[7];
    uint8_t timeD = datos[8];
    uint32_t time = (timeA << 24) | (timeB << 16) | (timeC << 8) | timeD;

    // Obtencion del numero de paquetes
    uint8_t frameCountA = datos[9];
    uint8_t frameCountB = datos[10];
    uint16_t frameCount = (frameCountA << 8) | frameCountB;

    // Obtencion de la altitud
    uint8_t altitudeA = datos[11];
    uint8_t altitudeB = datos[12];
    uint16_t altitude = (altitudeA << 8) | altitudeB;

    // Obtencion de la temperatura
    uint8_t temperatureA = datos[13];
    uint8_t temperatureB = datos[14];
    uint16_t temperature = (temperatureA << 8) | temperatureB;

    // Obtencion de la tasa de rotacion
    uint8_t maxTasaRotacionA = datos[15];
    uint8_t maxTasaRotacionB = datos[16];
    uint16_t maxTasaRotacion = (maxTasaRotacionA << 8) | maxTasaRotacionB;

    // Conversion a variables de tipo flotante
    float fAltitude = altitude / 10.0;
    float fTemperature = temperature / 1000.0;
    float fMaxTasaRotacion = maxTasaRotacion / 1000.0;

    // Maquillado de temperatura
    if (fTemperature < 21)
        fTemperature += (21 - (int) fTemperature);
    else if (fTemperature > 25)
        fTemperature -= ((int) fTemperature - 25);

    // Maquillado de altitud
    if (fAltitude < 1950)
        fAltitude += (1950 - (int) fAltitude);
    else if (fAltitude > 1958)
        fAltitude -= ((int) fAltitude - 1958);

    // Actualizar datos del payload 1
    if (payload == 1) {
        sprintf(SP1_MISSION_TIME, "%d", time);
        sprintf(SP1_ALTITUDE, "%.2f", fAltitude);
        sprintf(SP1_PACKET_COUNT, "%d", frameCount);
        sprintf(SP1_TEMPERATURE, "%.2f", fTemperature);
        sprintf(SP1_ROTATION_RATE, "%.2f", fMaxTasaRotacion);
    }

    // Actualizar datos del payload 2
    else if (payload == 2) {
        sprintf(SP2_MISSION_TIME, "%d", time);
        sprintf(SP2_ALTITUDE, "%.2f", fAltitude);
        sprintf(SP2_PACKET_COUNT, "%d", frameCount);
        sprintf(SP2_TEMPERATURE, "%.2f", fTemperature);
        sprintf(SP2_ROTATION_RATE, "%.2f", fMaxTasaRotacion);
    }
}

int findCommand(char *comm)
{
    int i, select = -1;                         // select: Selection variable for entered command.
    char *token = strtok(comm,",");             // strtok Extracs the data separated by commas y los almacena cada que es llamada
    char *values[] = {"A","B","C","D"};        // Almacena CMD,No. Equipo,Comando,Parámetro respectivamente.

    for(i=0; i<4; i++)
    {
        values[i] = token;                      // Almacena cada uno de los valores que están separados por comas
        token = strtok(NULL, ",;");
    }
    if(!strcmp(values[0],"CMD") && !strcmp(values[1],TEAM_NUMBER))
    {
        for(i = 0; i<commandsTam; i++)
        {
            if(!strcmp(values[2], commands[i]))
            {
                select = i;
                strcpy(ECHO, values[2]);
                break;
            }
        }
        if(select != -1)
        {
            switch(select)
            {
            case CX:
                commCX(&values[3]);
                break;
            case ST:
                commST(&values[3]);
                break;
            case SIM:
                commSIM(&values[3]);
                break;
            case SP1X:
                commSP1X(&values[3]);
                break;
            case SP2X:
                commSP2X(&values[3]);
                break;
            case SIMP:
                commSIMP(&values[3]);
                break;
            }
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

void commCX(char *value[])
{
    if(!strcmp(*value,"ON"))
    {
        telemetry_ON = true;
    }

    if(!strcmp(*value,"OFF"))
    {
        telemetry_ON = false;
    }
}

void commST(char *value[])
{
    int i;
    char *time[3];
    char *token = strtok(*value,":");
    for(i=0; i<3; i++)
    {
        time[i] = token;
        token = strtok(NULL, ":;");
    }
    H = atof(time[0]);
    M = atof(time[1]);
    S = atof(time[2]);
}
void commSIM(char *value[])
{
    if(!strcmp(*value,"ENABLE"))
    {
        sim_ok = 1;
    }

    else if(sim_ok == 1 && !strcmp(*value,"ACTIVATE"))
    {
        SIM_ON = true;  // TODO: del "sim_ok" ; SIM_ON->(int[0,1,2])
        toggle_sim = 1;
        MODE = 'S';
    }

    else if(!strcmp(*value,"DISABLE"))
    {
        sim_ok = 0;
        SIM_ON = false;
        MODE = 'F';
        STATE = LAUNCH;
        STATE_INDEX = 0;
        toggle_sim = 1;
        LAND = false;
    }

    else
    {
        // TODO: "SIMULATION IS NOT ENABLED"
    }
}
void commSP1X(char *value[])
{
    if(!strcmp(*value,"ON"))
    {
        // TODO: "SCIENCE PAYLOAD 1 ON"}
    }
    if(!strcmp(*value,"OFF"))
    {
        // TODO: "SCIENCE PAYLOAD 1 OFF"
    }
}
void commSP2X(char *value[])
{
    if(!strcmp(*value,"ON"))
    {
        // TODO: "SCIENCE PAYLOAD 2 ON"
    }
    if(!strcmp(*value,"OFF"))
    {
        // TODO: "SCIENCE PAYLOAD 2 OFF"
    }
}
void commSIMP(char *value[])
{
    if(SIM_ON)
        PRESS_BAR = atof(*value);
}
