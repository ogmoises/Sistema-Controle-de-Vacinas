#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include <stdint.h>

#define LCD_ADDR 0x27 // Endereço do módulo I2C do LCD

// USCI-B0
#define SDA BIT0        // P3.0
#define SCL BIT1        // P3.1
#define LCD_SEL P3SEL
#define LCD_DIR P3DIR
#define LCD_REN P3REN
#define LCD_OUT P3OUT

#define LCD_RS 0x01 // 0: enviar instrução - 1: enviar caractere
#define LCD_RW 0x02 // Read-Write / 0: Escrever - 1: Ler 
#define LCD_EN 0x04 // Enable / 1: LCD captura os dados - 0: LCD copia os dados
#define LCD_BL 0x08 // Backlight / 0: desligado - 1: ligado

void i2cInit(void);
uint8_t i2cSend(uint8_t addr, uint8_t data);

void lcdInit(void);
void lcdWrite(char *str);
void lcdClear(void);
void lcdSetCursor(uint8_t linha, uint8_t coluna);

#endif
