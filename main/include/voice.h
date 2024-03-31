/**
 * @file voice.h
 * @author Péricles Buarque de Gusmão Filho (periclesbgf@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _VOICE_H_
#define _VOCIE_H_

#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "inmp441.h"

#define EXAMPLE_STD_BCLK_IO1        GPIO_NUM_17      // I2S bit clock io number   I2S_BCLK
#define EXAMPLE_STD_WS_IO1          GPIO_NUM_19      // I2S word select io number    I2S_LRC
#define EXAMPLE_STD_DOUT_IO1        GPIO_NUM_15     // I2S data out io number    I2S_DOUT
#define EXAMPLE_STD_DIN_IO1         GPIO_NUM_NC    // I2S data in io number

#define EXAMPLE_BUFF_SIZE               2048
#define SAMPLE_RATE_SUB                     44100

void i2s_subwoofer_write(void *args);
esp_err_t init_subwoofer(void);

#endif