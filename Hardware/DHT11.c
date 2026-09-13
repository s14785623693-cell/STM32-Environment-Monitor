#include "stm32f10x.h"
#include "Delay.h"
#include "DHT11.h"


// DHT11初始化
void DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 释放总线，让DATA保持高电平
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
}


// 发送开始信号
void DHT11_Start(void)
{
    // MCU主动拉低
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);

    // 保持低电平至少18ms
    Delay_ms(20);

    // 释放DATA
    GPIO_SetBits(GPIOA, GPIO_Pin_0);

    // 稍微等待
    Delay_us(30);
}


// 检查DHT11响应
uint8_t DHT11_CheckResponse(void)
{
    uint32_t timeout = 0;

    // 等待DHT11把DATA拉低
    while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
    {
        timeout++;

        if (timeout > 10000)
        {
            return 0;   // 响应失败
        }
    }

    timeout = 0;

    // 等待DHT11把DATA拉高
    while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
    {
        timeout++;

        if (timeout > 10000)
        {
            return 0;   // 响应失败
        }
    }

    return 1;   // 响应成功
}


// 读取1bit
uint8_t DHT11_ReadBit(void)
{
    // 等待DHT11结束约50us的低电平
    while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0);

    // 等待40us后采样
    Delay_us(40);

    // 判断此时DATA的电平
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


// 读取1个字节
uint8_t DHT11_ReadByte(void)
{
    uint8_t data = 0;

    for (int i = 0; i < 8; i++)
    {
        uint8_t bit = DHT11_ReadBit();

        data = (data << 1) | bit;
    }

    return data;
}


// 读取温湿度
uint8_t DHT11_ReadData(uint8_t *temp_int, uint8_t *temp_dec,
                       uint8_t *hum_int, uint8_t *hum_dec);
{
    uint8_t hum_int_data;
    uint8_t hum_dec_data;
    uint8_t temp_int_data;
    uint8_t temp_dec_data;
    uint8_t checksum;

    // 发送开始信号
    DHT11_Start();

    // 等待并检查DHT11响应
    if (DHT11_CheckResponse() == 0)
    {
        return;
    }

    // DHT11发送5个字节
    hum_int_data = DHT11_ReadByte();
    hum_dec_data = DHT11_ReadByte();
    temp_int_data = DHT11_ReadByte();
    temp_dec_data = DHT11_ReadByte();
    checksum = DHT11_ReadByte();

    // 校验失败
    if ((uint8_t)(hum_int_data + hum_dec_data +
                  temp_int_data + temp_dec_data) != checksum)
    {
        return 0;
    }


    // 校验成功，传回main
    *temp_int = temp_int_data;
    *temp_dec = temp_dec_data;

    *hum_int = hum_int_data;
    *hum_dec = hum_dec_data;

    return 1;
}

