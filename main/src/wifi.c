#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "wifih.h"

#include "lwip/err.h"
#include "lwip/sys.h"
/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_MAXIMUM_RETRY  ESP_MAXIMUM_RETRY
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH
#define EXAMPLE_H2E_IDENTIFIER ESP_WIFI_PW_ID


#ifdef ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#endif
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "wifi station";

static int s_retry_num = 0;


static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

esp_err_t retrieve_credentials(char *ssid, char *password) {

    // Abre o namespace NVS
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("wifi_config", NVS_READONLY, &nvs_handle));

    // Recupera as credenciais do NVS
    size_t ssid_size, password_size;
    ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "ssid", NULL, &ssid_size));
    ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "password", NULL, &password_size));

    if (ssid_size > 0 && password_size > 0) {
        ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "ssid", ssid, &ssid_size));
        ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "password", password, &password_size));
    }

    // Fecha o namespace NVS
    nvs_close(nvs_handle);

    return ESP_OK;
}

esp_err_t wifi_init_sta()
{

    char ssid[SSID_LENGTH];
    char password[PASSWORD_LENGTH];
    ESP_LOGI(TAG, "wifi_init_sta Initializing WIFI");
    retrieve_credentials(ssid, password);
    ESP_LOGI(TAG, "wifi_init_sta finished. SSID: %s password: %s", ssid, password);
    s_wifi_event_group = xEventGroupCreate();
    ESP_LOGI(TAG, "wifi_init_sta after xEventGroupCreate");
#if 0
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(TAG, "wifi_init_sta destroying default wifi netif");
    esp_netif_destroy_default_wifi(esp_netif_get_default_netif());
    ESP_LOGI(TAG, "wifi_init_sta after destroying default wifi netif");
#endif
    esp_netif_create_default_wifi_sta();
    ESP_LOGI(TAG, "wifi_init_sta after creating default wifi sta");
#if 0
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
#endif
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = "",
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
        },
    };

    strncpy((char *)wifi_config.sta.ssid, ssid, SSID_LENGTH);
    strncpy((char *)wifi_config.sta.password, password, PASSWORD_LENGTH);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s",
                 ssid);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s",
                 ssid);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    return ESP_OK;
}
#if 0
esp_err_t wifi_config_init()
{
    ESP_LOGI(TAG, "wifi_config_init");

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    return ESP_OK;
}
#endif