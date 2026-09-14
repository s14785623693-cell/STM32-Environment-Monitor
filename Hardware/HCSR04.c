#include "stm32f10x.h"
#include "HCSR04.h"
#include "Delay.h"


#define HCSR04_TRIG_PORT GPIOA
#define HCSR04_TRIG_PIN GPIO_Pin_1

#define HCSR04_ECHO_PORT GPIOA
#define HCSR04_ECHO_PIN GPIO_Pin_6


static uint16_t StartTime;
static uint16_t EchoTime;

static uint8_t CaptureState;
static uint8_t MeasureFinish;



/**
  * 函    数：HC-SR04 GPIO初始化
  */
static void HCSR04_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);


    // TRIG输出
    GPIO_InitStructure.GPIO_Pin = HCSR04_TRIG_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(HCSR04_TRIG_PORT, &GPIO_InitStructure);


    // ECHO输入
    GPIO_InitStructure.GPIO_Pin = HCSR04_ECHO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(HCSR04_ECHO_PORT, &GPIO_InitStructure);


    GPIO_ResetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN);
}



/**
  * 函    数：TIM3初始化
  * 1MHz计数频率
  * 1个CNT = 1us
  */
static void HCSR04_TIM_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);


    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;


    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);


    TIM_Cmd(TIM3, ENABLE);
}



/**
  * 函    数：输入捕获初始化
  */
static void HCSR04_IC_Init(void)
{
    TIM_ICInitTypeDef TIM_ICInitStructure;


    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0;


    TIM_ICInit(TIM3, &TIM_ICInitStructure);
}



/**
  * 函    数：切换捕获边沿
  */
static void HCSR04_SetCaptureEdge(uint16_t Edge)
{
    TIM_ICInitTypeDef TIM_ICInitStructure;


    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = Edge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0;


    TIM_ICInit(TIM3, &TIM_ICInitStructure);
}



/**
  * 函    数：NVIC初始化
  */
static void HCSR04_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;


    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;


    NVIC_Init(&NVIC_InitStructure);


    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
}



/**
  * 函    数：HC-SR04初始化
  */
void HCSR04_Init(void)
{
    HCSR04_GPIO_Init();
    HCSR04_TIM_Init();
    HCSR04_IC_Init();
    HCSR04_NVIC_Init();
}



/**
  * 函    数：发送触发信号
  */
static void HCSR04_Trig(void)
{
    GPIO_SetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN);

    Delay_us(10);

    GPIO_ResetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN);
}



/**
  * 函    数：获取距离
  * 返回值：距离(cm)
  *          失败返回0
  */
float HCSR04_GetDistance(void)
{
    uint32_t Timeout = 30000;


    EchoTime = 0;
    MeasureFinish = 0;


    HCSR04_Trig();


    while(!MeasureFinish && Timeout--)
    {
        Delay_us(1);
    }


    if(Timeout == 0)
    {
        return 0;
    }


    return EchoTime * 0.034f / 2;
}



/**
  * 函    数：TIM3中断
  */
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET)
    {

        if(CaptureState == 0)
        {
            StartTime = TIM_GetCapture1(TIM3);

            CaptureState = 1;

            HCSR04_SetCaptureEdge(TIM_ICPolarity_Falling);
        }
        else
        {
            EchoTime = TIM_GetCapture1(TIM3) - StartTime;

            MeasureFinish = 1;

            CaptureState = 0;

            HCSR04_SetCaptureEdge(TIM_ICPolarity_Rising);
        }


        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
    }
}