#include "stm32f10x.h"
#include "Delay.h"
#include "DHT11.h"


/**
  * 函    数：DHT11 GPIO输出模式
  */
static void DHT11_GPIO_Out(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}


/**
  * 函    数：DHT11 GPIO输入模式
  */
static void DHT11_GPIO_In(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(DHT11_GPIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = DHT11_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStructure);
}


/**
  * 函    数：DHT11发送开始信号
  */
static void DHT11_Start(void)
{
    DHT11_GPIO_Out();

    // 主机拉低20ms
    DHT11_DQ_OUT(0);
    Delay_ms(20);

    // 主机释放总线
    DHT11_DQ_OUT(1);
    Delay_us(30);
}


/**
  * 函    数：DHT11检测响应
  * 返回值：0：正常
  *         1：错误
  */
static uint8_t DHT11_CheckResponse(void)
{
    uint8_t retry = 0;

    DHT11_GPIO_In();

    // 等待DHT11拉低
    while (DHT11_DQ_IN && retry < 100)
    {
        retry++;
        Delay_us(1);
    }

    if (retry >= 100)
    {
        return 1;
    }

    retry = 0;

    // 等待DHT11拉高
    while (!DHT11_DQ_IN && retry < 100)
    {
        retry++;
        Delay_us(1);
    }

    if (retry >= 100)
    {
        return 1;
    }

    return 0;
}


/**
  * 函    数：读取DHT11一位数据
  * 返回值：0或1
  */
static uint8_t DHT11_ReadBit(void)
{
    uint8_t retry = 0;

    // 等待低电平结束
    while (DHT11_DQ_IN && retry < 100)
    {
        retry++;
        Delay_us(1);
    }

    retry = 0;

    // 等待高电平开始
    while (!DHT11_DQ_IN && retry < 100)
    {
        retry++;
        Delay_us(1);
    }

    // 延时40us后采样
    Delay_us(40);

    if (DHT11_DQ_IN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/**
  * 函    数：读取DHT11一个字节
  * 返回值：读取到的数据
  */
static uint8_t DHT11_ReadByte(void)
{
    uint8_t i;
    uint8_t data = 0;

    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        data |= DHT11_ReadBit();
    }

    return data;
}


/**
  * 函    数：读取DHT11温湿度数据
  * 参数：humi 湿度
  *       temp 温度
  * 返回值：0：读取成功
  *         1：读取失败
  */
uint8_t DHT11_ReadData(uint8_t *humi, uint8_t *temp)
{
    uint8_t data[5];
    uint8_t i;

    // 发送开始信号
    DHT11_Start();

    // 检测DHT11响应
    if (DHT11_CheckResponse() != 0)
    {
        return 1;
    }

    // 读取5个字节
    for (i = 0; i < 5; i++)
    {
        data[i] = DHT11_ReadByte();
    }

    // 校验数据
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
    {
        return 1;
    }

    // 获取湿度和温度整数部分
    *humi = data[0];
    *temp = data[2];

    return 0;
}


/**
  * 函    数：DHT11初始化
  */
void DHT11_Init(void)
{
    DHT11_GPIO_Out();

    // 数据线默认拉高
    DHT11_DQ_OUT(1);

    // 等待DHT11稳定
    Delay_ms(1000);
}
