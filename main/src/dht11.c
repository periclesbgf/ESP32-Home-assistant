/**
 * @file dht11.c
 * @author Péricles Buarque de Gusmão Filho (periclesbgf@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <dht11.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <limits.h>

static const char *tag = "DHT11";

/**
 * @brief 
 * 
 * @return esp_err_t 
 */
esp_err_t dht11_configure(void)
{
    //for security we will wait 1 second for the sensor
    vTaskDelay(1000/portTICK_PERIOD_MS);

    ESP_LOGI(tag, "Configuring dht11 sensor");

    return ESP_OK;
}

/**
 * @brief 
 * 
 * @return esp_err_t 
 */
static esp_err_t _dht11_send_start_signal(void)
{
    gpio_set_direction(GPIO_DHT11_PIN, GPIO_MODE_OUTPUT); // set the pin to output mode according do datasheet
    gpio_set_level(GPIO_DHT11_PIN, 0);  //set the level to zero

    ets_delay_us(20000); //delay time according to datasheet

    gpio_set_level(GPIO_DHT11_PIN, 1);  //set the level to zero
    ets_delay_us(40);

    gpio_set_direction(GPIO_DHT11_PIN, GPIO_MODE_INPUT);    // set the pin to input mode according do datasheet

    /*after that the sensor will keep sending data for 80us then it will pull up the gpio_level,
    then keeps it for 80us that pulls down and start sending data*/

    return ESP_OK;
}

/**
 * @brief 
 * 
 * @param timeout Timeout expected on dht11 sensor
 * @param level   Expected returned level of dht11
 * @return esp_err_t  return status
 */
static uint16_t _dht11_wait_input_level(uint16_t timeout, int level)
{
    uint16_t wating_time = 0;

    while(gpio_get_level(GPIO_DHT11_PIN) == level)
    {
        if(wating_time > timeout)
        {
            return MAX_TIMEOUT;
        }
        wating_time += 1;
        ets_delay_us(1);
    }

    return wating_time;
}

/**
 * @brief 
 * 
 * @return esp_err_t 
 */
esp_err_t _dht11_receive_start_signal(void)
{
    if(_dht11_wait_input_level(DHT11_TIMEOUT, LOW) == MAX_TIMEOUT)
    {
        return ESP_ERR_TIMEOUT;
    }

    if(_dht11_wait_input_level(DHT11_TIMEOUT, HIGH) == MAX_TIMEOUT)
    {
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}

/**
 * @brief 
 * 
 * @return esp_err_t 
 */
esp_err_t dth11_read_statistics(void)
{
    uint8_t data[5] = {};
    uint16_t timeout = 0;
    uint16_t checksum = 0;

    if(_dht11_send_start_signal() == ESP_FAIL)
    {
        ESP_LOGE(tag, "ERROR sending DHT11 start signal");
        return ESP_FAIL;
    }

    if(_dht11_receive_start_signal() == ESP_ERR_TIMEOUT)
    {
        ESP_LOGE(tag, "ERROR receiving DHT11 start signal");
        return ESP_FAIL;
    }

    for (int i = 0; i < DHT11_READ_DATA_BITS; i++)
    {

        if((_dht11_wait_input_level(DHT11_FIRST_BIT_DATA_TIMEOUT, LOW) == MAX_TIMEOUT))
        {
            return ESP_ERR_TIMEOUT;
        }
        timeout = _dht11_wait_input_level(DHT11_DATA_BIT_1, HIGH);
        if(timeout == MAX_TIMEOUT)
        {
            return ESP_ERR_TIMEOUT;
        }

        if(timeout > DHT11_DATA_BIT_0)
        {
            data[i/8] |= (1<<(7-(i%8)));
        }
    }

    for (int i = 0; i < 4; i++)
    {
        checksum += data[i];
    }

    if (checksum == data[4])
    {
        ESP_LOGI(tag, "Temperature %i.%i C Humidity %i.%i %% Rh", data[2], data[3], data[0], data[1]);
    }

    return ESP_OK;
}

