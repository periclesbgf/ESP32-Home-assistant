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
/*
static void task_user_led_green(void *args)
{
    uint32_t level = 1;

    FOREVER
    {
        gpio_set_level(GPIO_USER_GREEN_LED_PIN, level);
        level ^= 1;
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
*/

static void task_user_led_RGB(void *args)
{
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    gpio_set_level(GPIO_USER_RED_RGB_LED_PIN, red);
    gpio_set_level(GPIO_USER_GREEN_RGB_LED_PIN, green);
    gpio_set_level(GPIO_USER_BLUE_RGB_LED_PIN, blue);
    FOREVER
    {
        red ^= 1;
        gpio_set_level(GPIO_USER_RED_RGB_LED_PIN, red);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        red ^= 1;
        gpio_set_level(GPIO_USER_RED_RGB_LED_PIN, red);
        green ^= 1;
        gpio_set_level(GPIO_USER_GREEN_RGB_LED_PIN, green);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        green ^= 1;
        gpio_set_level(GPIO_USER_GREEN_RGB_LED_PIN, green);
        blue ^= 1;
        gpio_set_level(GPIO_USER_BLUE_RGB_LED_PIN, blue);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        blue ^= 1;
        gpio_set_level(GPIO_USER_BLUE_RGB_LED_PIN, blue);
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
    status = (gpio_configure() || gpio_dht11_configure());

    if(status == ESP_FAIL)
    {
        ESP_LOGE(tag, "pin configuration error");
    }
    else
    {
        gpio_set_level(GPIO_USER_GREEN_LED_PIN, 1);
        //xTaskCreatePinnedToCore(task_user_led_green, "task_user_led_green", 1024, NULL, 2, NULL, tskNO_AFFINITY);
    }



    xTaskCreatePinnedToCore(task_user_led, "task_user_led", 1024, NULL, 2, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(task_user_led_RGB, "task_user_led_RGB", 1024, NULL, 2, NULL, tskNO_AFFINITY);
    
}