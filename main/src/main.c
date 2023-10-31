/**
 * @file main.c
 * @author Péricles Buarque de Gusmão Filho (periclesbgf@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <gpio.h>
#include <dht11.h>
#include <led_rgb.h>
#include <inmp441.h>
#include <driver/i2s_std.h>

static const char *TAG = "HomeAssistant";

#define I2S_WS GPIO_NUM_3
#define I2S_SD 13
#define I2S_SCK GPIO_NUM_2
#define I2S_PORT I2S_NUM_0
#define bufferLen 64
#define SAMPLE_RATE 44100
#define RECORD_TIME (15)

#define LED_PIN 2
#define FOREVER while(1)

#define SPI_DMA_CHAN        SPI_DMA_CH_AUTO
#define NUM_CHANNELS        (1) // For mono recording only!
#define SAMPLE_SIZE         (1024)
#define BYTE_RATE           (SAMPLE_RATE * (24 / 8)) * NUM_CHANNELS

i2s_chan_handle_t rx_handle = NULL;

size_t bytes_read;
const int WAVE_HEADER_SIZE = 44;

/**
 * @brief 
 * 
 */
static void init_microphone(void)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));

    i2s_std_config_t pdm_rx_cfg =
    {
        .clk_cfg = {
            .sample_rate_hz = CONFIG_EXAMPLE_SAMPLE_RATE,
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .mclk_multiple = I2S_MCLK_MULTIPLE_384,
        },
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_24BIT, I2S_SLOT_MODE_MONO),
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

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_handle, &pdm_rx_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));
}

/**
 * @brief 
 * 
 * @param rec_time 
 */
void i2s_example_pdm_rx_task(void *args)
{
    while (1)
    {
        uint32_t *r_buf = (uint32_t *)malloc(bufferLen * sizeof(uint32_t));
        memset(r_buf, 0, bufferLen * sizeof(uint32_t));
        assert(r_buf);

        size_t r_bytes = 0;
        /* Read i2s data */
        if (i2s_channel_read(rx_handle, r_buf, bufferLen, &r_bytes, portMAX_DELAY) == ESP_OK)
        {
            int32_t sample_buff = 0;

            /*
            printf("Read Task: i2s read %u bytes\n-----------------------------------\n", r_bytes);
            printf("[0] %x [1] %x [2] %x \n\n", r_buf[0], r_buf[1], r_buf[2]);
            */

            int samples_read = r_bytes / 8;
            if(samples_read >= 0)
            {
                float mean = 0.0;
                for (int i = 0; i < samples_read; ++i)
                {
                    sample_buff = r_buf[i];

                    mean += (sample_buff);
                }
                //printf("sample_buff %ld\n-----------------------------------\n", sample_buff);
                mean /= samples_read;

                printf("mean: %.0f\n", mean/10000);
            }
        }
        else
        {
            printf("Read Task: i2s read failed\n");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
        free(r_buf);
    }
}
/**
 * @brief example
 * 
 */
void app_main(void)
{
    ESP_LOGI(TAG, "Initializating...");
    gpio_set_level(GPIO_USER_PURPLE_LED_PIN, LOW);
    gpio_set_level(GPIO_USER_GREEN_LED_PIN, LOW);

    esp_err_t status = ESP_FAIL;
    gpio_configure();
    ESP_LOGI(TAG, "Initializating Microphone");
    init_microphone();
    ESP_LOGI(TAG, "Microphone initialized");
    gpio_set_level(GPIO_USER_GREEN_LED_PIN, HIGH);
    //função para inicializar o driver
    //status = (gpio_configure());
    ESP_LOGI(TAG, "Microphone initialized");

    i2s_example_pdm_rx_task(15);
    //xTaskCreatePinnedToCore(task_read_temperature, "task_read_temperature", 1024, NULL, 2, NULL, tskNO_AFFINITY);
    //xTaskCreatePinnedToCore(task_user_led, "task_user_led", 1024, NULL, 2, NULL, tskNO_AFFINITY);
    //xTaskCreatePinnedToCore(task_read_voice, "task_read_voice", 1024, NULL, 2 , NULL, tskNO_AFFINITY);
    //xTaskCreatePinnedToCore(task_user_led_RGB, "task_user_led_RGB", 1024, NULL, 2, NULL, tskNO_AFFINITY);
}