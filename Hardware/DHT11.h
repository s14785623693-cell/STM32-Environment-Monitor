#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f10x.h"

#define DHT11_GPIO_PORT    GPIOA
#define DHT11_GPIO_PIN     GPIO_Pin_0
#define DHT11_GPIO_CLK     RCC_APB2Periph_GPIOA

#define DHT11_DQ_IN        GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN)

#define DHT11_DQ_OUT(x)    GPIO_WriteBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN, \
                                         (BitAction)(x))

void DHT11_Init(void);

uint8_t DHT11_ReadData(uint8_t *humi, uint8_t *temp);

#endif
