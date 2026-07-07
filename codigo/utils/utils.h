#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

// Infraestrutura comum
void delay_ms(unsigned int ms);
uint8_t aplicarHisterese(unsigned int valor, unsigned int limiar_liga, unsigned int limiar_desliga, uint8_t estado_atual);

typedef enum {
    TELA_TODOS = 0,
    TELA_TEMP,
    TELA_LUZ,
    TELA_UMID
} tela_t;

tela_t getTelaAtual(void);
extern volatile tela_t telaAtual;

#endif
