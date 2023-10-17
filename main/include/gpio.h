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

// definir macros
#define GPIO_DHT11_PIN     GPIO_NUM_5 //Definindo o pino do sensor de temperatura como o GPIO_5 de acordo com o datasheet
#define GPIO_USER_LED_PIN  GPIO_NUM_2 //Definindo o pino do LED como o GPIO_2 de acordo com o datasheet
#define GPIO_USER_GREEN_LED_PIN  GPIO_NUM_18
//#define GPIO_USER_GREEN_RGB_LED_PIN  GPIO_NUM_22
//#define GPIO_USER_RED_RGB_LED_PIN  GPIO_NUM_21
//#define GPIO_USER_BLUE_RGB_LED_PIN  GPIO_NUM_19
//#define GPIO_USER_DYP  GPIO_NUM_23
#define GPIO_KY38_ANALOG_PIN GPIO_NUM_4
#define GPIO_KY38_DIGITAL_PIN GPIO_NUM_23
#define UART_NUM UART_NUM_0

#define HIGH 1
#define LOW 0

// prototipo das funcoes
esp_err_t gpio_configure(void);
esp_err_t gpio_dht11_configure(void);
esp_err_t gpio_configure_led_rgb(void);
esp_err_t gpio_ky38_configure(void);
esp_err_t uart_configure(void);





#endif/* _GPIO_H_ */
