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
#include <dyp.h>
#include <ky38.h>

#define LED_PIN 2
#define FOREVER while(1)

const uart_port_t uart_num = UART_NUM_1;

static const char *tag = "HomeAssistant";

/**
 * @brief 
 * 
 * @param args 
 */
static void task_user_led(void *args)
{
    uint32_t level = 0;

    FOREVER
    {
        gpio_set_level(GPIO_USER_LED_PIN, level);
        level ^= 1;
        vTaskDelay(1500/portTICK_PERIOD_MS);
    }

}

/**
 * @brief 
 * 
 * @param args 
 */
static void task_read_temperature(void *args)
{
    while(1)
    {
        unsigned char data;

        if (uart_read_bytes(UART_NUM, &data, 1, portMAX_DELAY) > 0) {
            // representation of T and t
            if (data == 0x54 || data == 0x74) {
                dth11_read_statistics();
                vTaskDelay(1500/portTICK_PERIOD_MS);
            }
        }
    }
}

static void task_read_sound(void *args)
{
    FOREVER
    {
        if(ky38_read_sound() == ESP_OK)
        {
            gpio_set_level(GPIO_USER_GREEN_LED_PIN, HIGH);
        }
        else
        {
            gpio_set_level(GPIO_USER_GREEN_LED_PIN, LOW);
        }
    }
}

/**
 * @brief example
 * 
 */
void app_main(void)
{
    ESP_LOGI(tag, "Initializating...");

    esp_err_t status = ESP_FAIL;

    //função para inicializar o driver
    status = (gpio_configure() || dht11_configure() || uart_configure());

    if(status == ESP_FAIL)
    {
        ESP_LOGE(tag, "pin configuration error");
    }
    else
    {
        gpio_set_level(GPIO_USER_GREEN_LED_PIN, HIGH);
        //xTaskCreatePinnedToCore(task_user_led_green, "task_user_led_green", 1024, NULL, 2, NULL, tskNO_AFFINITY);
    }

    xTaskCreatePinnedToCore(task_user_led, "task_user_led", 1024, NULL, 2, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(task_read_temperature, "task_read_temperature", 4096, NULL, 10, NULL, tskNO_AFFINITY);
}