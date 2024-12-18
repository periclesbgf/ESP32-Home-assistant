/**
 * @file led_rgb.h
 * @author Péricles Buarque de Gusmão Filho (periclesbgf@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _LED_RGB_H_
#define _LED_RGB_H_

#include <gpioo.h>

typedef struct led_rgb_t
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}led_rgb;

#endif