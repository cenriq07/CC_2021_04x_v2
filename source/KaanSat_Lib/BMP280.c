/*
 * Copyright (C) 2018 Ka'An Sat - https://kaansat.com.mx/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h>
#include <stdint.h>

#include "spi.h"
#include "KaanSat_Lib/BMP280.h"

/*
 * Constantes fisicas para convertir los datos
 * recibidos del BMP280 a unidades SI
 */
#define p0_def              101325
#define Tr_def              0.0065
#define R_def               8.3
#define M_def               0.02894
#define g_def               9.80665
#define T0_def              288.15
#define f_Pascales_def      101325
#define f_mmHg_def          760.0
#define f_Temp_def          273.15
#define f_T_e_Tr_def        44330.76923
#define Tr_m_R_e_M_p_g_def  0.1900308
#define Const_aire          29.270151

/*
 * Codigos de operacion para el BMP-280
 */
#define BMP280_OK       INT8_C(0)
#define leer_280        0x80
#define escribir_280    0x7F
#define temp_xlsb_280   0xFC
#define temp_lsb_280    0xFB
#define temp_msb_280    0xFA
#define press_xlsb_280  0xF9
#define press_lsb_280   0xF8
#define press_msb_280   0xF7
#define config_280      0xF5
#define ctrl_meas_280   0xF4
#define status_280      0xF3
#define reset_280       0xE0
#define id_280          0xD0
#define calib25_280     0xA1
#define calib24_280     0xA0
#define calib23_280     0x9F
#define calib22_280     0x9E
#define calib21_280     0x9D
#define calib20_280     0x9C
#define calib19_280     0x9B
#define calib18_280     0x9A
#define calib17_280     0x99
#define calib16_280     0x98
#define calib15_280     0x97
#define calib14_280     0x96
#define calib13_280     0x95
#define calib12_280     0x94
#define calib11_280     0x93
#define calib10_280     0x92
#define calib09_280     0x91
#define calib08_280     0x90
#define calib07_280     0x8F
#define calib06_280     0x8E
#define calib05_280     0x8D
#define calib04_280     0x8C
#define calib03_280     0x8B
#define calib02_280     0x8A
#define calib01_280     0x89
#define calib00_280     0x88

typedef uint32 BMP280_U32_t;
typedef int32_t BMP280_S32_t;
typedef void (*bmp280_delay_fptr_t) (uint32 period);
typedef int8_t (*bmp280_com_fptr_t) (uint8 dev_id, uint8 reg_addr,
                                     uint8* data, uint16_t len);

static float PRESION[2] = { 0.0, 0.0 };
static float ALTITUD[2] = { 0.0, 0.0 };
static float TEMPERATURA[2] = { 0.0, 0.0 };

float Temp_bmp280[2]= {0.0,0.0};
float Pres_bmp280[2]= {0.0,0.0};
float Alt_bmp280[2]= {0.0,0.0};

uint16 ComandoSPI[4] = { 0x0, 0x0, 0x0, 0x0 };
uint16 DatoSPI01[4]= {0,0,0,0};
uint16 DatoSPI02[4]= {0,0,0,0};
uint16 DatoSPI03[4]= {0,0,0,0};
uint16 DatoSPI04[4]= {0,0,0,0};

static uint16 STATUS_BMP280 = 0;

struct bmp280_calib_param {
    uint16_t dig_t1;
    int16_t dig_t2;
    int16_t dig_t3;
    uint16_t dig_p1;
    int16_t dig_p2;
    int16_t dig_p3;
    int16_t dig_p4;
    int16_t dig_p5;
    int16_t dig_p6;
    int16_t dig_p7;
    int16_t dig_p8;
    int16_t dig_p9;
    int32_t t_fine;
};

struct bmp280_config {
    uint8 os_temp;
    uint8 os_pres;
    uint8 odr;
    uint8 filter;
    uint8 spi3w_en;
};

struct bmp280_dev {
    uint8 chip_id;
    uint8 dev_id;
    uint8 intf;
    bmp280_com_fptr_t read;
    bmp280_com_fptr_t write;
    bmp280_delay_fptr_t delay_ms;
    struct bmp280_calib_param calib_param;
    struct bmp280_config conf;
};

static struct bmp280_calib_param ParametrosCalibracionBMP280;

spiDAT1_t   SPI1_data_configCh2;        /*SPI data register configuration BMP*/

/**
 * Se detiene para esperar al sensor
 */
void hacernada(uint32 id)
{
    while(id)
    {
        id--;
    }
}
void BMP280_Init(void)
{
    /* Configurar el SPI3 */
    SPI1_data_configCh2.CS_HOLD=FALSE;
    SPI1_data_configCh2.WDEL=TRUE;
    SPI1_data_configCh2.DFSEL=SPI_FMT_0; // antes en SPI_FMT_1: modo 2: Select the Data word format by setting DFSEL bits. Select the Number of the configured SPIFMTx register (0 to 3) to used for the communication. Note: It is highly recommended to use SPIDAT1 register, SPIDAT0 is supported for compatibility reason
    SPI1_data_configCh2.CSNR=SPI_CS_0;//0xFB; // con este controlamos el slave chip select 1

    ComandoSPI[0]=((0x7F & 0xF4)<<8)|0x00AB; // ENCENDEMOS y activamos sobremuestreos
    spiSendAndGetData(spiREG_BMP, &SPI1_data_configCh2,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(500000);

    ComandoSPI[0]=((0x7F & 0xF5)<<8)|0x0014; // ENCENDEMOS y activamos sobremuestreos
    spiSendAndGetData(spiREG_BMP, &SPI1_data_configCh2,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(100000);

    CAlibracion_BMP280(spiREG_BMP, SPI1_data_configCh2);

}

/**
 * Regresa la presion atmosferica detectada por el BMP280
 */
int BMP280_Presion (void)
{
    return (int) PRESION[1];
}

/**
 * Regresa la altitud (en metros) calculada a base de la presion
 * (y temperatura) por el BMP280
 */
float BMP280_Altitud (void)
{
    return ALTITUD[1];
}

/**
 * Regresa la temperatura (en grados Celsius) detectada
 * por el BMP280
 */
float BMP280_Temperatura (void)
{
    return TEMPERATURA[1];
}

void CAlibracion_BMP280(spiBASE_t *spi, spiDAT1_t dataconfig_t)
{
    int ciclosHacerNada =100;

    ComandoSPI[0]=((0x80)|(0x88 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x89 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_t1=(unsigned short)((DatoSPI02[0]<<8)+DatoSPI01[0]);

    ComandoSPI[0]=((0x80)|(0x8A & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x8B & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_t2=(short)((DatoSPI02[0]<<8)+DatoSPI01[0]);


    ComandoSPI[0]=((0x80)|(0x8C & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x8D & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_t3=(short)((DatoSPI02[0]<<8)+DatoSPI01[0]);

    ComandoSPI[0]=((0x80)|(0x8E & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x8F & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_p1=(unsigned short)((DatoSPI02[0]<<8)+DatoSPI01[0]);

    ComandoSPI[0]=((0x80)|(0x90 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x91 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_p2=(short)((DatoSPI02[0]<<8)+DatoSPI01[0]);


    ComandoSPI[0]=((0x80)|(0x92 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x93 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_p3=(short)((DatoSPI02[0]<<8)+DatoSPI01[0]);

    ComandoSPI[0]=((0x80)|(0x94 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x95 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_p4=(short)((DatoSPI02[0]<<8)+DatoSPI01[0]);

    ComandoSPI[0]=((0x80)|(0x96 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x97 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_p5=(short)((DatoSPI02[0]<<8)+DatoSPI01[0]);

    ComandoSPI[0]=((0x80)|(0x98 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x99 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_p6=(short)((DatoSPI02[0]<<8)+DatoSPI01[0]);

    ComandoSPI[0]=((0x80)|(0x9A & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x9B & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_p7=(short)((DatoSPI02[0]<<8)+DatoSPI01[0]);


    ComandoSPI[0]=((0x80)|(0x9C & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x9D & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_p8=(short)((DatoSPI02[0]<<8)+DatoSPI01[0]);

    ComandoSPI[0]=((0x80)|(0x9E & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(ciclosHacerNada);
    DatoSPI01[0]=(DatoSPI01[0]&0x00FF);

    ComandoSPI[0]=((0x80)|(0x9F & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
    hacernada(ciclosHacerNada);
    DatoSPI02[0]=(DatoSPI02[0]&0x00FF);

    ParametrosCalibracionBMP280.dig_p9=(short)((DatoSPI02[0]<<8)+DatoSPI01[0]);
}

float BMP280_CalcularAltitud (float P, float TK)
{
    return TK * Const_aire * ((float) log (p0_def / P));
}

void LEERTempYpresRAW_bmp280(spiBASE_t *spi, spiDAT1_t dataconfig_t,int hacernadaCICLOS, float *pres, float *temp, float *alt)
{
    unsigned int datoRecibidoSPI=0;
    unsigned int datoRecibidoSPI1=0;
    unsigned int datoRecibidoSPI2=0;

    ComandoSPI[0]=((0x80)|(0xF3&0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI01);
    hacernada(hacernadaCICLOS);
    STATUS_BMP280=(DatoSPI01[0]&0x00FF);
    //sciEnviarDatos(sprintf(command, "  ---> Status=0x%X",  STATUS_BMP280),command, 0);

    hacernada(hacernadaCICLOS);

    if((STATUS_BMP280&0x0001)==0)
    {
        // Leemos temperatura
        ComandoSPI[0]=((0x80)|(0xFA & 0x7F))<<8; //
        spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
        hacernada(hacernadaCICLOS);

        ComandoSPI[0]=((0x80)|(0xFB & 0x7F))<<8; //
        spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI03);
        hacernada(hacernadaCICLOS);

        ComandoSPI[0]=((0x80)|(0xFC & 0x7F))<<8; //
        spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI04);
        hacernada(hacernadaCICLOS);

        datoRecibidoSPI=(DatoSPI02[0]&0x00FF);
        datoRecibidoSPI1=(DatoSPI03[0]&0x00FF);
        datoRecibidoSPI2=((DatoSPI04[0]&0x00FF)>>4);
        datoRecibidoSPI=(datoRecibidoSPI<<12) + (datoRecibidoSPI1<<4) + (datoRecibidoSPI2);
        Temp_bmp280[0]=(double)datoRecibidoSPI;
        //sciEnviarDatos(sprintf(command, "   TMP= %f",  Temp_bmp280[0]),command, 0);

        // Leemos presión
        ComandoSPI[0]=((0x80)|(0xF7)&(0x7F))<<8; //
        spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI02);
        hacernada(hacernadaCICLOS);

        ComandoSPI[0]=((0x80)|(0xF8)&(0x7F))<<8; //
        spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI03);
        hacernada(hacernadaCICLOS);

        ComandoSPI[0]=((0x80)|(0xF9 & 0x7F))<<8; //
        spiSendAndGetData(spi, &dataconfig_t,(uint32) 1, ComandoSPI,DatoSPI04);
        hacernada(hacernadaCICLOS);

        datoRecibidoSPI=(DatoSPI02[0]&0x00FF);
        datoRecibidoSPI1=(DatoSPI03[0]&0x00FF);
        datoRecibidoSPI2=((DatoSPI04[0]&0x00FF)>>4);

        datoRecibidoSPI=(datoRecibidoSPI<<12) + (datoRecibidoSPI1<<4) + (datoRecibidoSPI2);
        Pres_bmp280[0]=(double)datoRecibidoSPI; // OJO esta presión No debe usarse fuera de la función

        CalcTempYpres_bmp280(pres, temp,alt);
        //pres=Pres_bmp280[1];// aquí tenemos la presión en bares
        //temp=Temp_bmp280[1];// aquí tenemos la temperatura en grados centígrados
    }
    // Leemos  *    ID    *
    // ComandoSPI[0]=((0x80)|(0xD0 & 0x7F))<<8; // ENCENDEMOS, byte alto registro byte bajo valor
    // spiSendAndGetData(spiREG_BMP, &SPI1_data_configCh1,(uint32) 1, ComandoSPI,DatoSPI05);
    // hacernada(ciclosHacerNada);
    // DatoSPI05[0]=(DatoSPI05[0]&0x00FF);
    //sciEnviarDatos(sprintf(command, "  ->> ID=0x%X <<-",  DatoSPI05[0]),command, 1);

}

void CalcTempYpres_bmp280(float *pres,  float *temp, float *alt)
{
    int32_t var1;
    int32_t var2;
    int32_t temperature = 0;

    int64_t pressure = 0;
    int64_t varp1;
    int64_t varp2;
    //   **     **     **  //
    var1 = ((((((int)temp[0]) >> 3) - ((int32_t) ParametrosCalibracionBMP280.dig_t1 << 1))) * ((int32_t) ParametrosCalibracionBMP280.dig_t2)) >> 11;
    var2 = (((((((int)temp[0]) >> 4) - ((int32_t) ParametrosCalibracionBMP280.dig_t1)) * ((((int)temp[0]) >> 4) - ((int32_t) ParametrosCalibracionBMP280.dig_t1))) >> 12) * ((int32_t) ParametrosCalibracionBMP280.dig_t3)) >> 14;
    ParametrosCalibracionBMP280.t_fine = var1 + var2;
    temperature=(ParametrosCalibracionBMP280.t_fine * 5 + 128) >> 8;
    temp[1] = ((float)(temperature)/100.0);

    //   **     **     **  //

    varp1 = ((int64_t) (ParametrosCalibracionBMP280.t_fine)) - 128000;
    varp2 = varp1 * varp1 * (int64_t) ParametrosCalibracionBMP280.dig_p6;
    varp2 = varp2 + ((varp1 * (int64_t) ParametrosCalibracionBMP280.dig_p5) << 17);
    varp2 = varp2 + (((int64_t) ParametrosCalibracionBMP280.dig_p4) << 35);
    varp1 = ((varp1 * varp1 * (int64_t) ParametrosCalibracionBMP280.dig_p3) >> 8)  + ((varp1 * (int64_t) ParametrosCalibracionBMP280.dig_p2) << 12);
    varp1 = ((INT64_C(0x800000000000) + varp1) * ((int64_t) ParametrosCalibracionBMP280.dig_p1)) >> 33;
    if (varp1 != 0)
    {
        pressure = 1048576 - ((uint32_t)pres[0]);
        pressure = (((pressure << 31) - varp2) * 3125) / varp1;
        varp1 = (((int64_t) ParametrosCalibracionBMP280.dig_p9) * (pressure >> 13) * (pressure >> 13)) >> 25;
        varp2 = (((int64_t) ParametrosCalibracionBMP280.dig_p8) * pressure) >> 19;
        pressure = ((pressure + varp1 + varp2) >> 8) + (((int64_t) ParametrosCalibracionBMP280.dig_p7) << 4);
        //PRESION=(uint32_t) pressure;
        pres[1]=((float)pressure)/256.0;
    }
    else
    {
        pressure = 0;
    }
    alt[0]=alt[1];
    alt[1]=CalcAltitudfloat((float) pres[1], 273.15+(float)temp[1]);// temperatura en grados kelvin
}
void BMP280_CalcularTemperaturaPresion (void)
{
    int32_t var1;
    int32_t var2;
    int32_t temperature = 0;

    int64_t pressure = 0;
    int64_t varp1;
    int64_t varp2;
    //   ******     ******     ******  //
    var1 = ((((((int) TEMPERATURA[0]) >> 3)
              - ((int32_t) ParametrosCalibracionBMP280.dig_t1 << 1)))
            * ((int32_t) ParametrosCalibracionBMP280.dig_t2)) >> 11;
    var2 = (((((((int) TEMPERATURA[0]) >> 4)
               - ((int32_t) ParametrosCalibracionBMP280.dig_t1))
              * ((((int) TEMPERATURA[0]) >> 4)
                 - ((int32_t) ParametrosCalibracionBMP280.dig_t1))) >> 12)
            * ((int32_t) ParametrosCalibracionBMP280.dig_t3)) >> 14;
    ParametrosCalibracionBMP280.t_fine = var1 + var2;
    temperature = (ParametrosCalibracionBMP280.t_fine * 5 + 128) >> 8;
    TEMPERATURA[1] = ((float) (temperature) / 100.0);

    varp1 = ((int64_t) (ParametrosCalibracionBMP280.t_fine)) - 128000;
    varp2 = varp1 * varp1 * (int64_t) ParametrosCalibracionBMP280.dig_p6;
    varp2 = varp2
            + ((varp1 * (int64_t) ParametrosCalibracionBMP280.dig_p5) << 17);
    varp2 = varp2 + (((int64_t) ParametrosCalibracionBMP280.dig_p4) << 35);
    varp1 =
        ((varp1 * varp1 * (int64_t) ParametrosCalibracionBMP280.dig_p3) >> 8)
        + ((varp1 * (int64_t) ParametrosCalibracionBMP280.dig_p2)
           << 12);
    varp1 = ((INT64_C (0x800000000000) + varp1)
             * ((int64_t) ParametrosCalibracionBMP280.dig_p1)) >> 33;
    if (varp1 != 0) {
        pressure = 1048576 - ((uint32) PRESION[0]);
        pressure = (((pressure << 31) - varp2) * 3125) / varp1;
        varp1 = (((int64_t) ParametrosCalibracionBMP280.dig_p9)
                 * (pressure >> 13) * (pressure >> 13)) >> 25;
        varp2 = (((int64_t) ParametrosCalibracionBMP280.dig_p8) * pressure)
                >> 19;
        pressure = ((pressure + varp1 + varp2) >> 8)
                   + (((int64_t) ParametrosCalibracionBMP280.dig_p7) << 4);
        PRESION[1] = ((float) pressure) / 256.0;
    } else
        pressure = 0;

    ALTITUD[0] = ALTITUD[1];
    ALTITUD[1] = BMP280_CalcularAltitud ((float) PRESION[1],
                                         273.15 + (float) TEMPERATURA[1]); // temperatura en grados kelvin
}

float CalcAltitudfloat(float P, float TK)// temperatura en grados kelvin
{

    return TK*Const_aire*log(p0_def/P);

}
