#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"

void DHT11_Init(void);

uint8_t DHT11_ReadData(uint8_t *temp_int, uint8_t *temp_dec,
                       uint8_t *hum_int, uint8_t *hum_dec);

#endif