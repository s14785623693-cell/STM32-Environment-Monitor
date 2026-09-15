#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "HCSR04.h"
#include "Serial.h"


uint8_t Humidity;
uint8_t Temperature;
float Distance;


int main(void)
{
    OLED_Init();
    DHT11_Init();
    HCSR04_Init();
    Serial_Init();


    OLED_ShowString(1, 1, "Environment");


    while(1)
    {
        // DHT11
        if(DHT11_ReadData(&Humidity, &Temperature) == 0)
        {
            OLED_ShowString(2, 1, "Temp:");
            OLED_ShowNum(2, 6, Temperature, 2);
            OLED_ShowString(2, 8, "C");


            OLED_ShowString(3, 1, "Humi:");
            OLED_ShowNum(3, 6, Humidity, 2);
            OLED_ShowString(3, 8, "%");
        }
        else
        {
            OLED_ShowString(2, 1, "Temp:ERROR");
            OLED_ShowString(3, 1, "Humi:ERROR");
        }


        // HC-SR04
        Distance = HCSR04_GetDistance();


        OLED_ShowString(4, 1, "Dist:");

        if(Distance == 0)
        {
            OLED_ShowString(4, 6, "ERROR");
        }
        else
        {
            OLED_ShowNum(4, 6, (uint16_t)Distance, 3);
            OLED_ShowString(4, 9, "cm");
        }


        // 串口输出
        Serial_SendString("Temp:");
        Serial_SendNumber(Temperature);

        Serial_SendString("C  ");


        Serial_SendString("Humi:");
        Serial_SendNumber(Humidity);

        Serial_SendString("%  ");


        Serial_SendString("Dist:");
        Serial_SendNumber((uint16_t)Distance);

        Serial_SendString("cm\r\n");


        Delay_ms(1000);
    }
}