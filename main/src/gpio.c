/**
 * @file gpio.c
 * @author Péricles Buarque de Gusmão Filho (periclesbgf@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-07-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <gpioo.h>
#include <esp_log.h>

static const char *tag = "GPIO";

/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t gpio_configure(void)
{
    ESP_LOGI(tag, "CONFIGURANDO LED");

    gpio_config_t io_conf = {};

    //desabilitar interrupção
    io_conf.intr_type = GPIO_INTR_DISABLE;

    //selecionar o modo dos pinos(output ou input)
    io_conf.mode = GPIO_MODE_OUTPUT;

    //selecionar quais pinos iremos configurar
    io_conf.pin_bit_mask = ((1<<GPIO_USER_LED_PIN)|(1<<GPIO_USER_GREEN_LED_PIN)|(1<<GPIO_USER_GREEN_LED_PIN_3)|(1<<GPIO_USER_GREEN_LED_PIN_2)|(1<<GPIO_USER_RED_LED_PIN)|(1<<GPIO_USER_BLUE_LED_PIN)|(1<<LUMINARIA));

    //configurar os resistores
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    return ESP_OK;
}

/**
 * @brief
 *
 * @return esp_err_t
 */
esp_err_t basic_gpio_set(void)
{
    ESP_LOGI(tag, "CONFIGURANDO LED");

    if(
        gpio_set_level(GPIO_USER_LED_PIN, HIGH) ||
        gpio_set_level(GPIO_USER_GREEN_LED_PIN, HIGH) ||
        gpio_set_level(GPIO_USER_GREEN_LED_PIN_3, HIGH) ||
        gpio_set_level(GPIO_USER_GREEN_LED_PIN_2, HIGH) ||
        gpio_set_level(GPIO_USER_RED_LED_PIN, HIGH) ||
        gpio_set_level(GPIO_USER_BLUE_LED_PIN, HIGH) ||
        gpio_set_level(LUMINARIA, HIGH)
    )
    {
        ESP_LOGE(tag, "Erro ao setar os pinos");
        return ESP_FAIL;
    }

    return ESP_OK;
}