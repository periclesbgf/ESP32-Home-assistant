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
#define ESP_WPA3_SAE_PWE_BOTH
#define ESP_WIFI_PW_ID ""
#define ESP_MAXIMUM_RETRY 5

#define SSID_LENGTH 32
#define PASSWORD_LENGTH 64

esp_err_t retrieve_credentials(char *ssid, char *password);
esp_err_t wifi_init_sta();

#endif