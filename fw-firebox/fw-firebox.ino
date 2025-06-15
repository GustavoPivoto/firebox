#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

//---------------------------------- Defines ---------------------------------//
#define PINO_LED_VERDE      (1 << PD2)
#define PINO_LED_AMARELO    (1 << PD4)
#define PINO_LED_VERMELHO   (1 << PD3)
#define PINO_BUZZER         (1 << PB3)
#define PINO_SENSOR_FOGO    (1 << PD5)
#define PINO_SENSOR_GAS     (0)

//------------------------------- Variáveis Globais --------------------------//
unsigned int contador_tempo_ms = 0;

unsigned int contador_pisca_gas = 0;
unsigned int contador_pisca_fogo = 0;

unsigned short int resultado_adc = 0;

bool sistema_ativo = false;
bool alerta_gas = false;
bool alerta_fogo = false;

/* Controle de piscadas do LED verde */
bool led_verde_piscando = true;
unsigned int contador_pisca_led_verde = 0;
unsigned int quantidade_piscadas_led_verde = 0;

/* Controle do Buzzer */
unsigned int contador_pwm_buzzer = 0;
unsigned char ciclo_trabalho_pwm_buzzer = 0;
bool buzzer_aumentando = true;

//---------------------------------- Funções ---------------------------------//
void gpio_inicializar() {
    DDRD |= (PINO_LED_VERDE | PINO_LED_AMARELO | PINO_LED_VERMELHO);
    DDRB |= PINO_BUZZER;

    PORTD &= ~(PINO_LED_VERDE | PINO_LED_AMARELO | PINO_LED_VERMELHO);
    PORTB &= ~PINO_BUZZER;
}

void timer0_inicializar() {
    TCCR0A = (1 << WGM01);               // Modo CTC
    TCCR0B = (1 << CS01) | (1 << CS00);  // Prescaler 64
    OCR0A = 249;                         // Interrupção a cada 1ms
    TIMSK0 = (1 << OCIE0A);              // Habilita interrupção do comparador A
}

void timer2_inicializar() {
    TCCR2A |= (1 << WGM20) | (1 << WGM21) | (1 << COM2A1);  // Fast PWM
    TCCR2B |= (1 << CS21);  // Prescaler 8
    OCR2A = 0;              // Duty cycle inicial 0%
}

void adc_inicializar() {
    ADMUX = (0 << REFS1) | (1 << REFS0);  // Referência AVcc
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Prescaler 128
    ADMUX = (ADMUX & 0b11111000) | PINO_SENSOR_GAS;
}

void adc_iniciar_conversao() {
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
}

//---------------------------------- Função Principal ------------------------//
int main() {
    sistema_ativo = true;

    Serial.begin(9600);

    gpio_inicializar();
    adc_inicializar();
    timer0_inicializar();
    timer2_inicializar();

    sei();

    while (1) {
        adc_iniciar_conversao();
        resultado_adc = ADC;

        if (resultado_adc > 200) {
            alerta_gas = true;
        } else {
            alerta_gas = false;
            PORTD &= ~PINO_LED_AMARELO;
        }

        if (!(PIND & PINO_SENSOR_FOGO)) {
            alerta_fogo = true;
        } else {
            alerta_fogo = false;
            PORTD &= ~PINO_LED_VERMELHO;
            OCR2A = 0;  // Desliga buzzer
        }

        Serial.print("ADC: ");
        Serial.println(resultado_adc);
    }
}

//------------------------------- Rotinas de Interrupção ---------------------//
ISR(TIMER0_COMPA_vect) {
    if (sistema_ativo) {
        contador_tempo_ms++;

        /* Piscar LED Verde */
        if (led_verde_piscando) {
            contador_pisca_led_verde++;
            if (contador_pisca_led_verde >= 500) {
                contador_pisca_led_verde = 0;
                PORTD ^= PINO_LED_VERDE;

                quantidade_piscadas_led_verde++;
                if (quantidade_piscadas_led_verde >= 20) {
                    led_verde_piscando = false;
                    PORTD &= ~PINO_LED_VERDE;
                }
            }
        }

        /* Controle do Buzzer com PWM */
        if (alerta_fogo) {
            contador_pwm_buzzer++;
            if (contador_pwm_buzzer >= 1) {
                contador_pwm_buzzer = 0;

                if (buzzer_aumentando) {
                    if (ciclo_trabalho_pwm_buzzer < 255) {
                        ciclo_trabalho_pwm_buzzer++;
                    } else {
                        buzzer_aumentando = false;
                    }
                } else {
                    if (ciclo_trabalho_pwm_buzzer > 0) {
                        ciclo_trabalho_pwm_buzzer--;
                    } else {
                        buzzer_aumentando = true;
                    }
                }

                OCR2A = ciclo_trabalho_pwm_buzzer;
            }
        } else {
            ciclo_trabalho_pwm_buzzer = 0;
            buzzer_aumentando = true;
            OCR2A = 0;
        }

        /* Piscar LED Amarelo */
        if (alerta_gas) {
            contador_pisca_gas++;
            if (contador_pisca_gas >= 1000) {
                contador_pisca_gas = 0;
                PORTD ^= PINO_LED_AMARELO;
            }
        } else {
            contador_pisca_gas = 0;
        }

        /* Piscar LED Vermelho */
        if (alerta_fogo) {
            contador_pisca_fogo++;
            if (contador_pisca_fogo >= 1000) {
                contador_pisca_fogo = 0;
                PORTD ^= PINO_LED_VERMELHO;
            }
        } else {
            contador_pisca_fogo = 0;
        }

        /* Reset contador geral */
        if (contador_tempo_ms >= 1000) {
            contador_tempo_ms = 0;
        }
    }
}