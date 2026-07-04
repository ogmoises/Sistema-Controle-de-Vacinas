#include "buzzer.h"
#include "utils.h"
#include "leds.h"

static uint8_t estado_alarme = 0; // 0 (Desligado), 1 (Esperando), 2 (Apitando)

// Estados para histerese
static uint8_t alarme_temp = 0;
static uint8_t alarme_luz  = 0;

void buzzerInit(void)
{
    BUZZER_DIR |= BUZZER_PIN;   // Saída 
    BUZZER_OUT &= ~BUZZER_PIN;  // Começa desligado

    TA1CTL = TASSEL_1 | ID_3 | MC_0 | TACLR; // TIMERA1:TASSEL_1 = ACLK (32768 Hz) - ID_3 = Divisor por 8 - TACLR = Zera o contador inicial
    
    TA1CCTL0 = CCIE; // Habilita a interrupção baseada na flag CCIFG
}

void gerenciarAlarmes(unsigned int luz, unsigned int temp, uint8_t sistema_termico_habilitado)
{
    uint8_t condicao_alarme = 0;

    // HISTERESE DA LUMINOSIDADE
    if (!alarme_luz)
    {
        if (luz >= LUZ_MAX)
            alarme_luz = 1;
    }
    else
    {
        if (luz <= LUZ_MAX_DESLIGA)
            alarme_luz = 0;
    }

    // HISTERESE DA TEMPERATURA
    if (sistema_termico_habilitado)
    {
        if (!alarme_temp)
        {
            if (temp < TEMP_MIN || temp >= TEMP_CRITICA)
                alarme_temp = 1;
        }
        else
        {
            if (temp >= TEMP_MIN_DESLIGA &&
                temp < TEMP_CRITICA_DESLIGA)
            {
                alarme_temp = 0;
            }
        }
    }
    else
    {
        alarme_temp = 0;
    }


    // CONDIÇÃO GERAL DO ALARME
    if (alarme_luz || alarme_temp)
            condicao_alarme = 1;

    if (condicao_alarme) // Se alguma das condições de perigo foi ativada
    {
        if (estado_alarme == 0)  // LIGA o Timer
        {
            estado_alarme = 1;                 // Alarme esperando tempo mínimo

            if (alarme_temp)
                TA1CCR0 = TEMPO_BUZZER_TEMP;
            else
                TA1CCR0 = TEMPO_BUZZER_LUZ;
            
            TA1CTL |= TACLR;                   // Garante que a contagem (TAR) comece do zero
            TA1CTL |= MC_1;                    // LIGA o Timer em Modo UP (MC_1)
        }   
    }
    else // Se todos os sensores voltaram ao normal e o sistema estava rodando:
    {
        if (estado_alarme != 0)        // DESLIGA imediatamente
        {
            estado_alarme = 0;
            TA1CTL &= ~MC_3;           // Zera os bits MC0 e MC1 (Para o Timer)
            TA1CTL |= TACLR;           // Zera o contador
            BUZZER_OUT &= ~BUZZER_PIN; // Garante que o pino não fique travado em HIGH
        }
    }
}

#pragma vector=TIMER1_A0_VECTOR         // Ocorre quando o alarme toca
__interrupt void Timer1_A0_ISR(void)
{
    if (estado_alarme == 1) // Passou o tempo de espera
    {
        estado_alarme = 2;         // Avança para o modo ligado
        TA1CCR0 = TEMPO_BIPE;   // Reconfigura o hardware para bipar a cada 0.2s
        TA1CTL |= TACLR;           // Zera o timer
        BUZZER_OUT |= BUZZER_PIN;  // Dá o primeiro bipe
    }
    else if (estado_alarme == 2)   // Alarme disparado
    {
        BUZZER_OUT ^= BUZZER_PIN;       // Alterna a onda sonora
    }
}
