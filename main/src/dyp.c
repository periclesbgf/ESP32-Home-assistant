/**
 * @file dyp.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
/*#include <dyp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <limits.h>

esp_err_t dyp_configure(void)
{
    //for security we will wait 1 second for the sensor
    vTaskDelay(1000/portTICK_PERIOD_MS);

    ESP_LOGI(tag, "CONFIGURANDO GPIO LED RGB");

    gpio_config_t io_conf = {};

    //desabilitar interrupção
    io_conf.intr_type = GPIO_INTR_DISABLE;

    //selecionar o modo dos pinos(output ou input)
    io_conf.mode = GPIO_MODE_OUTPUT;

    //selecionar quais pinos iremos configurar
    io_conf.pin_bit_mask = ((1<<GPIO_USER_GREEN_RGB_LED_PIN)|
                            (1<<GPIO_USER_RED_RGB_LED_PIN)|
                            (1<<GPIO_USER_BLUE_RGB_LED_PIN));

    //configurar os resistores
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    return gpio_config(&io_conf);

    ESP_LOGI(tag, "Configuring dht11 sensor");

    return ESP_OK;
}
*/



