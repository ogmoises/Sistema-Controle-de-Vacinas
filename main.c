// A cada volta no while, iremos:
// Ler a temperatura, luminosidade e umidade (a cada 20 ciclos)
// A partir dos valores obtidos, decidimos o comportamento dos LEDs, ventoinha e buzzer
// Atualizamos a tela do LCD caso necessário a partir do botão
// Espera 50ms

#include <msp430.h>
#include <stdint.h>
#include "adc.h"
#include "lcd.h"
#include "leds.h"
#include "dht11.h"
#include "buzzer.h"
#include "interface.h"
#include "pwm.h"
#include "button.h" 
#include "utils.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;  // Trava o watchdog 

    // 1. INICIAMOS TODOS OS MÓDULOS
    adcInit();
    i2cInit();
    lcdInit();
    buttonInit();
    ledsInit();
    buzzerInit();
    pwmInit();

    __enable_interrupt(); // Habilitamos interrupções

    unsigned int luz = 0, temp = 0, umid = 0; // Valor dos sensores

    uint8_t dht_umid = 0; // Valor lido pelo DHT11 referente a umidade
    uint8_t dht_temp = 0; // Valor lido pelo DHT11 referente a temperatura
    static uint16_t contadorUmSegundo = 0; // Responsável pelo tempo que irá demorar para lermos novamente o DHT11 e o LDR

    static uint8_t ciclos_bloqueio = 100; // Bloqueio dos atuadores em relação a temperatura. Cada ciclo é 50ms - 100 = 5s
    
    initLeituraDHT11(&umid); // Inicialização segura do DHT11 - Para não iniciar em 0

    while(1)
    {
        // 1. LEITURA E FILTRO DA TEMPERATURA (LM35)
        temp = lerTemperaturaMedia();     

        // 3. LEITURA DO DHT11 E LDR (A cada 1 segundo)
        contadorUmSegundo ++; 
        if (contadorUmSegundo >= 20) {
            contadorUmSegundo = 0;
             // 2. LEITURA DA LUZ (LDR)
             luz = lerLuminosidadeEmPorcentagem();
            __disable_interrupt(); 
            dht11_read(&dht_umid, &dht_temp); 
            __enable_interrupt();
            umid = dht_umid;
        }

        // 4. LÓGICA DE ATUADORES

        // Atuadores com proteção inicial (Temperatura)
        if (ciclos_bloqueio > 0) {
            // SISTEMA TÉRMICO INICIANDO
            ciclos_bloqueio--; 
            setFanSpeed(0);                         // Garante a ventoinha parada
            verificarAlarmes(temp, luz, umid, 0);   // LED da Temperatura desabilitado
            gerenciarAlarmes(luz, temp, 0);
        } 
        else {
            // SISTEMA TÉRMICO ESTABILIZADO
            controlarVentoinha(temp); 
            verificarAlarmes(temp, luz, umid, 1);   // LED da Temperatura habilitado
            gerenciarAlarmes(luz, temp, 1);
        }

        // 5. Atualizar botão e display
        buttonUpdate();
        gerenciarDisplay(temp, luz, umid);

        delay_ms(50); // Esperamos 50ms para o próximo ciclo
    }
}
