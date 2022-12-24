//https://www.tinkercad.com/things/jJ7pLP3yRQ4-sm-05-lcd

#ifndef _DEF_PRINCIPAIS_H
#define _DEF_PRINCIPAIS_H

#define F_CPU 16000000UL // define a frequencia do microcontrolador - 16MHz

#include <avr/io.h>       //definições do componente especificado
#include <util/delay.h>   //biblioteca para o uso das rotinas de _delay_ms e _delay_us()
#include <avr/pgmspace.h> //para o uso do PROGMEM, gravação de dados na memória flash

// Definições de macros para o trabalho com bits

#define set_bit(y, bit) (y |= (1 << bit))  // coloca em 1 o bit x da variável Y
#define clr_bit(y, bit) (y &= ~(1 << bit)) // coloca em 0 o bit x da variável Y
#define cpl_bit(y, bit) (y ^= (1 << bit))  // troca o estado lógico do bit x da variável Y
#define tst_bit(y, bit) (y & (1 << bit))   // retorna 0 ou 1 conforme leitura do bit

#endif

#ifndef _LCD_H
#define _LCD_H

// Definições para facilitar a troca dos pinos do hardware e facilitar a re-programação

#define DADOS_LCD PORTC // 4 bits de dados do LCD no PORTD
#define nibble_dados 0  // 0 para via de dados do LCD nos 4 LSBs do PORT empregado 
                        //                        (Px0-D4, Px1-D5, Px2-D6, Px3-D7)
                        // 1 para via de dados do LCD nos 4 MSBs do PORT empregado 
                        //                        (Px4-D4, Px5-D5, Px6-D6, Px7-D7)
#define CONTR_LCD PORTC // PORT com os pinos de controle do LCD (pino R/W em 0).
#define E  PC4           // pino de habilitação do LCD (enable)
#define RS PC5          // pino para informar se o dado é uma instrução ou caractere

// sinal de habilitação para o LCD
#define pulso_enable()         \
        _delay_us(1);          \
        set_bit(CONTR_LCD, E); \
        _delay_us(1);          \
        clr_bit(CONTR_LCD, E); \
        _delay_us(45)

// protótipo das funções
void cmd_LCD(unsigned char c, char cd);
void inic_LCD_4bits();

#endif

//================================================================================ //
//    Sub-rotinas para o trabalho com um LCD 16x2 com via de dados de 4 bits       //
//    Controlador HD44780 - Pino R/W aterrado                                      //
//    A via de dados do LCD deve ser ligado aos 4 bits mais significativos ou      //
//    aos 4 bits menos significativos do PORT do uC                                //
//================================================================================ //

//---------------------------------------------------------------------------------//
// Sub-rotina para enviar caracteres e comandos ao LCD com via de dados de 4 bits
//---------------------------------------------------------------------------------//

// c é o dado  e cd indica se é instrução ou caractere
void cmd_LCD(unsigned char c, char cd)
{
    if (cd == 0)
        clr_bit(CONTR_LCD, RS);
    else
        set_bit(CONTR_LCD, RS);

// primeiro nibble de dados - 4 MSB
#if (nibble_dados)
    // compila código para os pinos de dados do LCD nos 4 MSB do PORT
    DADOS_LCD = (DADOS_LCD & 0x0F) | (0xF0 & c);
#else
    // compila código para os pinos de dados do LCD nos 4 LSB do PORT
    DADOS_LCD = (DADOS_LCD & 0xF0) | (c >> 4);
#endif

    pulso_enable();

// segundo nibble de dados - 4 LSB
#if (nibble_dados)
    // compila código para os pinos de dados do LCD nos 4 MSB do PORT
    DADOS_LCD = (DADOS_LCD & 0x0F) | (0xF0 & (c << 4));
#else
    // compila código para os pinos de dados do LCD nos 4 LSB do PORT
    DADOS_LCD = (DADOS_LCD & 0xF0) | (0x0F & c);
#endif

    pulso_enable();

    // se for instrução de retorno ou limpeza espera LCD estar pronto
    if ((cd == 0) && (c < 4))
        _delay_ms(2);
}

//---------------------------------------------------------------------------------//
// Sub-rotina para inicialização do LCD com via de dados de 4 bits
//---------------------------------------------------------------------------------//
void inic_LCD_4bits() // sequência ditada pelo fabricando do circuito integrado 
{                     // HD44780 o LCD será só escrito. Então, R/W é sempre zero.

    clr_bit(CONTR_LCD, RS); // RS em zero indicando que o dado para o LCD será uma
                            // instrução
    clr_bit(CONTR_LCD, E);  // pino de habilitação em zero

    _delay_ms(20); // tempo para estabilizar a tensão do LCD, após 
                   // VCC ultrapassar 4.5 V (na prática pode ser maior).

// interface de 8 bits
#if (nibble_dados)
    DADOS_LCD = (DADOS_LCD & 0x0F) | 0x30;
#else
    DADOS_LCD = (DADOS_LCD & 0xF0) | 0x03;
#endif

    pulso_enable(); // habilitação respeitando os tempos de resposta do LCD
    _delay_ms(5);
    pulso_enable();
    _delay_us(200);
    pulso_enable(); 
    /*  
        Até aqui ainda é uma interface de 8 bits.
        Muitos programadores desprezam os comandos acima, 
        respeitando apenas o tempo de estabilização da 
        tensão (geralmente funciona). Se o LCD não for 
        inicializado primeiro no modo de 8 bits, haverá 
        problemas se o microcontrolador for inicializado 
        e o display já o tiver sido.
    */

// interface de 4 bits, deve ser enviado duas vezes (a outra está abaixo)
#if (nibble_dados)
    DADOS_LCD = (DADOS_LCD & 0x0F) | 0x20;
#else
    DADOS_LCD = (DADOS_LCD & 0xF0) | 0x02;
#endif

    pulso_enable();
    cmd_LCD(0x28, 0); // interface de 4 bits 2 linhas (aqui se habilita
                      // as 2 linhas) são enviados os 2 nibbles (0x2 e 0x8)
    cmd_LCD(0x08, 0); // desliga o display
    cmd_LCD(0x01, 0); // limpa todo o display
    cmd_LCD(0x0C, 0); // mensagem aparente cursor inativo não piscando
    cmd_LCD(0x80, 0); // inicializa cursor na primeira posição a esquerda-1a linha
}

//---------------------------------------------------------------------------------//
// Variáveis do timer e funções do                                                           
//---------------------------------------------------------------------------------//

long click_a = 0;       // momento do último click do botão A
long click_b = 0;       // momento do último click do botao B
char last_a = 1 << PB5; //último estado do botao A
char last_b = 1 << PB4; //último estado do botao B

unsigned char count_a = 0;  // conta os pontos do time A
unsigned char count_b = 0;  // conta os pontos do time B

long timer;                 // tempo restante
long total_time = 600000;   // tempo total e inicial
/*
    inicia em 9:59 já que millis() não
    retornará zero no inicio do loop
*/

int min, sD1, sD0, msD1, msD0; // guarda os dígitos do timer
int pA1, pA0, pB1, pB0;        // guarda os dígitos do placar

void get_time()
{
    // calcula tempo restante
    timer = total_time - millis();
    // separa os dígitos
    min  = timer / 60000;
    sD1  = (timer % 60000) / 10000;
    sD0  = (timer % 10000) / 1000;
    msD1 = (timer % 1000) / 100;
    msD0 = (timer % 100) / 10;

    min  += '0'; //já converte pra ascii
    sD1  += '0';
    sD0  += '0'; 
    msD1 += '0';
    msD0 += '0';
}

void count_points()
{
    // lê os botões
    char read_a = PINB & (1 << PB5);
    char read_b = PINB & (1 << PB4);

    // verifica se houve mudança de estado nos botões
    // e quando foi a última mudança (debounce)
    if (read_a != last_a && (millis() - click_a) > 10)
    {
        click_a = millis();
        // incrementa pontuação circularmente
        if (read_a == 0)
            count_a = (count_a + 1) % 100;
        last_a = read_a;
    }

    // separa dígitos da pontuação e converte para ascii
    pA0 = (count_a % 10) + '0';
    pA1 = (count_a / 10) + '0';

    // análogo ao A, para a pontuação de B
    if (read_b != last_b && (millis() - click_b) > 10)
    {
        click_b = millis();
        if (read_b == 0)
            count_b = (count_b + 1) % 100;
        last_b = read_b;
    }
    pB0 = (count_b % 10) + '0';
    pB1 = (count_b / 10) + '0';
}

//---------------------------------------------------------------------------------//
// Execução
//---------------------------------------------------------------------------------//

void setup()
{
    DDRC = 0xFF; // porta C como saida

    // Botões
    // Pinos 12 e 13 como entrada 
    // (botões B e A, respectivamente)
    DDRB &= ~(1 << DDB4) & ~(1 << DDB5);
    // Ativa pull-up dos botões
    PORTB |= (1 << PB4) | (1 << PB5);

    inic_LCD_4bits();
}

void loop()
{
    // calcula tempo
    get_time();

    // calcula pontos a partir da leitura dos botões
    // para de contar pontos depois do fim do tempo
    if (timer > 0) count_points();

    // Apresenta informações no display

    // PLACAR DO TIME A
    cmd_LCD(0x80, 0);  // posiciona cursor no local desejado (2o argumento 0)
    cmd_LCD(pA1,  1);  // escreve digitos (2o argumento 1)
    cmd_LCD(pA0,  1);

    // TIMER
    cmd_LCD(0x84, 0);
    cmd_LCD('0',  1);
    cmd_LCD(min,  1);
    cmd_LCD(':',  1);
    cmd_LCD(sD1,  1);
    cmd_LCD(sD0,  1);
    cmd_LCD(':',  1);
    cmd_LCD(msD1, 1);
    cmd_LCD(msD0, 1);

    // PLACAR DO TIME B
    cmd_LCD(0x8E, 0);
    cmd_LCD(pB1,  1);
    cmd_LCD(pB0,  1);

    // delay pro tinkercad não explodir
    delay(20);
}