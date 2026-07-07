#ifndef LEDS_H_
#define LEDS_H_

#include <msp430.h>
#include <stdint.h>

// Definição dos pinos dos LEDs locais
#define LED_TEMP BIT0 // P2.0 
#define LED_UMID BIT3 // P2.3 
#define LED_LUZ  BIT2 // P2.2 
#define LEDS_DIR P2DIR
#define LEDS_OUT P2OUT

// CALIBRAÇÃO DOS ATUADORES A PARTIR DESSES VALORES
#define TEMP_MIN             135 // Temperatura Baixa
#define TEMP_MIN_DESLIGA     140 // Um pouco acima de TEMP_MIN
#define TEMP_MAX             180 // Temperatura Alta
#define TEMP_MAX_DESLIGA     175 // Um pouco abaixo de TEMP_MAX
#define TEMP_CRITICA         230 // Temperatura em que ligamos o buzzer e ativamos 100% da ventoinha
#define TEMP_CRITICA_DESLIGA 220 // Um pouco abaixo de TEMP_CRITICA

#define LUZ_MAX          35  // Luminosidade Alta
#define LUZ_MAX_DESLIGA  30 // Um pouco abaixo de LUZ_MAX

#define UMID_MIN         50 // Umidade Baixa
#define UMID_MIN_DESLIGA 55 // Um pouco acima de UMID_MIN_DESLIGA
#define UMID_MAX         75 // Umidade Alta
#define UMID_MAX_DESLIGA 70 // Um pouco abaixo de UMID_MAX

// Protótipos das funções
void ledsInit(void);
void verificarAlarmes(unsigned int temp, unsigned int luz, unsigned int umid, uint8_t temp_habilitada);

#endif
