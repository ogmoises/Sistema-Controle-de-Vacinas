#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <msp430.h>
#include <stdint.h>
#include "utils.h"

// Botão -> P2.6
#define BUTTON_PIN BIT6
#define BUTTON_DIR P2DIR
#define BUTTON_REN P2REN
#define BUTTON_OUT P2OUT
#define BUTTON_IES P2IES
#define BUTTON_IFG P2IFG
#define BUTTON_IE P2IE
#define BUTTON_IN P2IN

extern volatile uint8_t bloqueioBotao;
extern volatile uint8_t atualizarTela;

void buttonInit(void);
void buttonUpdate(void);


#endif
