//https://www.tinkercad.com/things/dmWT68rSXgv-sm-04

#define F_CPU 16000000UL //define a frequência do microcontrolador em 16MHz

#include <avr/io.h> //definições do componente especificado
#include <util/delay.h> //biblioteca para o uso das rotinas de _delay_
#include <avr/pgmspace.h> //biblioteca para poder gravar dados na memória flash

const unsigned char table[] PROGMEM = {
	0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 
	0x00, 0x18, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E
};

long click_a = 0;		//momento do último click do botão A
long click_b = 0;		//momento do último click do botao B
char last_a = 1<<PB4;	//último estado do botao A
char last_b = 1<<PB5;	//último estado do botao B

unsigned char count_a = 0;	//conta os pontos do time A
unsigned char count_b = 0;	//conta os pontos do time B

long timer;					//tempo restante
long total_time = 600000;	//tempo total e inicial 
							/*	
								inicia em 9:59 já que millis() não 
								retornará zero no inicio do loop
							*/

int min, sD1, sD0;			//guarda os dígitos do timer
int pA1, pA0, pB1, pB0;		//guarda os dígitos do placar

void setup()
{
    UCSR0B = 0x00;
    
    // Segmentos
    //PORTD como saída (seg. a b c d)
    DDRD |= (1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7);
    //PORTC como saída (seg. e f g)
    DDRC |= (1<<PC0)|(1<<PC1)|(1<<PC2);
    
    //desliga os segmentos do display
    PORTD |= (1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7);	
  	PORTC |= (1<<PC0)|(1<<PC1)|(1<<PC2);
	
	// Displays
  	//seta como saída pinos dos displays do placar A
  	DDRD |= (1<<DDD2) | (1<<DDD3);
    //seta como saída pinos dos displays do placar B
    DDRD |= (1<<DDD0) | (1<<DDD1);
  	//seta como saída pinos dos displays do timer
  	DDRC |= (1<<DDC3) | (1<<DDC4) | (1<<DDC5);
  
    // Botões
    // Pinos 12 e 13 como entrada
    DDRB &= ~(1<<DDB4) & ~(1<<DDB5);
    // Ativa pull-up dos botões
    PORTB |= (1<<PB4)|(1<<PB5) ;
}

void get_time()
{
	//calcula tempo restante
  	timer = total_time - millis();
	//separa os dígitos
  	min = timer / 60000;
  	sD1 = (timer % 60000) / 10000;
  	sD0 = (timer % 10000) / 1000;
}

void count_points()
{
	//lê os botões
    char read_a = PINB & (1<<PB4);
    char read_b = PINB & (1<<PB5);
	
	//verifica se houve mudança de estado nos botões
	//e quando foi a última mudança (debounce)
    if(read_a != last_a && (millis() - click_a) > 10)
    {
        click_a = millis();
		//incrementa pontuação circularmente
        if(read_a == 0) count_a = (count_a + 1) % 100;
        last_a = read_a;
    }

	//separa dígitos da pontuação
  	pA0 = count_a % 10;
  	pA1 = count_a / 10;
  
	//análogo ao A, para a pontuação de B
    if(read_b != last_b && (millis() - click_b) > 10)
    {
        click_b = millis();
        if(read_b == 0) count_b = (count_b + 1) % 100;
        last_b = read_b;
    }
  	pB0 = count_b % 10;
  	pB1 = count_b / 10;
}

void set_segments(char d)
{
	//pega bits do dígito correto da tabela
	unsigned char segs = pgm_read_byte(&table[d]);
  	
	//apaga o que havia nos bits mais significativos do PORTD
  	PORTD &= 0b00001111;
	//coloca valor equivalente aos segmentos a, b, c e d (4 bits menos 
	//significativos da tabela) na parte mais significativa do PORTD
  	PORTD |= (segs << 4);
	//apaga o que havia nos dígitos menos significativos do PORTC
  	PORTC &= 0b11111000;
	//coloca valor equivalente aos segmentos e, f e g (3 bits mais
	//significativos da tabela) na parte menos significativa do PORTC
  	PORTC |= (segs >> 4);
}

void loop()
{
	//calcula tempo   	
  	get_time();
	
	//calcula pontos a partir da leitura dos botões
	//para de contar pontos depois do fim do tempo
	if(timer > 0) count_points();
	
	/*
		- ativa segmentos de cada dígito;
		- acende e apaga displays em ordem
		  de aparecimento;
	*/

  	/*-----------PLACAR DO TIME A*/
	// dígito 1
	set_segments(pA1);
  	PORTD |= (1 << PD3);
  	_delay_ms(1);
    PORTD &= ~(1 << PD3);
 	// dígito 0
	set_segments(pA0);
  	PORTD |= (1 << PD2);
  	_delay_ms(1);
  	PORTD &= ~(1 << PD2);
	
	/*----------------------TIMER*/
  	// minutos
	set_segments(min);
  	PORTC |= (1 << PC3);
  	_delay_ms(1);
  	PORTC &= ~(1 << PC3);
  	// dígito 1 dos segundos
	set_segments(sD1);
  	PORTC |= (1 << PC4);
  	_delay_ms(1);
  	PORTC &= ~(1 << PC4);
  	// dígito 0 dos segundos
	set_segments(sD0);
  	PORTC |= (1 << PC5);
  	_delay_ms(1);
  	PORTC &= ~(1 << PC5);
  	
	/*-----------PLACAR DO TIME B*/
  	// dígito 1
	set_segments(pB1);
    PORTD |= (1 << PD1);
  	_delay_ms(1);
  	PORTD &= ~(1 << PD1);
	// dígito 0
	set_segments(pB0);
  	PORTD |= (1 << PD0);
  	_delay_ms(1);
  	PORTD &= ~(1 << PD0);

	//delay pro tinkercad não explodir
    delay(20); 
}