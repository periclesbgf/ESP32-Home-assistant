/**
 * @file dht11.h
 * @author Péricles Buarque de Gusmão Filho (periclesbgf@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _DHT11_H_
#define _DHT11_H_

#include <gpio.h>
#include <esp_err.h>

#define DHT11_TIMEOUT 80
#define DHT11_FIRST_BIT_DATA_TIMEOUT 50
#define DHT11_DATA_BIT_0 28
#define DHT11_DATA_BIT_1 70
#define DHT11_READ_DATA_BITS 40
#define MAX_TIMEOUT 0xFFFF

esp_err_t dht11_configure(void);
esp_err_t dth11_read_statistics(void);

#endif// _DHT11_H_

