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

#define LED_PIN 2
#define FOREVER while(1)


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
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }


}

/**
 * @brief example
 * 
 */
void app_main(void)
{
    ESP_LOGI(tag, "Ola");
    ESP_LOGI(tag, "Size of int %d", sizeof(int));

    esp_err_t status = ESP_FAIL;

    //função para inicializar o driver
    status = gpio_configure();

    if(status == ESP_FAIL)
    {
        ESP_LOGE(tag, "pin configuration error");

    }

    xTaskCreatePinnedToCore(task_user_led, "task_user_led", 1024, NULL, 2, NULL, tskNO_AFFINITY);

}