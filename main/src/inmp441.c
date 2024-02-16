/**
 * @file inmp441.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <inmp441.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

static const char *TAG = "INMP441";

i2s_chan_handle_t rx_handle = NULL;

