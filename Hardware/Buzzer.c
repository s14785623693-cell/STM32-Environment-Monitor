#include "stm32f10x.h"
#include "Buzzer.h"

void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // PA2配置为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化时关闭蜂鸣器
    Buzzer_OFF();
}

void Buzzer_ON(void)
{
    // 低电平触发
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}

void Buzzer_OFF(void)
{
    // 高电平关闭
    GPIO_SetBits(GPIOA, GPIO_Pin_2);
}
