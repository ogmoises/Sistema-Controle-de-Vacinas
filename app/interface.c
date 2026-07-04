#include "interface.h"
#include "lcd.h"
#include "utils.h"
#include "button.h"
#include "pwm.h"

#include <string.h>


void lcdWriteCentered(char *str, uint8_t linha) // Calcula quantos espaços em branco deixamos antes de escrever a string
{
    uint8_t len = 0;
    uint8_t col;
    char *p = str;

    while(*p++) len++; //compacta o tamanho da string até encontrar o terminador nulo (\0)

    if(len >= 16) col = 0;
    else col = (16 - len) / 2;

    lcdSetCursor(linha, col);
    lcdWrite(str);
}

void atualizarLCD(tela_t telaAtual, unsigned int temp, unsigned int luz, unsigned int umid)
{
    char bufferNum[6];
    char linha2[17];
    char t4[5], l4[5], u4[5];

    lcdClear();

    switch(telaAtual)
    {
        case TELA_TODOS:
            lcdSetCursor(0,0);
            lcdWrite("Temp  Lumi  Umid");

            formatTemperatura(temp, t4);
            formatPorcentagem(luz,  l4);
            formatPorcentagem(umid, u4);

            lcdSetCursor(1,0); lcdWrite(t4);    // Posição da escrita da temperatura
            lcdSetCursor(1,6); lcdWrite(l4);    // Posição da escrita da luminosidade 
            lcdSetCursor(1,12); lcdWrite(u4);   // Posição da escrita da umidade
            break;

        case TELA_TEMP:
        {
            char linha1[17];
            char bufFan[6];
            
            // Monta a Linha 1: "Temp(PWM:XX%)"
            strcpy(linha1, "Temp( PWM:");
            uintToStr(getFanSpeed(), bufFan);
            strcat(linha1, bufFan);
            strcat(linha1, "%)");
            lcdWriteCentered(linha1, 0);
            
            // Monta a Linha 2 com a Temperatura + Status
            formatTemperatura(temp, bufferNum);
            strcpy(linha2, bufferNum);
            strcat(linha2, "\xDF" "C"); 
            
            if (temp >= TEMP_CRITICA)      strcat(linha2, " - CRITICA");
            else if (temp > TEMP_MAX)      strcat(linha2, " - ALTA");
            else if (temp < TEMP_MIN)      strcat(linha2, " - BAIXA");
            else                           strcat(linha2, " - OK");
            
            lcdWriteCentered(linha2, 1); 
            break;
        }

        case TELA_LUZ:
            lcdWriteCentered("Luminosidade", 0);
            
            uintToStr(luz, bufferNum);
            strcpy(linha2, bufferNum);
            strcat(linha2, "%");
            
            if (luz > LUZ_MAX) strcat(linha2, " - ALTA");
            else               strcat(linha2, " - OK");
            
            lcdWriteCentered(linha2, 1);
            break;

        case TELA_UMID:
            lcdWriteCentered("Umidade", 0);
            
            uintToStr(umid, bufferNum);
            strcpy(linha2, bufferNum);
            strcat(linha2, "%");
            
            if (umid > UMID_MAX)       strcat(linha2, " - ALTA");
            else if (umid < UMID_MIN)  strcat(linha2, " - BAIXA");
            else                       strcat(linha2, " - OK");
            
            lcdWriteCentered(linha2, 1);
            break;
    }
}


void gerenciarDisplay(unsigned int temp, unsigned int luz, unsigned int umid)
{
    // A variável estática guarda o valor entre as chamadas da função
    static unsigned int contadorDisplay = 0; 

    // 1. Verifica se deve atualizar a tela agora (por tempo ou por clique de botão)
    if(atualizarTela) {
        atualizarLCD(telaAtual, temp, luz, umid); 
        atualizarTela = 0; 
    }

    // 2. Conta o tempo para a próxima atualização automática
    contadorDisplay++;
    if(contadorDisplay >= 20) { // 20 ciclos de 50ms = 1 segundo
        contadorDisplay = 0;
        atualizarTela = 1; // Dispara a flag para atualizar no próximo ciclo
    }
}







void uintToStr(unsigned int valor, char *str) // LCDs entendem apenas caracteres ASCII - Função de conversão
{
    unsigned int tempVal;
    int i = 0, j;

    if(valor == 0) {
        str[0] = '0'; str[1] = '\0'; return;
    }
    tempVal = valor;
    while(tempVal > 0) {
        str[i++] = (tempVal % 10) + '0';
        tempVal /= 10;
    }
    str[i] = '\0';
    for(j = 0; j < i/2; j++) {
        char aux = str[j];
        str[j] = str[i-1-j];
        str[i-1-j] = aux;
    }
}


void formatTemperatura(unsigned int valor, char *out)
{
    out[0] = (valor / 100) % 10 + '0'; // Dezena
    out[1] = (valor / 10)  % 10 + '0'; // Unidade
    out[2] = '.';                      // Ponto decimal
    out[3] = valor % 10 + '0';         // Casa decimal
    out[4] = '\0';                     // Fim da string
}


void formatPorcentagem(unsigned int valor, char *out)
{
    out[0] = (valor / 100) % 10 + '0';  // Centena
    out[1] = (valor / 10)  % 10 + '0';  // Dezena
    out[2] = valor % 10 + '0';          // Unidade
    out[3] = '%';                       // Símbolo de porcentagem
    out[4] = '\0';                      // Fim da string
}
