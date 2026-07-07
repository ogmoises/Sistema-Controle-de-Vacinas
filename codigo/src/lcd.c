// Módulo PCF854 se comunica em I2C (não o LCD)
// MSP envia bytes por I2C, o módulo converte e o LCD recebe como se fosse um LCD paralelo comum, de 4 bits

#include <msp430.h>
#include <stdint.h>
#include "lcd.h"
#include "utils.h"


void lcdWriteNibble(uint8_t nibble, uint8_t isChar) // EX3
{
    uint8_t data; // Função recebe um nibble (nibble = 4 bits)

    data = (nibble << 4); // Movemos nibble para os bits superiores (pinos D7, D6, D5, D4)

    if(isChar)
    {
        data |= LCD_RS; // Ativa o modo de enviar caractere se isChar == 1
    }

    data |= LCD_BL;      // Liga backlight

    i2cSend(LCD_ADDR, data); // EN == 0 - LCD só aceita valor quando fazemos pulso no pino EN
    i2cSend(LCD_ADDR, data | LCD_EN); // EN == 1
    i2cSend(LCD_ADDR, data); // EN == 0
}


void lcdWriteByte(uint8_t byte, uint8_t isChar) // EX4
{
    lcdWriteNibble(byte >> 4, isChar);   // nibble mais significativo (desloca 4 bits pra direita, eliminado o nibble menos significativo e colocando
                                         // o nibble mais significativo nele)

    lcdWriteNibble(byte & 0x0F, isChar); // nibble menos significativo (faz and bit a bit com 0000 1111 - elimina nibble mais significativo, restando
                                         // apenas o nibble menos significativo)
}


void i2cInit(void)
{
    UCB0CTL1 |= UCSWRST; // Coloca a USCI-B0 em modo reset (Universal Serial Communication Interface)
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC; // Mestre / I2C / Síncrono
    UCB0CTL1 |= UCSSEL_2; // SMCLK

    UCB0BR0 = 10; // Divisor de clock -> Clock I2C = 100kHz
    UCB0BR1 = 0; // Parte alta do divisor de clock - Deve ser mantida em 0

    LCD_SEL |= SDA | SCL; // Libera os pinos de SDA e SCL para controle por meio dos periféricos (e não do GPIO)
    LCD_DIR &= ~(SDA | SCL); // Entrada
    LCD_REN |= SDA | SCL; // Ativa resistores internos
    LCD_OUT |= SDA | SCL; // Pullup (nível baixo ativa as funções)

    UCB0CTL1 &= ~UCSWRST; // Tira reset - Interface começa a funcionar
}


uint8_t i2cSend(uint8_t addr, uint8_t data)
{
    UCB0IFG &= ~UCNACKIFG; // Limpa a flag de interrupção - Garante funcionamento apropriado
    UCB0I2CSA = addr; // Slave Address
    UCB0CTL1 |= UCTR; // Configura interface como transmissora (receptora = 0) - enviaremos dados ao LCD
    UCB0CTL1 |= UCTXSTT; // Sinalização de start

    while(!(UCB0IFG & UCTXIFG)); // Libera quando o buffer estiver vazio

    UCB0TXBUF = data; // Escreve o byte a ser enviado

    while(UCB0CTL1 & UCTXSTT); // Libera quando start terminar (momento que o escravo responde)

    if(UCB0IFG & UCNACKIFG) // Verifica NACK (0:ACK - Escravo respondeu; 1:NACK - Escravo não respondeu)
    {                       // Se houver NACK:

        UCB0CTL1 |= UCTXSTP; // Sinalização de stop

        while(UCB0CTL1 & UCTXSTP); // Libera quando stop terminar

        UCB0IFG &= ~UCNACKIFG; // Desativa a flag de interrupção

        return 1; // Retorna erro
    }

    while(!(UCB0IFG & UCTXIFG)); // Se não houve erro (se não houver NACK): Libera quando transmissão terminar
    UCB0CTL1 |= UCTXSTP; // Sinalização de stop
    while(UCB0CTL1 & UCTXSTP); // Libera quando stop terminar

    return 0; // Retorna sucesso
}


void lcdInit(void) // EX5
{
    lcdWriteNibble(0x3,0); // [0011 0000] Instrução - Configura a interface em: modo 8 bits; usando 1 linha; fonte 5x8
    delay_ms(5); // Espera LCD processar o comando

    lcdWriteNibble(0x3,0); // Repetir comando (LCD poderia estar em estado indefinido anteriormente)
    delay_ms(5);

    lcdWriteNibble(0x3,0); // Repetir comando pela terceira vez - Garante que estejamos em estado conhecido
    delay_ms(5);

    lcdWriteNibble(0x2,0); // [0010 0000] Trocamos para modo 4 bits - LCD receberá nibble alto + nibble baixo
    delay_ms(5);

    lcdWriteByte(0x28,0); // [0010 1000] Configura a interface em: modo 4 bits; usando 2 linhas; fonte 5x8
    lcdWriteByte(0x0C,0); // [0000 1100] Configura o display: Display ligado, Cursor desligado, Cursos estático
    lcdWriteByte(0x01,0); // Limpa display
    delay_ms(2); // Leva 1.53ms para executar
    lcdWriteByte(0x06,0); // [0000 0110] Comportamento do cursor: Após escrever um caractere, avance uma posição para a direita
}


void lcdWrite(char *str) // EX6 - char *str é um ponteiro para uma string
{
    uint8_t pos = 0; // conta quantos caracteres já foram escritos

    while(*str) // Enquanto o caractere apontado por str não for 0x00 (o último da string)
    {
        lcdWriteByte(*str,1); // Escreva o caractere (isChar = 1) apontado por str no momento

        pos++;

        if(pos == 16) // Se esgotamos a primeira linha
        {
            lcdWriteByte(0xC0,0); // [1100 0000] Instrução: Próxima escrita de dados será na DDRAM 0x40 [segunda linha]
        }

        str++; // Move o ponteiro para o próximo caractere
    }
}


void lcdClear(void)
{
    lcdWriteByte(0x01,0); // Limpa display
    delay_ms(2);
}


void lcdSetCursor(uint8_t linha, uint8_t coluna)
{
    uint8_t endereco;

    if(linha == 0) // Primeira linha
        endereco = 0x80 + coluna; // Posicionamento horizontal do caractere escrito
    else // Segunda linha
        endereco = 0xC0 + coluna; 

    lcdWriteByte(endereco,0);
}
