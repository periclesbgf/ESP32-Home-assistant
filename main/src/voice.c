#include "voice.h"
#include "utils.h"

#define EXAMPLE_BUFF_SIZE               2048

static i2s_chan_handle_t tx_chan;        // I2S tx channel handler
#if 0
void i2s_subwoofer_write(void *args) {


    uint16_t *buffer = calloc(1, EXAMPLE_BUFF_SIZE * 2);


    size_t w_bytes = 0;
    uint32_t offset = 0;
    while (1) {
        /* Write i2s data */
        if (i2s_channel_write(tx_chan, buffer, EXAMPLE_BUFF_SIZE * 2, &w_bytes, 1000) == ESP_OK) {
            printf("Write Task: i2s write %d bytes\n", w_bytes);
        } else {
            printf("Write Task: i2s write failed\n");
        }
        if (offset>(pcm_end-pcm_start)){
            break;
        }

        for (int i = 0; i < EXAMPLE_BUFF_SIZE; i++) {
            offset++;
            buffer[i] = pcm_start[offset]<<3;
        }
        printf("size %d\noffset %lu\n", pcm_end-pcm_start,offset);

    }
    ESP_ERROR_CHECK(i2s_channel_disable(tx_chan));

    vTaskDelete(NULL);
}



esp_err_t init_subwoofer(void) {
    ESP_LOGI("voice", "init_subwoofer");
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &tx_chan, NULL));


    i2s_std_config_t tx_std_cfg = {
            .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
            .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT,
                                                        I2S_SLOT_MODE_MONO),

            .gpio_cfg = {
                    .mclk = I2S_GPIO_UNUSED,    // some codecs may require mclk signal, this example doesn't need it
                    .bclk = EXAMPLE_STD_BCLK_IO1,
                    .ws   = EXAMPLE_STD_WS_IO1,
                    //.dout = EXAMPLE_STD_DOUT_IO1,
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
#endif