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

// definir macros
#define GPIO_DHT11_PIN     GPIO_NUM_5 //Definindo o pino do sensor de temperatura como o GPIO_5 de acordo com o datasheet
#define GPIO_USER_LED_PIN  GPIO_NUM_2 //Definindo o pino do LED como o GPIO_2 de acordo com o datasheet
#define GPIO_USER_GREEN_LED_PIN  GPIO_NUM_18
// prototipo das funcoes
esp_err_t gpio_configure(void);
esp_err_t gpio_dht11_configure(void);






#endif/* _GPIO_H_ */
