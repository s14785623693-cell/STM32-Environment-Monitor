#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "HCSR04.h"


float Distance;


int main(void)
{
    OLED_Init();

    HCSR04_Init();


    OLED_ShowString(1, 1, "HC-SR04 TEST");


    while(1)
    {
        Distance = HCSR04_GetDistance();


        OLED_ShowString(2, 1, "Distance:");


        if(Distance == 0)
        {
            OLED_ShowString(3, 1, "ERROR");
        }
        else
        {
            OLED_ShowNum(3, 1, (uint16_t)Distance, 3);
            OLED_ShowString(3, 5, "cm");
        }


        Delay_ms(200);
    }
}
