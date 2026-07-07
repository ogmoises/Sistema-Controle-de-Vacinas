# Monitoramento e Controle de Condições Ambientais em Armazenamento de Vacinas

Este repositório contém o projeto final desenvolvido para a disciplina **ENE0440 - Laboratório de Sistemas Microprocessados** da Universidade de Brasília (UnB), sob a orientação do Prof. Dr. William H. C. Sánchez.

O objetivo do sistema é garantir a integridade de insumos imunobiológicos (vacinas) dentro de uma câmara frigorífica simulada, controlando e monitorando ativamente variáveis de temperatura, umidade relativa e luminosidade interna utilizando o microcontrolador **MSP430F5529LP**.

---

## 🛠️ Arquitetura de Hardware e Componentes

A integração de hardware foi projetada para isolar a eletrônica de potência dos barramentos sensíveis de leitura analógica:
- **Núcleo de Processamento:** MSP430F5529LP (Texas Instruments)
- **Sensor de Temperatura:** LM35 (Transdutor analógico linear com ganho de $10\,\text{mV/°C}$)
- **Sensor de Umidade Relativa:** DHT11 (Protocolo digital *Single-Wire*)
- **Sensor de Luminosidade:** Fotoresistor LDR em divisor de tensão de $10\,\text{k}\Omega$
- **Atuador Térmico:** Micro Ventoinha de 12V acionada via Driver Ponte H L298N com alimentação externa
- **Interface Local (IHM):** Display LCD 2x16 com módulo de comunicação I²C e botão pulsador de navegação de telas
- **Alarmes:** Buzzer ativo de 5V e LEDs independentes de sinalização de falhas por pino de GPIO

---

## 💻 Recursos Avançados de Firmware (Linguagem C)

O software foi modelado de forma não bloqueante sob a estrutura de um *superloop*, garantindo determinismo temporal e previsibilidade elétrica:

1. **Filtro Digital de Média Móvel:** As leituras analógicas do sensor LM35 passam por um filtro circular de 64 amostras para eliminar ruídos de alta frequência induzidos pelo chaveamento magnético do motor elétrico, otimizado via deslocamento de bits à direita (`soma >> 6`).
2. **Máquina de Estados Térmica com Histerese:**
   - **Zona Fria ($T < 14,0\,\text{°C}$):** Ventoinha desligada ($0\,\%$ PWM) para evitar o congelamento das vacinas. O retorno para a zona ideal possui recuo por histerese em $14,0\,\text{°C}$.
   - **Zona Normal ($14,0\,\text{°C} \le T \le 18,0\,\text{°C}$):** Ventoinha fixada em $20\,\%$ de exaustão contínua para homogeneização do ar interno.
   - **Zona de Aquecimento ($18,0\,\text{°C} < T < 23,0\,\text{°C}$):** Aceleração linear proporcional do ciclo de trabalho (duty cycle) do PWM entre $20\,\%$ e $100\,\%$. O recuo de retorno obedece à histerese em $17,5\,\text{°C}$.
   - **Zona Crítica ($T \ge 23,0\,\text{°C}$):** Ventoinha em saturação máxima de potência ($100\,\%$ PWM). O alarme sonoro desarma apenas ao recuar abaixo de $22,0\,\text{°C}$.
3. **Janela de Confirmação contra Transientes (Debounce Temporal):** Os alarmes acústicos do Buzzer (gerenciados pelo `Timer A1` com clock secundário `ACLK`) incorporam um atraso de validação de $5\,\text{segundos}$ contínuos, prevenindo falsos positivos causados por oscilações térmicas ou feixes rápidos de luz na abertura de portas.
4. **Histerese Higrométrica Dupla:** O monitoramento do DHT11 possui duas zonas mortas de amortecimento independentes: acionamento por baixa umidade em $50\,\%$ com retorno em $55\,\%$, e violação por alta umidade em $75\,\%$ com retorno em $70\,\%$.

---

## 📊 Resultados Experimentais

Os dados foram coletados em tempo real diretamente dos registradores da memória RAM do microcontrolador durante os ensaios práticos de bancada. 

*Nota: Os gráficos temporais de validação de Temperatura, PWM, Umidade e Luminosidade estão disponíveis e detalhados no corpo do artigo científico associado (`relatorio.pdf`).*

---

## 🚀 Como Executar o Projeto

1. Instale o ambiente de desenvolvimento **Code Composer Studio (CCS Theia)**.
2. Importe os arquivos da pasta `/codigo`.
3. Conecte a placa MSP430F5529LP via USB e realize o mapeamento físico de pinos conforme descrito na Tabela de Pinagem do relatório.
4. Compile e execute em modo *Debug*.

---

### 👤 Autor
- **Moises de Araújo Altounian**
- Engenharia de Computação, Universidade de Brasília (UnB)
