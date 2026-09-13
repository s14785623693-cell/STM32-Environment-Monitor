#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"

uint8_t Humidity_Int;
uint8_t Humidity_Dec;

uint8_t Temperature_Int;
uint8_t Temperature_Dec;


int main(void)
{
    DHT11_Init();
    OLED_Init();

    OLED_ShowString(1, 1, "DHT11 TEST");

    while (1)
    {
        if (DHT11_ReadData(&Humidity_Int,
                           &Humidity_Dec,
                           &Temperature_Int,
                           &Temperature_Dec) == 0)
        {
            OLED_ShowString(2, 1, "Temp:");

            OLED_ShowNum(2, 6, Temperature_Int, 2);
            OLED_ShowString(2, 8, ".");
            OLED_ShowNum(2, 9, Temperature_Dec, 1);
            OLED_ShowString(2, 10, "C");

            OLED_ShowString(3, 1, "Humi:");

            OLED_ShowNum(3, 6, Humidity_Int, 2);
            OLED_ShowString(3, 8, ".");
            OLED_ShowNum(3, 9, Humidity_Dec, 1);
            OLED_ShowString(3, 10, "%");
        }
        else
        {
            OLED_ShowString(2, 1, "DHT11 ERROR");
            OLED_ShowString(3, 1, "             ");
        }

        Delay_ms(2000);
    }
}