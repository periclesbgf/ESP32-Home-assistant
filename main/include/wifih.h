/**
 * @file wifig.h
 * @author Péricles Buarque de Gusmão Filho (periclesbgf@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _WIFI_H_
#define _WIFI_H_

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <freertos/event_groups.h>
#include <nvs_flash.h>
#include <string.h>
#include <lwip/sockets.h>

#define ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SSID "ssid"
#define ESP_WIFI_PASSWORD "password"
#define ESP_WPA3_SAE_PWE_BOTH
#define ESP_WIFI_PW_ID ""
#define ESP_MAXIMUM_RETRY 5

void wifi_init_sta(void);

#endif