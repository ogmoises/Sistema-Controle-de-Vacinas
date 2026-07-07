#ifndef BUZZER_H_
#define BUZZER_H_

#include <msp430.h>
#include <stdint.h>

// Buzzer -> P2.4
#define BUZZER_DIR P2DIR
#define BUZZER_OUT P2OUT
#define BUZZER_PIN BIT4 

#define SEGUNDO  4096 - 1           // Cálculos para o Timer com ACLK dividido por 8 (4096 ticks por segundo)
#define TEMPO_BUZZER_LUZ 20480 - 1  // 5 segundos
#define TEMPO_BUZZER_TEMP 20480 - 1 // 5 segundos
#define TEMPO_BIPE 819 - 1          // 0.2 segundos

void buzzerInit(void);
void gerenciarAlarmes(unsigned int luz, unsigned int temp, uint8_t sistema_termico_habilitado);

#endif
