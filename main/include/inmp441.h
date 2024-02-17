/**
 * @file inmp441.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _INMP441_H_
#define _INMP441_H_

#include <gpio.h>
#include <esp_err.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_system.h"
#include <driver/i2s_std.h>
#include <driver/spi_common.h>

#define I2S_WS GPIO_NUM_3
#define I2S_SD GPIO_NUM_5
#define I2S_SCK GPIO_NUM_2
#define I2S_PORT I2S_NUM_0

#define bufferLen 64
#define SAMPLE_RATE 16000
#define RECORD_TIME 1
#define RECORD_TIME_RESPONSE 3
#define SPI_DMA_CHAN        SPI_DMA_CH_AUTO
#define NUM_CHANNELS        (1) // For mono recording only!
#define SAMPLE_SIZE         (1024)
#define BYTE_RATE           (SAMPLE_RATE * (24 / 8)) * NUM_CHANNELS

#endif