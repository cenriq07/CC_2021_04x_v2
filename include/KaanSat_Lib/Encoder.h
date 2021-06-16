#ifndef ENCODER_H
#define ENCODER_H

#include "spi.h"
#include "FreeRTOS.h"
#include "os_task.h"

extern void EncoderInit();
extern float EncoderRead();

#endif
