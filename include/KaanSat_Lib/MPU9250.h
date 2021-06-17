/*
 * MPU9250.h
 *
 *  Created on: 07/05/2021
 *      Author: Enrique
 */

#include "sys_core.h"
#include "gio.h"
#include "het.h"
#include "spi.h"
#include "math.h"
#include "sci.h"
#include "adc.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef INCLUDE_MPU9250_H_
#define INCLUDE_MPU9250_H_

#define MPU_WRITE                       0x00
#define MPU_READ                        0x80
#define MPU_SELF_TEST_X_GYRO            0x00
#define MPU_SELF_TEST_Y_GYRO            0x01
#define MPU_SELF_TEST_Z_GYRO            0x02

#define MPU_AK8963_WIA                  0x00
#define MPU_AK8963_INFO                 0x01
#define MPU_AK8963_ST1                  0x02
#define MPU_AK8963_HXL                  0x03
#define MPU_AK8963_HXH                  0x04
#define MPU_AK8963_HYL                  0x05
#define MPU_AK8963_HYH                  0x06
#define MPU_AK8963_HZL                  0x07
#define MPU_AK8963_HZH                  0x08
#define MPU_AK8963_ST2                  0x09
#define MPU_AK8963_CNTL                 0x0A
#define MPU_AK8963_RSV                  0x0B
#define MPU_AK8963_CNTL2                0x0B
#define MPU_AK8963_ASTC                 0x0C
#define MPU_AK8963_TS1                  0x0D
#define MPU_AK8963_TS2                  0x0E
#define MPU_AK8963_I2CDIS               0x0F
#define MPU_AK8963_ASAX                 0x10
#define MPU_AK8963_ASAY                 0x11
#define MPU_AK8963_ASAZ                 0x12

#define MPU_AK8963_ADDRESS              0x0C


#define MPU_SELF_TEST_X_ACCEL           0x0D
#define MPU_SELF_TEST_Y_ACCEL           0x0E
#define MPU_SELF_TEST_Z_ACCEL           0x0F
#define MPU_XG_OFFSET_H                 0x13
#define MPU_XG_OFFSET_L                 0x14
#define MPU_YG_OFFSET_H                 0x15
#define MPU_YG_OFFSET_L                 0x16
#define MPU_ZG_OFFSET_H                 0x17
#define MPU_ZG_OFFSET_L                 0x18
#define MPU_SMPLRT_DIV                  0x19

#define MPU_CONFIG                      0x1A
#define MPU_GYRO_CONFIG                 0x1B
#define MPU_ACCEL_CONFIG                0x1C
#define MPU_ACCEL_CONFIG_2              0x1D
#define MPU_LP_ACCEL_ODR                0x1E
#define MPU_WOM_THR                     0x1F
#define MPU_FIFO_EN                     0x23
#define MPU_I2C_MST_CTRL                0x24
#define MPU_I2C_SLV0_ADDR               0x25
#define MPU_I2C_SLV0_REG                0x26
#define MPU_I2C_SLV0_CTRL               0x27
#define MPU_I2C_SLV1_ADDR               0x28
#define MPU_I2C_SLV1_REG                0x29
#define MPU_I2C_SLV1_CTRL               0x2A
#define MPU_I2C_SLV2_ADDR               0x2B
#define MPU_I2C_SLV2_REG                0x2C
#define MPU_I2C_SLV2_CTRL               0x2D
#define MPU_I2C_SLV3_ADDR               0x2E
#define MPU_I2C_SLV3_REG                0x2F
#define MPU_I2C_SLV3_CTRL               0x30
#define MPU_I2C_SLV4_ADDR               0x31
#define MPU_I2C_SLV4_REG                0x32
#define MPU_I2C_SLV4_DO                 0x33
#define MPU_I2C_SLV4_CTRL               0x34

#define MPU_I2C_SLV4_DI                 0x35
#define MPU_I2C_MST_STATUS              0x36
#define MPU_INT_PIN_CFG                 0x37
#define MPU_INT_ENABLE                  0x38
#define MPU_INT_STATUS                  0x3A
#define MPU_ACCEL_XOUT_H                0x3B
#define MPU_ACCEL_XOUT_L                0x3C
#define MPU_ACCEL_YOUT_H                0x3D
#define MPU_ACCEL_YOUT_L                0x3E
#define MPU_ACCEL_ZOUT_H                0x3F
#define MPU_ACCEL_ZOUT_L                0x40
#define MPU_TEMP_OUT_H                  0x41
#define MPU_TEMP_OUT_L                  0x42
#define MPU_GYRO_XOUT_H                 0x43
#define MPU_GYRO_XOUT_L                 0x44
#define MPU_GYRO_YOUT_H                 0x45
#define MPU_GYRO_YOUT_L                 0x46
#define MPU_GYRO_ZOUT_H                 0x47
#define MPU_GYRO_ZOUT_L                 0x48
#define MPU_EXT_SENS_DATA_00            0x49
#define MPU_EXT_SENS_DATA_01            0x4A
#define MPU_EXT_SENS_DATA_02            0x4B
#define MPU_EXT_SENS_DATA_03            0x4C
#define MPU_EXT_SENS_DATA_04            0x4D
#define MPU_EXT_SENS_DATA_05            0x4E
#define MPU_EXT_SENS_DATA_06            0x4F
#define MPU_EXT_SENS_DATA_07            0x50
#define MPU_EXT_SENS_DATA_08            0x51
#define MPU_EXT_SENS_DATA_09            0x52
#define MPU_EXT_SENS_DATA_10            0x53
#define MPU_EXT_SENS_DATA_11            0x54
#define MPU_EXT_SENS_DATA_12            0x55
#define MPU_EXT_SENS_DATA_13            0x56
#define MPU_EXT_SENS_DATA_14            0x57
#define MPU_EXT_SENS_DATA_15            0x58
#define MPU_EXT_SENS_DATA_16            0x59
#define MPU_EXT_SENS_DATA_17            0x5A
#define MPU_EXT_SENS_DATA_18            0x5B
#define MPU_EXT_SENS_DATA_19            0x5C
#define MPU_EXT_SENS_DATA_20            0x5D
#define MPU_EXT_SENS_DATA_21            0x5E
#define MPU_EXT_SENS_DATA_22            0x5F
#define MPU_EXT_SENS_DATA_23            0x60
#define MPU_I2C_SLV0_DO                 0x63
#define MPU_I2C_SLV1_DO                 0x64
#define MPU_I2C_SLV2_DO                 0x65
#define MPU_I2C_SLV3_DO                 0x66
#define MPU_I2C_MST_DELAY_CTRL          0x67
#define MPU_SIGNAL_PATH_RESET           0x68
#define MPU_MOT_DETECT_CTRL             0x69
#define MPU_USER_CTRL                   0x6A
#define MPU_PWR_MGMT_1                  0x6B
#define MPU_PWR_MGMT_2                  0x6C
#define MPU_FIFO_COUNTH                 0x72
#define MPU_FIFO_COUNTL                 0x73
#define MPU_FIFO_R_W                    0x74
#define MPU_WHO_AM_I                    0x75
#define MPU_XA_OFFSET_H                 0x77
#define MPU_XA_OFFSET_L                 0x78
#define MPU_YA_OFFSET_H                 0x7A
#define MPU_YA_OFFSET_L                 0x7B
#define MPU_ZA_OFFSET_H                 0x7D
#define MPU_ZA_OFFSET_L                 0x7E

spiDAT1_t   SPI2_data_configCh0;
spiDAT1_t   SPI2_data_configCh1;
spiDAT1_t   SPI2_data_configCh2;
spiDAT1_t   SPI2_data_configCh3;

spiDAT1_t   SPI1_data_configCh0;
spiDAT1_t   SPI3_data_configCh0;

extern uint16_t AK8963ReadOut[7];

enum{
    X,Y,Z
};

#define vX  -45.079
#define vY  53.760
#define vZ  -304.786
#define B   118.58

void MPUInit();
void wait(unsigned int id);
bool sciEnviarDatos(uint8 numOfDat, char * charDat, bool sc);
bool sciEnviarDatos01(sciBASE_t *registro_SCI_LIN, uint8 numOfDat, char * charDat, bool sc);
void     config_MPU_9250(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig);
void config_AK8963(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig);
void read_AK8963(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig, int16_t regRead,int16_t *ReadOut);
void write_AK8963(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig, int16_t regwrite, int16_t *ReadOut, int16_t dataWrite);
boolean mpuGetMagnetometer(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig, int16_t *ReadOut);

void config_AK8963_VF(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig );
void     config_MPU_9250_VF(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig );

void read_AK8963_VF(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig, int16_t regRead,int16_t *ReadOut);
void write_AK8963_VF(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig, int16_t regwrite, int16_t *ReadOut, int16_t dataWrite);
boolean read_MANG_AK8963_VF(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig, int16_t *ReadOut, boolean imprimir_SN);
boolean read_MPU_ACEL_TMP_VF(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig, int *ReadOut_TMP_Giro_Acc, boolean imprimir_SN_TMP, boolean imprimir_SN_GIRO, boolean imprimir_SN_ACEL);

boolean mpuGetGyroAccel(spiBASE_t *spiREGISTRO, spiDAT1_t *SPIconfig, int *Gyro, int *Accel);

#endif /* INCLUDE_MPU9250_H_ */

