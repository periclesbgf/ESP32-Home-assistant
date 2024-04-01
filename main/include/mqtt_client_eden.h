#ifndef MQTT_CLIENT_EDEN_H
#define MQTT_CLIENT_EDEN_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define ADRESS_URI CONFIG_ADRESS_URI


extern const char client_cert_start[] asm("_binary_client_crt_start");
extern const char client_key_start[] asm("_binary_client_key_start");
extern const char root_cert_auth_start[]   asm("_binary_AmazonRootCA1_pem_start");

void log_error_if_nonzero(const char *message, int error_code);

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

void mqtt_app_start(void);

void mqtt_publish(void *params);

#endif /* ifndef MQTT_CLIENT_EDEN_H */
