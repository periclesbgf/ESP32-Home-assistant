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

typedef struct
{
    uint8_t  chunk_id[4]; // "RIFF" (4 bytes)
    uint32_t chunk_size;   // Tamanho do arquivo menos 8 bytes (4 bytes)
    uint8_t  format[4];    // "WAVE" (4 bytes)
    uint8_t  subchunk1_id[4]; // "fmt " (4 bytes)
    uint32_t subchunk1_size;  // Tamanho do cabeçalho do formato (16 bytes) (4 bytes)
    uint16_t audio_format;    // Formato de áudio (1 para PCM) (2 bytes)
    uint16_t num_channels;    // Número de canais (1 para mono, 2 para estéreo) (2 bytes)
    uint32_t sample_rate;     // Taxa de amostragem (4 bytes)
    uint32_t byte_rate;       // Byte rate (número de bytes transmitidos por segundo) (4 bytes)
    uint16_t block_align;     // Número de bytes para um bloco de amostra, incluindo todos os canais (2 bytes)
    uint16_t bits_per_sample; // Número de bits por amostra (4 bytes)
    uint8_t  subchunk2_id[4]; // "data" (4 bytes)
    uint32_t subchunk2_size;  // Número de bytes nos dados (4 bytes)
} wav_header_t;


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