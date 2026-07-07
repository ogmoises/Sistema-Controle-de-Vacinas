#ifndef DHT11_H_
#define DHT11_H_

#include <msp430.h>
#include <stdint.h>

// DHT11 -> P6.3
#define DHT_DIR P6DIR
#define DHT_OUT P6OUT
#define DHT_REN P6REN
#define DHT_IN  P6IN
#define DHT_PIN BIT3

int dht11_read(uint8_t *umidade, uint8_t *temperatura); // Função que retorna 1 se a leitura foi bem-sucedida, 0 se falhou
void initLeituraDHT11(unsigned int *umid_inicial);

#endif
