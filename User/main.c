#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "DHT11.h"


int main(void)
{
    uint8_t temp_int;
    uint8_t temp_dec;
    uint8_t hum_int;
    uint8_t hum_dec;


    OLED_Init();
    DHT11_Init();


    while(1)
    {
        if(DHT11_ReadData(&temp_int, &temp_dec,
                          &hum_int, &hum_dec))
        {
            OLED_ShowString(1,1,"Temp:");
            OLED_ShowNum(1,6,temp_int,2);
            OLED_ShowChar(1,8,'.');
            OLED_ShowNum(1,9,temp_dec,1);


            OLED_ShowString(2,1,"Humi:");
            OLED_ShowNum(2,6,hum_int,2);
            OLED_ShowChar(2,8,'.');
            OLED_ShowNum(2,9,hum_dec,1);
        }
        else
        {
            OLED_ShowString(1,1,"DHT11 Error");
        }


        Delay_ms(1000);
    }
}