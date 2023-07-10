/**
 * @file ky38.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <ky38.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

static const char *tag = "KY38";

esp_err_t ky38_configure(void)
{
    //for security we will wait 1 second for the sensor
    vTaskDelay(1000/portTICK_PERIOD_MS);

    ESP_LOGI(tag, "Configuring ky38 sensor");

    gpio_set_direction(GPIO_KY38_ANALOG_PIN, GPIO_MODE_INPUT); // set the pin to output mode according do datasheet
    gpio_set_direction(GPIO_KY38_DIGITAL_PIN, GPIO_MODE_INPUT); // set the pin to output mode according do datasheet

    return ESP_OK;
}

esp_err_t ky38_read_sound(void)
{
    if(gpio_get_level(GPIO_KY38_ANALOG_PIN) == HIGH)
    {
        return ESP_OK;
    }
    return ESP_FAIL;
}