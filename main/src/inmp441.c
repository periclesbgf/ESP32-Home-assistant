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
#if 0

static const char *TAG = "INMP441";

i2s_chan_handle_t rx_handle = NULL;

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
        //xSemaphoreTake(sema4, portMAX_DELAY);
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

    ESP_LOGI(TAG, "Envio concluído. Dados enviados para o servidor via UDP.");
    vTaskDelete(NULL);
}

void i2s_example_tcp_stream_task(void *args)
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

    while (1)
    {
        xSemaphoreTake(sema4, portMAX_DELAY);
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

            vTaskDelay(10);
            //close(sock);
            free(r_buf);
        }
    }
    ESP_LOGI(TAG, "Envio concluído. Dados enviados para o servidor via TCP.");
    vTaskDelete(NULL);
}
#endif