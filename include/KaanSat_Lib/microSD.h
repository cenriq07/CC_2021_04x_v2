/*
 * microSD.h
 *
 *  Created on: 20/04/2021
 *      Author: Enrique
 */

#ifndef INCLUDE_KAANSAT_LIB_MICROSD_H_
#define INCLUDE_KAANSAT_LIB_MICROSD_H_

#include "sci.h"
#include "stdio.h"
#include "gio.h"
#include "spi.h"
#include "uartstdio.h"
#include "het.h"
#include "math.h"

#define DATA_FILENAME   "FSW_DATA.TXT"
#define STATE_FILENAME  "FSW_STE.TXT"

FIL filew; //file txt
FRESULT iFResult;

#define PATH_BUF_SIZE           80

static FIL g_sFileObject;
static char g_pcTmpBuf[PATH_BUF_SIZE];
static char g_pcCwdBuf[PATH_BUF_SIZE] = "/";

char Data_acel[100];
char F_STATE[10];
int res;

int sdWriteMemory(const char* path, char* data);

#endif /* INCLUDE_KAANSAT_LIB_MICROSD_H_ */
