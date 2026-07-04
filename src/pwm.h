#ifndef PWM_H_
#define PWM_H_

#include <msp430.h>

// PWM da Ventoinha -> P1.4
#define PWM_DIR P1DIR
#define PWM_SEL P1SEL
#define PWM_PIN BIT4

void pwmInit(void);
void setFanSpeed(unsigned int porcentagem); // Recebe de 0 a 100%
unsigned int getFanSpeed(void);
void controlarVentoinha(unsigned int temperatura_atual);

#endif
