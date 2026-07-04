#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <stdint.h>
#include "utils.h"
#include "leds.h"

// Protótipos das funções visuais
void lcdWriteCentered(char *str, uint8_t linha);

// Função recebe os valores por parâmetro
void atualizarLCD(tela_t telaAtual, unsigned int temp, unsigned int luz, unsigned int umid);
void gerenciarDisplay(unsigned int temp, unsigned int luz, unsigned int umid);

// Funções de conversão (interface.c)
void uintToStr(unsigned int valor, char *str);
void formatTemperatura(unsigned int valor, char *out);
void formatPorcentagem(unsigned int valor, char *out);

#endif
