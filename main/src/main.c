#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <gpio.h>
#include <dht11.h>
#include <led_rgb.h>
#include <inmp441.h>
#include <driver/i2s_std.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <wifih.h>
#include <esp_event.h>
#include <tcp.h>
#include <udp.h>
#include "freertos/semphr.h"
#include "esp_spiffs.h"

static const char *TAG = "HomeAssistant";

#define I2S_WS GPIO_NUM_3
#define I2S_SD 13
#define I2S_SCK GPIO_NUM_2
#define I2S_PORT I2S_NUM_0
#define bufferLen 64
#define SAMPLE_RATE 16000
#define RECORD_TIME 1
#define RECORD_TIME_RESPONSE 3

#define LED_PIN 2
#define FOREVER while(1)
#define SPIFFS_PATH "/spiffs/audio.wav"

#define SPI_DMA_CHAN        SPI_DMA_CH_AUTO
#define NUM_CHANNELS        (1) // For mono recording only!
#define SAMPLE_SIZE         (1024)
#define BYTE_RATE           (SAMPLE_RATE * (24 / 8)) * NUM_CHANNELS

i2s_chan_handle_t rx_handle = NULL;

size_t bytes_read;
const int WAVE_HEADER_SIZE = 16;
SemaphoreHandle_t sema4;
SemaphoreHandle_t sema_tcp;
int semaforo = 1;

void init_microphone(void)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    esp_err_t err = i2s_new_channel(&chan_cfg, NULL, &rx_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Erro ao criar canal I2S: %d", err);
        abort();  // Encerra o programa em caso de erro
    }

    i2s_std_config_t pdm_rx_cfg =
    {
        .clk_cfg = {
            .sample_rate_hz = SAMPLE_RATE,
            .clk_src = I2S_CLK_SRC_APLL,
            .mclk_multiple = I2S_MCLK_MULTIPLE_256,
        },
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg =
        {
            .bclk = I2S_SCK,
            .ws = I2S_WS,
            .din = I2S_SD,
            .dout = -1,
            .invert_flags =
            {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    err = i2s_channel_init_std_mode(rx_handle, &pdm_rx_cfg);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Erro ao inicializar modo padrão I2S: %d", err);
        abort();  // Encerra o programa em caso de erro
    }

    err = i2s_channel_enable(rx_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Erro ao habilitar canal I2S: %d", err);
        abort();  // Encerra o programa em caso de erro
    }
}

void i2s_example_udp_stream_task(void *args)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        ESP_LOGE(TAG, "Erro ao criar o socket UDP");
        vTaskDelete(NULL);
    }

    int addr_family = 0;
    int ip_protocol = 0;

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
    }

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

    ESP_LOGI(TAG, "Socket created, sending to %s:%d", HOST_IP_ADDR, PORT);

    while (1)
    {
        if (semaforo == 1)
        {
            size_t total_samples = SAMPLE_RATE * RECORD_TIME;
            size_t total_bytes = total_samples * sizeof(int16_t);

            int16_t *r_buf = (int16_t *)malloc(bufferLen);
            assert(r_buf);

            size_t bytes_sent = 0;
            while (bytes_sent < total_bytes)
            {
                size_t bytes_to_read = (bufferLen < total_bytes - bytes_sent) ? bufferLen : (total_bytes - bytes_sent);

                size_t r_bytes = 0;
                if (i2s_channel_read(rx_handle, r_buf, bytes_to_read, &r_bytes, portMAX_DELAY) != ESP_OK)
                {
                    ESP_LOGE(TAG, "Erro ao ler dados do microfone");
                    break;
                }

                ssize_t sent_bytes = sendto(sock, r_buf, r_bytes, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                if (sent_bytes < 0)
                {
                    ESP_LOGE(TAG, "Erro ao enviar dados para o servidor");
                    break;
                }

                bytes_sent += sent_bytes;
                // printf("%d\n", bytes_sent);
                vTaskDelay(pdMS_TO_TICKS(6));
            }
            vTaskDelay(10);
            free(r_buf);
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(3000));
        }
    }

    ESP_LOGI(TAG, "Envio concluído. Dados enviados para o servidor via UDP.");
    vTaskDelete(NULL);
}

void i2s_example_tcp_stream_task(void *args)
{
    vTaskDelay(10);

    while (1)
    {
        if (semaforo == 2)
        {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0)
            {
                ESP_LOGE(TAG, "Erro ao criar o socket TCP");
                vTaskDelete(NULL);
            }

            struct sockaddr_in server_address;
            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(12446);  // Substitua pelo número da porta do seu servidor
            inet_aton("192.168.1.3", &server_address.sin_addr);  // Substitua pelo endereço IP do seu servidor

            if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
            {
                ESP_LOGE(TAG, "Erro ao conectar ao servidor");
                close(sock);
                vTaskDelete(NULL);
            }
            gpio_set_level(GPIO_USER_GREEN_LED_PIN, HIGH);
            size_t total_samples = SAMPLE_RATE * RECORD_TIME_RESPONSE;
            size_t total_bytes = total_samples * sizeof(int16_t);

            int16_t *r_buf = (int16_t *)malloc(bufferLen);
            assert(r_buf);

            size_t bytes_sent = 0;

            while (bytes_sent < total_bytes)
            {
                size_t bytes_to_read = (bufferLen < total_bytes - bytes_sent) ? bufferLen : (total_bytes - bytes_sent);

                size_t r_bytes = 0;
                if (i2s_channel_read(rx_handle, r_buf, bytes_to_read, &r_bytes, portMAX_DELAY) != ESP_OK)
                {
                    ESP_LOGE(TAG, "Erro ao ler dados do microfone");
                    break;
                }

                ssize_t sent_bytes = send(sock, r_buf, r_bytes, 0);
                if (sent_bytes < 0)
                {
                    ESP_LOGE(TAG, "Erro ao enviar dados para o servidor");
                    break;
                }

                bytes_sent += sent_bytes;
                //printf("%d\n", bytes_sent);
                vTaskDelay(pdMS_TO_TICKS(6));
            }
            gpio_set_level(GPIO_USER_LED_PIN, LOW);
            vTaskDelay(10);
            close(sock);
            free(r_buf);

            ESP_LOGI(TAG, "Envio concluído. Dados enviados para o servidor via TCP.");
            gpio_set_level(GPIO_USER_GREEN_LED_PIN, LOW);
            semaforo = 1;
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(3000));
        }
    }
}


void tcp_server_task(void *pvParameters)
{
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        ESP_LOGE(TAG, "Erro ao criar o socket TCP do servidor");
        vTaskDelete(NULL);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);  // Substitua pelo número da porta desejado

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        ESP_LOGE(TAG, "Erro ao vincular o socket TCP do servidor");
        close(server_sock);
        vTaskDelete(NULL);
    }

    if (listen(server_sock, 5) < 0)
    {
        ESP_LOGE(TAG, "Erro ao escutar conexões TCP");
        close(server_sock);
        vTaskDelete(NULL);
    }

    ESP_LOGI(TAG, "Servidor TCP está escutando...");

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0)
        {
            ESP_LOGE(TAG, "Erro ao aceitar conexão TCP");
            continue;
        }

        ESP_LOGI(TAG, "Cliente conectado");

        char buffer[1024];
        ssize_t bytes_received;

        while ((bytes_received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0)
        {
            buffer[bytes_received] = '\0';
            ESP_LOGI(TAG, "Recebido: %s", buffer);
            if (strcmp(buffer, "eden") == 0)
            {
                // Libere o semáforo para continuar a execução da task i2s_example_udp_stream_task
                // Crie uma nova task para processar com um tempo de gravação diferente
                semaforo = 2;
                vTaskDelay(10);
                //xTaskCreate(i2s_example_tcp_stream_task, "i2s_example_tcp_stream_task", 7168, NULL, 5, NULL);
            }
            // Faça o que for necessário com a string recebida aqui
        }
        vTaskDelay(200);

        close(client_sock);
        ESP_LOGI(TAG, "Cliente desconectado");
    }
}


void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Inicializando...");
    wifi_init_sta();

    sema4 = xSemaphoreCreateBinary();
    sema_tcp = xSemaphoreCreateBinary();
    xSemaphoreGive(sema4);

    gpio_set_level(GPIO_USER_PURPLE_LED_PIN, LOW);
    gpio_set_level(GPIO_USER_GREEN_LED_PIN, LOW);
    gpio_set_level(GPIO_USER_LED_PIN, LOW);

    //esp_err_t status = ESP_FAIL;
    gpio_configure();
    ESP_LOGI(TAG, "Inicializando Microfone");

    ESP_ERROR_CHECK(esp_netif_init());

    init_microphone();

    ESP_LOGI(TAG, "Microfone inicializado");

    xTaskCreate(tcp_server_task, "tcp_server_task", 4096, NULL, 5, NULL);
    xTaskCreate(i2s_example_udp_stream_task, "i2s_example_udp_stream_task", 7168, NULL, 5, NULL);
    xTaskCreate(i2s_example_tcp_stream_task, "i2s_example_tcp_stream_task", 7168, NULL, 5, NULL);
    //vTaskStartScheduler();
}
