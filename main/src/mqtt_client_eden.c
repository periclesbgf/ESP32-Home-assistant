#include "mqtt_client_eden.h"
#include "gpioo.h"

static const char *TAG = "MQTT";

esp_mqtt_client_handle_t client;
int msg_id;


void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}


void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "luminaria/debug", 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("Received message: TOPIC=%.*s DATA=%.*s\r\n", event->topic_len, event->topic, event->data_len, event->data);
        // Chamar a função turn_led aqui
        char* message = malloc(event->data_len + 1); // Aloca memória para armazenar a mensagem recebida
        strncpy(message, event->data, event->data_len); // Copia os dados da mensagem para a variável
        message[event->data_len] = '\0'; // Adiciona o terminador de string
        turn_led(message); // Chama a função para tratar a mensagem
        free(message); // Libera a memória alocada
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
  const esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = ADRESS_URI,
    .broker.verification.certificate = (const char *)root_cert_auth_start,
    .credentials = {
      .authentication = {
        .certificate = (const char *)client_cert_start,
        .key = (const char *)client_key_start,
      },
    }
  };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void mqtt_publish(void *params){
    msg_id = esp_mqtt_client_subscribe(client, "luminaria/debug", 0);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

    // msg_id = esp_mqtt_client_publish(client, "luminaria/debug", "Test was send",0 ,0 ,0);
    // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

    //msg_id = esp_mqtt_client_unsubscribe(client, "topic/esp32/pub");
    //ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
}

void turn_led(char *data){
    if(strcmp(data, "on") == 0){
        gpio_set_level(LUMINARIA, 1);
    }else if(strcmp(data, "off") == 0){
        gpio_set_level(LUMINARIA, 0);
        gpio_set_level(GPIO_USER_GREEN_LED_PIN, 0);
        gpio_set_level(GPIO_USER_LED_PIN, 0);
        gpio_set_level(GPIO_USER_RED_LED_PIN, 0);
    }
}
