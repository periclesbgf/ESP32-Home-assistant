#include <stdio.h>
#include "utils.h"
#include "inmp441.h"
#include "voice.h"
#include "mqtt_client_eden.h"

static const char *TAG = "HomeAssistant";

i2s_chan_handle_t rx_handle = NULL;

i2s_chan_handle_t tx_chan = NULL;

size_t bytes_read;
const int WAVE_HEADER_SIZE = 16;
SemaphoreHandle_t sema_tcp;
int semaforo = 1;

int aws_iot_demo_main( int argc, char ** argv );


esp_err_t init_subwoofer(void) {
    ESP_LOGI("voice", "init_subwoofer");
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &tx_chan, NULL));


    i2s_std_config_t tx_std_cfg = {
            .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(25000),
            .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT,
                                                        I2S_SLOT_MODE_MONO),

            .gpio_cfg = {
                    .mclk = I2S_GPIO_UNUSED,    // some codecs may require mclk signal, this example doesn't need it
                    .bclk = EXAMPLE_STD_BCLK_IO1,
                    .ws   = EXAMPLE_STD_WS_IO1,
                    .dout = EXAMPLE_STD_DOUT_IO1,
                    .din  = EXAMPLE_STD_DIN_IO1,
                    .invert_flags = {
                            .mclk_inv = false,
                            .bclk_inv = false,
                            .ws_inv   = false,
                    },
            },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &tx_std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));

    return ESP_OK;
}


/**
 * @brief Initializes the microphone for recording.
 *
 * Configures the I2S channel for audio recording. If there's a failure in configuration,
 * an error message is logged, and the program is terminated.
 */
esp_err_t init_microphone(void)
{
    ESP_LOGI(TAG, "Microphone initializing...");
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
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

    return ESP_OK;
}

/**
 * @brief Task for transmitting audio over UDP.
 *
 * This task creates a UDP socket and continuously transmits audio data.
 * If there's an error in socket creation or transmission failure, the task is terminated.
 *
 * @param args Arguments for the task (not used).
 */
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

    char host_ip[HOST_IP_LENGTH];
    ESP_LOGI(TAG, "wifi_init_sta Initializing WIFI");
    retrieve_credentials(NULL, NULL, host_ip);
    ESP_LOGI(TAG, "wifi_init_sta finished. host_ip: %s", host_ip);

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
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

    ESP_LOGI(TAG, "Socket created, sending to %s:%d", host_ip, PORT);

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

/**
 * @brief Task for transmitting audio over TCP.
 *
 * This task creates a TCP socket and transmits audio data to a specified server.
 * If there's an error in socket creation or connection, the task is terminated.
 *
 * @param args Arguments for the task (not used).
 */
void i2s_example_tcp_stream_task(void *args)
{
    char host_ip[HOST_IP_LENGTH];
    ESP_LOGI(TAG, "wifi_init_sta Initializing WIFI");
    retrieve_credentials(NULL, NULL, host_ip);
    ESP_LOGI(TAG, "wifi_init_sta finished. host_ip: %s", host_ip);

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
            inet_aton(host_ip, &server_address.sin_addr);  // Substitua pelo endereço IP do seu servidor

            if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
            {
                ESP_LOGE(TAG, "Erro ao conectar ao servidor");
                close(sock);
                vTaskDelete(NULL);
            }
            gpio_set_level(GPIO_USER_GREEN_LED_PIN, HIGH);
            gpio_set_level(GPIO_USER_GREEN_LED_PIN_2, HIGH);
            gpio_set_level(GPIO_USER_GREEN_LED_PIN_3, HIGH);
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
                vTaskDelay(pdMS_TO_TICKS(6));
            }
            vTaskDelay(10);
            close(sock);
            free(r_buf);

            ESP_LOGI(TAG, "Envio concluído. Dados enviados para o servidor via TCP.");
            gpio_set_level(GPIO_USER_GREEN_LED_PIN, LOW);
            gpio_set_level(GPIO_USER_GREEN_LED_PIN_2, LOW);
            gpio_set_level(GPIO_USER_GREEN_LED_PIN_3, LOW);
            semaforo = 1;
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(3000));
        }
    }
}

/**
 * @brief TCP Server Task.
 *
 * This task creates a TCP server that listens for connections and processes received commands.
 * If there's an error in socket creation or in listening for connections, the task is terminated.
 *
 * @param pvParameters Parameters for the task (not used).
 */
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
            size_t w_bytes = 0;
            buffer[bytes_received] = '\0';

            if (strcmp(buffer, "eden") == 0)
            {
                semaforo = 2;
                vTaskDelay(10);
                ESP_LOGI(TAG, "Recebido: %s", buffer);
            }
            else if(strcmp(buffer, "ll1") == 0)
            {
                gpio_set_level(GPIO_USER_RED_LED_PIN, HIGH);
                ESP_LOGI(TAG, "Recebido: %s", buffer);
            }
            else if(strcmp(buffer, "dl1") == 0)
            {
                gpio_set_level(GPIO_USER_RED_LED_PIN, LOW);
                ESP_LOGI(TAG, "Recebido: %s", buffer);
            }
            else if(strcmp(buffer, "ll2") == 0)
            {
                gpio_set_level(LUMINARIA, HIGH);
                ESP_LOGI(TAG, "Recebido: %s", buffer);
            }
            else if(strcmp(buffer, "dl2") == 0)
            {
                gpio_set_level(LUMINARIA, LOW);
                ESP_LOGI(TAG, "Recebido: %s", buffer);
            }
            else
            {
                if (i2s_channel_write(tx_chan, buffer, bytes_received, &w_bytes, 1000) == ESP_OK) {
                    printf("Write Task: i2s write %d bytes\n", w_bytes);
                } else {
                    printf("Write Task: i2s write failed\n");
                }
            }
        }
        //vTaskDelay(200);

        close(client_sock);
        ESP_LOGI(TAG, "Cliente desconectado");
    }
}


esp_err_t initial_tcp_client_task(void) {

    char host_ip[HOST_IP_LENGTH];
    ESP_LOGI(TAG, "Retrieving credentials...");
    retrieve_credentials(NULL, NULL, host_ip);
    ESP_LOGI(TAG, "Credentials Retrieved. host_ip: %s", host_ip);

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(12340);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return ESP_FAIL;
    }
    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Successfully connected");

    char *hello_msg = "Hello from ESP32!";
    send(sock, hello_msg, strlen(hello_msg), 0);

    char rx_buffer[128];

    int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    if (len > 0) {
        rx_buffer[len] = 0; // Null-terminate a string recebida
        printf("Received message: %s\n", rx_buffer);
    }

    shutdown(sock, 0);
    close(sock);
    //vTaskDelete(NULL);
    return ESP_OK;
}

/**
 * @brief Main entry point of the program.
 *
 * Initializes the system, configures the network and microphone, and creates necessary tasks.
 */
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


    if(wifi_init_softap() != ESP_OK)
    {
        ESP_LOGI(TAG, "Erro ao inicializar o Hotspot");
    }
    ESP_LOGI(TAG, "wifi_init_softap finished");

    if(wifi_init_sta() != ESP_OK)
    {
        ESP_LOGI(TAG, "Erro ao Conectar no WIFI");
    }
    ESP_LOGI(TAG, "wifi_init_sta finished");

    if(gpio_configure() != ESP_OK)
    {
        ESP_LOGI(TAG, "Erro ao configurar GPIOs");
    }
    ESP_LOGI(TAG, "gpio_configure finished");

    // if(init_microphone() != ESP_OK)
    // {
    //     ESP_LOGI(TAG, "Erro ao inicializar o microfone");
    // }
    // ESP_LOGI(TAG, "Microfone initialized successfully!");

    // if(init_subwoofer() != ESP_OK)
    // {
    //     ESP_LOGI(TAG, "Erro ao inicializar o subwoofer");
    // }

    //xTaskCreate(i2s_subwoofer_write, "i2s_subwoofer_write", 4096, NULL, 5, NULL);

    mqtt_app_start();
    mqtt_publish(NULL);

    // if (initial_tcp_client_task() != ESP_OK){
    //     ESP_LOGI(TAG, "Erro ao criar a tarefa do cliente TCP");
    // }
    ESP_LOGI(TAG, "initial_tcp_client_task finished");
    //vTaskDelay(10);
    //xTaskCreate(i2s_example_udp_stream_task, "i2s_example_udp_stream_task", 7168, NULL, 5, NULL);
    //xTaskCreate(i2s_example_tcp_stream_task, "i2s_example_tcp_stream_task", 7168, NULL, 5, NULL);
    //xTaskCreate(tcp_server_task, "tcp_server_task", 9216, NULL, 5, NULL);

}