/**
 * @file gpio.h
 * @author Péricles Buarque de Gusmão Filho (periclesbgf@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*Macros de Segurança*/
#ifndef _GPIO_H_
#define _GPIO_H_

// incluir bibliotecas
#include <esp_err.h>
#include <driver/gpio.h>
#include <dht11.h>
#include <rom/ets_sys.h>
#include "driver/uart.h"

#define GPIO_DHT11_PIN     GPIO_NUM_5
#define GPIO_USER_LED_PIN  GPIO_NUM_2
#define GPIO_USER_GREEN_LED_PIN  GPIO_NUM_22
#define GPIO_USER_GREEN_LED_PIN_2  GPIO_NUM_21
#define GPIO_USER_GREEN_LED_PIN_3 GPIO_NUM_23
#define GPIO_USER_BLUE_LED_PIN GPIO_NUM_12
#define GPIO_USER_RED_LED_PIN GPIO_NUM_4
#define LUMINARIA GPIO_NUM_18
#define LED_PIN 2

#define HIGH 1
#define LOW 0

esp_err_t gpio_configure(void);
esp_err_t gpio_dht11_configure(void);
esp_err_t gpio_configure_led_rgb(void);

#endif
