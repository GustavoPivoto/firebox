# Firebox – Detector de Gás e Fogo para Ambientes Industriais Indoor

## Descrição do Projeto
O **Firebox** é um sistema desenvolvido para **detecção de gás e fogo em ambientes industriais internos**, utilizando o microcontrolador **ATmega328P** (Arduino Uno). O objetivo é proporcionar **monitoramento contínuo e alertas rápidos** em situações de risco, como vazamento de gás ou presença de chamas.

Projeto desenvolvido por:
- Gustavo Pivoto Ambrósio – GEC – 2028
- Gustavo Pereira Moraes – GET – 9762
- Rodrigo Armengol de Oliveira – GES – 294

---

## Funcionalidades
- **Detecção de gás** utilizando o sensor MQ-2.
- **Detecção de chamas** utilizando um sensor de fogo.
- **Alertas visuais e sonoros:**
  - LED verde: Inicialização.
  - LED amarelo: Detecção de gás.
  - LED vermelho: Detecção de chama ou simultaneamente gás e fogo.
  - Buzzer: Sinalização sonora.

---

## Como Funciona
1. Ao ligar o dispositivo, o LED verde pisca por 10 segundos (fase de inicialização).
2. O sistema entra em modo standby e começa o monitoramento contínuo.
3. Quando gás é detectado:
   - O LED amarelo pisca.
4. Quando chama é detectada:
   - O LED vermelho acende.
   - O buzzer emite som intermitentemente.

---

## Estrutura Física
### Visão Geral do Dispositivo:
![Visão Geral do Dispositivo](imagens/visao-geral-do-dispositivo.png)

### Circuito Interno:
![Circuito Interno](imagens/circuito-interno.png)

---

## Componentes Utilizados
- Microcontrolador **ATmega328P** (Arduino Uno)
- Sensor de gás **MQ-2**
- Sensor de chama
- Buzzer
- LEDs: Verde, Amarelo e Vermelho
- Protoboard
- Jumpers
