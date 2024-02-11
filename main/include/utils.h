/**
 * @file utils.h
 * @author Péricles Buarque de Gusmão Filho (periclesbgf@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <gpio.h>
#include <dht11.h>
#include <led_rgb.h>
#include <inmp441.h>
#include <driver/i2s_std.h>
#include <wifih.h>
#include <esp_event.h>
#include <tcp.h>
#include <udp.h>
#include "freertos/semphr.h"
#include "esp_spiffs.h"
#include <webserver.h>
#include "nvs_flash.h"
#include "esp_wifi.h"


#define True 1
#define False 0


#endif
