#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"
#include "HCSR04.h"
#include "Serial.h"
#include "Buzzer.h"

uint8_t Humidity;
uint8_t Temperature;
float Distance;

int main(void)
{
    OLED_Init();
    DHT11_Init();
    HCSR04_Init();
    Serial_Init();
    Buzzer_Init();

    OLED_ShowString(1, 1, "Environment");

    while(1)
    {
        // DHT11
        if(DHT11_ReadData(&Humidity, &Temperature) == 0)
        {
            OLED_ShowString(2, 1, "T:");
            OLED_ShowNum(2, 3, Temperature, 2);
            OLED_ShowString(2, 5, "C");

            OLED_ShowString(2, 7, "H:");
            OLED_ShowNum(2, 9, Humidity, 2);
            OLED_ShowString(2, 11, "%");
        }
        else
        {
            OLED_ShowString(2, 1, "T:ERROR H:ERROR");
        }

        // HC-SR04
        Distance = HCSR04_GetDistance();

        OLED_ShowString(3, 1, "Dist:");

        if(Distance == 0)
        {
            OLED_ShowString(3, 6, "ERROR");
        }
        else
        {
            OLED_ShowNum(3, 6, (uint16_t)Distance, 3);
            OLED_ShowString(3, 9, "cm");
        }

        // 报警状态
        OLED_ShowString(4, 1, "Status:");

        if(Temperature > 35 || Humidity > 80 || Distance < 10)
        {
            Buzzer_ON();
            OLED_ShowString(4, 8, "ALERT");
        }
        else
        {
            Buzzer_OFF();
            OLED_ShowString(4, 8, "OK   ");
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
