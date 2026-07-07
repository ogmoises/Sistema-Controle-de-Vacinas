// DHT11 mede umidade e temperatura e devolve os valores digitalmente em 5 bytes
// Byte 0: Umidade inteira - VAMOS USAR ESSA
// Byte 1: Umidade decimal
// Byte 2: Temperatura inteira
// Byte 3: Temperatura decimal
// Byte 4: Checksum

// MSP manda sinal de START, DHT responde (handshake)
// DHT11 manda os 40 bits, identificados pelo tempo HIGH -> dado está no tempo do pulso

// Protocolo
// LINHA EM REPOUSO: Pino HIGH
// START: MSP puxa para LOW por ~18ms
// MSP SOLTA A LINHA: puxa para HIGH por ~30us, mudando o pino para entrada
// DHT RESPONDE (handshake): LOW (80us), HIGH (80us), transmite 40bits
// TRANSMISSÃO PARA CADA BIT: 50us em LOW, seguido de ~27us em HIGH para 0 e ~70us em HIGH para 1

#include "dht11.h"
#include "utils.h"

int dht11_read(uint8_t *umidade, uint8_t *temperatura) // Usamos ponteiros para não sobrescrever o retorno da função
{
    uint8_t bits[5] = {0, 0, 0, 0, 0}; // Vetor para guardar 40 bits
    uint8_t i, j; // i percorre cada bit do byte; j percorre os 5 bytes
    uint16_t timeout; // Variável para evitar travamento interno

    // MSP envia o sinal de START
    DHT_DIR |= DHT_PIN;       // Pino como saída
    DHT_OUT &= ~DHT_PIN;      // Puxa a linha para LOW -> START
    delay_ms(20);             // Mantém em 0 por pelo menos 18ms (20.000 ciclos de 1MHz)

    DHT_OUT |= DHT_PIN;       // Puxa a linha para HIGH -> SOLTA A LINHA
    __delay_cycles(30);       // Espera ~30us
    
    DHT_DIR &= ~DHT_PIN;      // Muda o pino para entrada : MSP para de falar e passa a ouvir o sensor

    DHT_REN |= DHT_PIN;       // Habilita o resistor interno
    DHT_OUT |= DHT_PIN;       // Define o resistor como Pull-up



    // MSP espera a resposta do DHT11 (ele joga a linha pra 0 e depois pra 1)
    timeout = 10000;
    while((DHT_IN & DHT_PIN) && timeout--) if(timeout == 0) return 0; // Esperao DHT11 puxar linha pra LOW
    
    timeout = 10000;
    while(!(DHT_IN & DHT_PIN) && timeout--) if(timeout == 0) return 0; // Espera DHT11 puxar linha pra HIGH
    
    timeout = 10000;
    while((DHT_IN & DHT_PIN) && timeout--) if(timeout == 0) return 0; // Espera DHT11 puxar pra LOW novamente para iniciar os dados

    // Lê os 40 bits de dados (5 bytes)
    for(j = 0; j < 5; j++) 
    {
        for(i = 0; i < 8; i++) 
        {
            timeout = 10000;    // Espera a linha subir (início da transmissão do bit)
            while(!(DHT_IN & DHT_PIN) && timeout--) if(timeout == 0) return 0; 

            // Se a linha ficar alta por muito tempo (~70us), é o bit 1. Se for rápido (~28us), é o bit 0. Esperamos 40us para medir:
            __delay_cycles(40); 

            if(DHT_IN & DHT_PIN) // Se depois de 40us a linha ainda estiver alta:
            {
                bits[j] |= (1 << (7 - i)); // Grava o bit 1 dentro do byte atual (da esquerda pra direita)
                
                timeout = 10000; // Espera a linha descer para se preparar para o próximo bit
                while((DHT_IN & DHT_PIN) && timeout--) if(timeout == 0) return 0; 
            }
        }
    }

    // Checksum - Quinto byte deve ser a soma dos 4 anteriores, para verificar se os dados foram passados corretamente
    if(bits[0] + bits[1] + bits[2] + bits[3] == bits[4]) 
    {
        *umidade = bits[0];     // Pega a parte inteira da umidade
        *temperatura = bits[2]; // Pega a parte inteira da temperatura
        return 1; // Sucesso
    }
    
    return 0; // Falhou na checagem de erros
}

void initLeituraDHT11(unsigned int *umid_inicial) {     // Força a primeira leitura do DHT11 para evitar inicialização com zero
    uint8_t dht_umid_tmp = 0;   // umidade
    uint8_t dht_temp_tmp = 0;   // temperatura

    __disable_interrupt();      // Leitura do DHT11 precisa de timing muito preciso. Interrupções causam erros de temporização
    dht11_read(&dht_umid_tmp, &dht_temp_tmp); // Lê o sensor
    __enable_interrupt();       // Reabilita interrupções para o sistema voltar ao normal 

    *umid_inicial = dht_umid_tmp;   // Copia a umidade recebida para usarmos como valor inicial
}
