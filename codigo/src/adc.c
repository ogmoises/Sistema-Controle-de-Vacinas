#include <msp430.h>
#include <stdint.h>
#include "adc.h"
#include "utils.h"

void adcInit(void)
{
    ADC_SEL |= (LUZ | TEMP);            // 1. Configura os pinos físicos para funcionarem como entrada analógica
    
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;  // ADC12CTL0 e ADC12CTL1: Registradores de Controle ADC12
                                        // ADC12SHT0_2: tAmostragem = 16 ciclos / ADC12ON: liga o módulo ADC
    ADC12CTL1 = ADC12SHP;               // ADC12SHP: Pulso de amostragem será controlado pelo timer interno
    ADC12CTL2 = ADC12RES_2;             // ADC12CTL2: Registrador de configurações adicionais 
                                        // ADC12RES_2: Resolução de 12 bits (0 a 4095)
}

unsigned int readADC(uint8_t channel)
{
    ADC12CTL0 &= ~ADC12ENC;       // Desabilita ADC para mudar canal
    ADC12MCTL0 = channel;         // Escolhe canal a ser observado
    ADC12CTL0 |= ADC12ENC;        // Reabilita o ADC

   
    __delay_cycles(50);           // Espera para a chave analógica assentar fisicamente (50us)

    
    ADC12CTL0 |= ADC12SC;        // 1 LEITURA FANTASMA RÁPIDA (Drena o pino anterior)
    while (ADC12CTL1 & ADC12BUSY);
    volatile unsigned int lixo = ADC12MEM0; 

    
    ADC12CTL0 |= ADC12SC;       // LEITURA REAL
    while (ADC12CTL1 & ADC12BUSY);

    return ADC12MEM0;
}


unsigned int lerTemperaturaMedia(void) // Média móvel das últimas 16 leituras do LM35 (para estabilidade e filtro de ruído)
{
    // Vetor que guarda o histórico das últimas 16 leituras
    static unsigned int leituras[64] = {0};
    static unsigned long soma = 0;
    static uint8_t indice = 0;
    static uint8_t inicializado = 0;

    unsigned int nova_leitura = readADC(ADC_TEMP);
    
    // Primeiro, preenchemos todo o vetor com a primeira leitura - Para que o sistema não comece em 0°C
    if (inicializado == 0) {
        uint8_t i;
        for (i = 0; i < 64; i++) {  // Preenche todas as posições com a mesma leitura para ter valor inicial
            leituras[i] = nova_leitura;
            soma += nova_leitura;
        }
        inicializado = 1;
    } 
    // Comportamento normal: remove o valor antigo e junta o novo -> média móvel das última 16 leituras
    else {
        soma = soma - leituras[indice];  // Subtrai a leitura mais velha
        leituras[indice] = nova_leitura; // Substitui pela nova no vetor
        soma = soma + nova_leitura;      // Adiciona a nova leitura à soma
        
        indice++; // Avança para a próxima posição
        if (indice >= 64) {
            indice = 0; // Se chegou ao fim do vetor, volta ao início (circular)
        }
    }
    
    unsigned int media = soma >> 6; // A média é a soma dividida por 64
    
    return (unsigned int)(((unsigned long)media * 3300) >> 12); // Converte a média do ADC para graus Celsius (formato 253 = 25.3 °C)
}

unsigned int lerLuminosidadeEmPorcentagem(void)
{
    unsigned int luz_adc = readADC(ADC_LUZ);
    // Conversão de 12 bits (4095) para 0-100%
    return (unsigned int)(((unsigned long)luz_adc * 100) >> 12);
}
