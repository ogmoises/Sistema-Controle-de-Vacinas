#include "button.h"
#include "utils.h"

volatile uint8_t bloqueioBotao = 0; // Botão começa desbloqueado
volatile uint8_t atualizarTela = 1; // Tela atualizada quando clicamos no botão


void buttonInit(void)
{
    BUTTON_DIR &= ~BUTTON_PIN;   // Entrada
    BUTTON_REN |= BUTTON_PIN;    // Resistor interno ON
    BUTTON_OUT |= BUTTON_PIN;    // pull-up (Pressionado = 0)

    BUTTON_IES |= BUTTON_PIN;    // interrupção na borda de descida
    BUTTON_IFG &= ~BUTTON_PIN;   // limpa flag
    BUTTON_IE  |= BUTTON_PIN;    // habilita interrupção
}


void buttonUpdate(void) // Libera o botão depois que for solto
{
    if(bloqueioBotao && (BUTTON_IN & BUTTON_PIN)) {     // Se botão estiver bloqueado e solto
        delay_ms(20);                                   // Delay para não travar a operação
        if(BUTTON_IN & BUTTON_PIN) bloqueioBotao = 0;   // Libera o botão
    }
}


#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
    if(BUTTON_IFG & BUTTON_PIN) {   // Se a interrupção foi causada pelo pino do botão
        if(!bloqueioBotao && !(BUTTON_IN & BUTTON_PIN)) {   // Se o botão foi apertado e não foi bloqueado ainda:

            if(telaAtual == TELA_UMID) telaAtual = TELA_TODOS; // Se chegou na última telavolta para a TELA_TODOS (Circular)
            else telaAtual++;

            atualizarTela = 1;
            bloqueioBotao = 1;  
        }
        BUTTON_IFG &= ~BUTTON_PIN; // Zera a flag de interrupção   
    }
}
