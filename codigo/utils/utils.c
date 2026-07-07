#include "utils.h"

volatile tela_t telaAtual = TELA_TODOS;

void delay_ms(unsigned int ms) {
    while(ms--) __delay_cycles(1000); 
}

// Retorna 1 se deve ligar, 0 se deve desligar, ou mantém o estado atual se estiver na zona morta
uint8_t aplicarHisterese(unsigned int valor, unsigned int limiar_liga, unsigned int limiar_desliga, uint8_t estado_atual) 
{
    if (valor >= limiar_liga) return 1; // Liga se passou do limite superior
    if (valor <= limiar_desliga) return 0; // Desliga se caiu abaixo do limite inferior
    return estado_atual; // Mantém o estado atual se estiver entre os dois (zona morta)
}
