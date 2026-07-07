// Ventoinha gira com velocidade controlada a partir do duty cycle

#include "utils.h"
#include "pwm.h"
#include "leds.h" // Mínimos e máximos dos sensores
#include <stdint.h>

static unsigned int current_fan_speed = 0;

typedef enum
{
    FAN_FRIO,
    FAN_NORMAL,
    FAN_AQUECENDO,
    FAN_CRITICO

} FanState;

static FanState fanState = FAN_NORMAL;


void pwmInit(void)
{
    PWM_DIR |= PWM_PIN; // Pino é saída
    PWM_SEL |= PWM_PIN; // Pino é controlado pelo Timer A0

    TA0CCR0 = 100 - 1; // Freq = 10kHz

    // Canal 3 (P1.4)
    TA0CCTL3 = OUTMOD_7; // Reset/Set -> 0 até CCR3
    TA0CCR3 = 0;         // Duty cycle inicia em 0%

    // 4. Liga o Timer A0
    TA0CTL = TASSEL_2 | MC_1 | TACLR; // SMCLK / Modo UP / Zera o timer
}

void setFanSpeed(unsigned int porcentagem) // Atualiza velocidade com base na temperatura observada
{
    if (porcentagem > 100) porcentagem = 100; // Evita valores inválidos
    current_fan_speed = porcentagem;
    TA0CCR3 = porcentagem * 10; // Pega % e ajusta entre valores de 0 a 1000 - Atualiza duty cycle
}

unsigned int getFanSpeed(void)
{
    return current_fan_speed;
}

void controlarVentoinha(unsigned int temperatura_atual)
{

    // Atualiza o estado (Histerese)
    switch (fanState)
    {
        case FAN_FRIO:

            if (temperatura_atual >= TEMP_MIN_DESLIGA)
                fanState = FAN_NORMAL;

            break;


        case FAN_NORMAL:

            if (temperatura_atual < TEMP_MIN)
                fanState = FAN_FRIO;

            else if (temperatura_atual > TEMP_MAX)
                fanState = FAN_AQUECENDO;

            break;


        case FAN_AQUECENDO:

            if (temperatura_atual <= TEMP_MAX_DESLIGA)
                fanState = FAN_NORMAL;

            else if (temperatura_atual >= TEMP_CRITICA)
                fanState = FAN_CRITICO;

            break;


        case FAN_CRITICO:

            if (temperatura_atual <= TEMP_CRITICA_DESLIGA)
                fanState = FAN_AQUECENDO;

            break;
    }

    // Aplica o PWM conforme o estado
    switch (fanState)
    {
        case FAN_FRIO:

            setFanSpeed(0);
            break;


        case FAN_NORMAL:

            setFanSpeed(20);
            break;


        case FAN_AQUECENDO:
        {
            unsigned int range_temp = TEMP_CRITICA - TEMP_MAX;
            unsigned int desvio = temperatura_atual - TEMP_MAX;

            unsigned int porcentagem_pwm =
                    20 + ((desvio * 80) / range_temp);

            if (porcentagem_pwm > 100)
                porcentagem_pwm = 100;

            setFanSpeed(porcentagem_pwm);

            break;
        }


        case FAN_CRITICO:

            setFanSpeed(100);
            break;
    }
}
