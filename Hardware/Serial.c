#include "stm32f10x.h"


void Serial_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitStructure);


    USART_Cmd(USART1, ENABLE);
}



void Serial_SendByte(uint8_t Byte)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

    USART_SendData(USART1, Byte);
}



void Serial_SendString(char *String)
{
    uint16_t i = 0;

    while(String[i] != '\0')
    {
        Serial_SendByte(String[i]);

        i++;
    }
}



void Serial_SendNumber(uint16_t Number)
{
    if(Number >= 1000)
    {
        Serial_SendByte(Number / 1000 + '0');
    }

    if(Number >= 100)
    {
        Serial_SendByte(Number / 100 % 10 + '0');
    }

    if(Number >= 10)
    {
        Serial_SendByte(Number / 10 % 10 + '0');
    }

    Serial_SendByte(Number % 10 + '0');
}