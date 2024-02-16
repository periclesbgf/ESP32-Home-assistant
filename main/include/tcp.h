/**
 * @file tcp.h
 * @author Péricles Buarque de Gusmão Filho (periclesbgf@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _LED_TCP_H_
#define _LED_TCP_H_

#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"

#define HOST_IP_ADDR "192.168.1.12" //Substitua pelo IP do servidor em python
#define PORT 12445

int tcp_client(void);

#endif
