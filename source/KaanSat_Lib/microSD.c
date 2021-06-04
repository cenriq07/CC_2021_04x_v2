/*
 * microSD.c
 *
 *  Created on: 20/04/2021
 *      Author: Enrique
 */

#include "KaanSat_Lib/microSD.h"

int sdWriteMemory(const char* path, char* data)
{
    gioToggleBit(gioPORTA, 0U);

    iFResult = open_append(&filew, path);

    if (iFResult != FR_OK)
    {
        /* Error. Cannot create the file */
        //UARTprintf("Error opening file\n");
        //return 0;
    }
    else
    {
        //UARTprintf("Writing ");
        res = f_printf(&filew, data);

        if (res <0)
        {
           /* Error. Cannot write header */
           //UARTprintf("Cannot write\n");
           //return 0;
        }
        else
        {
           res = f_close(&filew); // If you don't do this the file might not save
           if (res != FR_OK){   /* Error. Cannot close the file */  /*return 0;*/}
           else {   /*return 1;*/ }
        }
    }

    gioToggleBit(gioPORTA, 0U);
}

