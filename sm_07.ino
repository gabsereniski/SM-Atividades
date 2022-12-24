//https://www.tinkercad.com/things/eR4P2RPtslQ-sm-07

#include <util/delay.h>
#include <avr/pgmspace.h>

#define DISPLAY PORTD

#define set_bit(port, bit) (port |= (1 << bit))
#define clr_bit(port, bit) (port &= ~(1 << bit))

const unsigned char table[] PROGMEM = {
    0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78,
    0x00, 0x18, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E};

long click_dec = 0;         // momento do último click do botão A
long click_inc = 0;         // momento do último click do botao B
char last_dec = (1 << PC4); //último estado do botao A
char last_inc = (1 << PC5); //último estado do botao B

long timer = 0;
int deg = 0;

int d[3] = {0, 0, 0};       // armazena digitos do grau
volatile int disp = 2;      // indica qual display deve ser ativado
volatile int prev_disp = 1; // indica qual display deve ser apagado

void read_buttons()
{
    // lê os botões
    char read_dec = PINC & (1 << PC4);
    char read_inc = PINC & (1 << PC5);

    // verifica se houve mudança de estado nos botões
    // e quando foi a última mudança (debounce)
    if (read_dec != last_dec && (timer - click_dec) > 100)
    {
        click_dec = timer;
        OCR1A -= 200;     // decrementa PWM para mudança da rotação do servo motor
        deg -= 9;         // decrementa a quantidade de graus equivalente à mudançã no pwm
        if (OCR1A < 1000) // mantêm OCR1A dentro do intervalo de valores válidos do servo
        {
            OCR1A = 1000;
            deg = 0;
        }

        last_dec = read_dec;
    }

    // análogo
    if (read_inc != last_inc && (timer - click_inc) > 100)
    {
        click_inc = timer;
        OCR1A += 200;
        deg += 9;
        if (OCR1A > 5000)
        {
            OCR1A = 5000;
            deg = 180;
        }

        last_inc = read_inc;
    }

    // separa os digitos do grau
    d[2] = (deg / 100);
    d[1] = (deg / 10) % 10;
    d[0] = (deg % 10);
}

int main()
{
    UCSR0B = 0x00;
    // coloca pinos dos displays como saída
    DDRC |= (1 << PC0) | (1 << PC1) | (1 << PC2);
    // PORTD como saída
    DDRD = 0xFF;
    // apaga segmentos
    PORTD = 0xFF;

    // muda o estado de OC0A na igualdade de comparacao
    OCR0A = 155;                         // mais prox de 10ms
    TCCR0B |= (1 << CS02) | (1 << CS00); // TC0 com prescaler de 1024, a 100Hz
    TIMSK0 |= (1 << OCIE0A);             // habilita a interrupção do TC0 na igualdade de comparcao com OCR0A

    // coloca pinos dos botões como saída
    DDRC &= ~(1 << DDC4) & ~(1 << DDC5);
    // Ativa pull-up dos botões
    PORTC |= (1 << PC4) | (1 << PC5);

    // sinal do servo como saída
    DDRB |= (1 << PB1);
    ICR1 = 39999; // 20ms, parte baixa do pwm
    OCR1A = 1000; // 5000 0.5ms a 2.5, parte alta do pwm
    TCCR1A = (1 << WGM11);
    TCCR1B |= (1 << CS11) | (1 << WGM12) | (1 << WGM13);

    // ativar a saída no pino somente depois
    // de configurar o mode de operação do timer
    TCCR1A |= (1 << COM1A1);

    // ativa interrupções
    sei();

    //fica lendo os botões
    while (1)
    {
        read_buttons();
        delay(20);
    }
}

ISR(TIMER0_COMPA_vect)
{
    timer += 10;

    // seta segmentos do digito atual
    DISPLAY = pgm_read_incyte(&table[d[disp]]);
    // desliga display anterior
    clr_bit(PORTC, prev_disp);
    // ativa atual
    set_bit(PORTC, disp);

    //atualiza indice dos displays
    switch (disp)
    {
        case 2: disp = 1, prev_disp = 2; break;
        case 1: disp = 0, prev_disp = 1; break;
        case 0: disp = 2, prev_disp = 0; break;
    }
}
