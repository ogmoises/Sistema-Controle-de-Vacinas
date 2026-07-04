#include "leds.h"
#include "utils.h"

void ledsInit(void)
{
    LEDS_DIR |= (LED_TEMP | LED_UMID | LED_LUZ);    // LEDs são saída
    LEDS_OUT &= ~(LED_TEMP | LED_UMID | LED_LUZ);   // Começam desligados
}

void verificarAlarmes(unsigned int temp,unsigned int luz, unsigned int umid, uint8_t temp_habilitada)
{
    static uint8_t led_temp_estado = 0;
    static uint8_t led_luz_estado  = 0;
    static uint8_t led_umid_estado = 0;

    // LED DE TEMPERATURA
    if(temp_habilitada)
    {
        if(!led_temp_estado)
        {
            // Liga se sair da faixa segura
            if(temp < TEMP_MIN || temp > TEMP_MAX)
                led_temp_estado = 1;
        }
        else
        {
            // Só desliga quando voltar totalmente para dentro da histerese
            if(temp >= TEMP_MIN_DESLIGA &&
               temp <= TEMP_MAX_DESLIGA)
            {
                led_temp_estado = 0;
            }
        }
    }
    else
    {
        led_temp_estado = 0;
    }

    if(led_temp_estado)
        LEDS_OUT |= LED_TEMP;
    else
        LEDS_OUT &= ~LED_TEMP;


    // LED DE LUMINOSIDADE
    if(!led_luz_estado)
    {
        if(luz >= LUZ_MAX)
            led_luz_estado = 1;
    }
    else
    {
        if(luz <= LUZ_MAX_DESLIGA)
            led_luz_estado = 0;
    }

    if(led_luz_estado)
        LEDS_OUT |= LED_LUZ;
    else
        LEDS_OUT &= ~LED_LUZ;


    // LED DE UMIDADE
    if(!led_umid_estado)
    {
        if(umid < UMID_MIN || umid > UMID_MAX)
            led_umid_estado = 1;
    }
    else
    {
        if(umid >= UMID_MIN_DESLIGA &&
           umid <= UMID_MAX_DESLIGA)
        {
            led_umid_estado = 0;
        }
    }

    if(led_umid_estado)
        LEDS_OUT |= LED_UMID;
    else
        LEDS_OUT &= ~LED_UMID;
}
