#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#ifndef _WEB_SERVER_H_
#define _WEB_SERVER_H_

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN


esp_err_t post_handler(httpd_req_t*);
esp_err_t get_handler(httpd_req_t*);
httpd_handle_t start_webserver(void);
void stop_webserver(httpd_handle_t);
void wifi_init_softap(void);

#endif
