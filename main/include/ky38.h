/**
 * @file ky38.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */
/*Macros de Segurança*/
#ifndef _KY38_H_
#define _KY38_H_

#include <gpio.h>
#include <esp_err.h>

esp_err_t ky38_read_sound(void);
esp_err_t ky38_configure(void);



#endif