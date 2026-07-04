#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

// Sensores Analógicos -> P6.0 e P6.1
#define LUZ  BIT0 // P6.0 -> LDR (LUZ)
#define TEMP BIT1 // P6.1 -> NTC (TEMP)
#define ADC_SEL P6SEL

#define ADC_LUZ  ADC12INCH_0 // P6.0 -> LDR (LUZ)
#define ADC_TEMP ADC12INCH_1 // P6.1 -> NTC (TEMP)

void adcInit(void);
unsigned int readADC(uint8_t channel);
unsigned int lerTemperaturaMedia(void);
unsigned int lerLuminosidadeEmPorcentagem(void);

#endif
