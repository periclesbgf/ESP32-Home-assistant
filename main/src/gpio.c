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
#include <gpio.h>
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

    return gpio_config(&io_conf);
}

/**
 * @brief 
 * 
 */
esp_err_t uart_configure(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    return uart_driver_install(UART_NUM, 256, 0, 0, NULL, 0);
}

/*
esp_err_t gpio_configure_led_rgb(void)
{
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
}
*/