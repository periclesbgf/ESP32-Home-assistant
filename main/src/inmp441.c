#if 0
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


static const char *tag = "INMP441";

uint16_t counter = ((I2S_SAMPLE_RATE / BUFFERDEPTH) + 1);

int16_t sBuffer[bufferLen];

esp_err_t i2s_install()
{
    i2s_driver_config_t i2s_config = {};
  // Set up I2S Processor configuration
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX);
    i2s_config.sample_rate = I2S_SAMPLE_RATE;
    i2s_config.bits_per_sample = (i2s_bits_per_sample_t)16;
    i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    i2s_config.communication_format =
        (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S | I2S_COMM_FORMAT_I2S_MSB);
    i2s_config.intr_alloc_flags = 0;
    i2s_config.dma_buf_count = DMA_BUFFER_COUNT;
    i2s_config.dma_buf_len = DMA_BUFFER_LEN;
    i2s_config.use_apll = false;
    i2s_config.tx_desc_auto_clear = false;
    i2s_config.fixed_mclk = 0;

  ESP_LOGI(tag, "After initializing i2s_install");

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    ESP_LOGI(tag, "i2s_install");
  return ESP_OK;
}

esp_err_t i2s_setpin()
{
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);

  ESP_LOGI(tag, "i2s_setpin");

  return ESP_OK;
}

void inmp441_collect_sample(int16_t *data)
{
    ESP_LOGI(tag, "inmp441_collect_sample");
    size_t bytesRead = 0;
    i2s_read(I2S_NUM_0, &data, sizeof(int16_t) * BUFFERDEPTH, &bytesRead, TICKS_TO_WAIT);

}
#endif