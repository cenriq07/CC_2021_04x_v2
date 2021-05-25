/*
 * Commands.h
 *
 *  Created on: 02/02/2021
 *      Author: Enrique
 */

#ifndef INCLUDE_KAANSAT_LIB_COMMANDS_H_
#define INCLUDE_KAANSAT_LIB_COMMANDS_H_

extern char *commands[];
extern int commandsTam;

enum
{
    CX,ST,SIM,SP1X,SP2X,SIMP
};

extern int sim_ok;

void getSPTelemetry(char *telemetry);
int getCommand(char cmd_char);
int findCommand(char *comm);
void commCX(char *value[]);
void commST(char *value[]);
void commSIM(char *value[]);
void commSP1X(char *value[]);
void commSP2X(char *value[]);
void commSIMP(char *value[]);

#endif /* INCLUDE_KAANSAT_LIB_COMMANDS_H_ */
