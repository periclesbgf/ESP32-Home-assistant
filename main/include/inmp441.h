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
//#include <driver/i2s_pdm.h>
#include <driver/spi_common.h>




#if 0
#define I2S_SD GPIO_NUM_33
#define I2S_WS GPIO_NUM_25
#define I2S_SCK GPIO_NUM_32
#define I2S_PORT I2S_NUM_0

#define bufferCnt 10
#define bufferLen 64

uint32_t I2S_SAMPLE_RATE = 8000;
uint32_t DMA_BUFFER_LEN = 1024;
uint32_t TICKS_TO_WAIT = 1000;
int8_t NOT_USED = -1;
uint8_t I2S_WS_I2S0 = 25;
uint8_t I2S_SD_I2S0 = 33;
uint8_t I2S_SCK_I2S0 = 26;
uint8_t DMA_BUFFER_COUNT = 4;
uint16_t BUFFERDEPTH = 8000;


esp_err_t i2s_install(void);
esp_err_t i2s_setpin(void);
esp_err_t inmp441_setup(void);
void inmp441_collect_sample(int16_t *data)




#endif// _INMP441_H_
#endif