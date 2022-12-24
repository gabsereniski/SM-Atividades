//https://www.tinkercad.com/things/7PCCQ8S47ce-sm-06

#define F_CPU = 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

// troca o estado lógico do bit x da variável Y
#define cpl_bit(y, bit) (y ^= (1 << bit))

#define LED0 PB5
#define LED1 PB4
#define LED2 PB3

// controla subida e descida dos valores de OCR2A
#define UP 0
#define DOWN 1
volatile bool direc = UP; 

ISR(TIMER0_COMPA_vect)
{
    // altera estado do led
    cpl_bit(PORTB, LED0);
    
    // incrementa e decrementa OCR2A gradativamente
    if (direc == UP)
    {
        OCR2A++;
        if (OCR2A == 253) direc = 1;
    }
    else
    {
        OCR2A--;
        if (OCR2A == 0) direc = 0;
    }
}

ISR(TIMER1_COMPA_vect)
{
    cpl_bit(PORTB, LED1);
}

int main()
{
    // somente pino do LED como saída
    DDRB = 0b00111000;  
    // apaga LED e habilita pull-ups nos pinos não utilizados
    PORTB = 0b11000111; 

    // Timer 0
    // muda o estado de OC0A na igualdade de comparacao
    TCCR0A = (1 << COM0A0);    
    // TC0 com prescaler de 1024
    TCCR0B = (1 << CS02) | (1 << CS00); 
    // mais prox de 10ms
    OCR0A = 155; 
    // habilita a interrupção do TC0 na igualdade de comparcao com OCR0A
    TIMSK0 = (1 << OCIE0A);             

    // Timer 1
    // muda o estado de OC1A na igualdade de comparacao
    TCCR1A = (1 << COM1A0);
    // TC1 com prescaler de 1024
    TCCR1B = (1 << CS11) | (1 << CS10); 
    // mais prox de 998ms
    OCR1A = 15593; 
    // habilita a interrupção do TC1 na igualdade de comparacao com OCR1A
    TIMSK1 = (1 << OCIE1A);             

    // Timer 2
    // modo PWM rapido
    TCCR2A |= (1 << WGM21) | (1 << WGM20);
    // TC2 com prescaler de 1024
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); 
    // OCR2A varia na interrupcao do timer 1
    OCR2A = 0;
    // ativa saida PWM
    TCCR2A |= (1 << COM2A1);

    sei();

    while (1) delay(10);
}