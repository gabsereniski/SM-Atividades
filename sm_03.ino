//https://www.tinkercad.com/things/aKkdZLz9ytl-sm-03

/*
não consegui implementar o clique longo do botão, 
então deixei o código realizando apenas as trocas de led, 
como explicado na questão 4
*/


char b1, b2; 	//botões

char ls1 = 1;	//last state botão 1
char ls2 = 1;	//last state botão 2
char led = 0;	//variável que seleciona led

//apaga todos os leds
void leds_off()
{
  	PORTD &= ~(1<<PD6);
  	PORTB &= ~(1<<PB1);
  	PORTC &= ~(1<<PC4);
}

//acende led selecionado
void turn_on(char led)
{
  	switch(led)
    {
      	case 0: PORTD |= (1<<PD6); break;
        case 1: PORTB |= (1<<PB1); break; 
        case 2: PORTC |= (1<<PC4); break;
    }
}

void setup()
{
  	//configura todos os pinos como entrada
  	DDRB = 0;
  	DDRC = 0;
  	DDRD = 0;
    
  	//ativa pull up nos pinos 2 e 3
    PORTD |= (1<<PD2) | (1<<PD3);
	
  	//configura pinos dos led como saída
    DDRD |= (1<<PD6);
    DDRB |= (1<<PB1);
    DDRC |= (1<<PC4);
}

void loop()
{
  	//lê sinais dos botões
  	b1 = PIND & (1<<PD2);
  	b2 = PIND & (1<<PD3);
  	
  	//se houve mudança de estado no botão 1
    if(b1 != ls1)
    {    
        if(b1 != LOW) 
        {
          	//incrementa led circularmente
            led = (led + 1)%3;
			
          	//apaga todos os leds
            leds_off();          	
			
          	//acende apenas led selecionado
            turn_on(led);
        }
		
      	//atualiza último estado do botão 1
      	ls1 = b1;
    }
	
  	//se houve mudança de estado no botão 2
    if(b2 != ls2)
    {
        if(b2 != LOW)
        {
          	//"decrementa" led circularmente
            if(led == 0) 		led = 2;
            else if(led == 1) 	led = 0;
            else 				led = 1;
			
            //apaga todos os leds
            leds_off();
			
          	//acende apenas led selecionado
          	turn_on(led);
        }
		
      	//atualiza último estado do botão 1
        ls2 = b2;
    }
  
  	delay(10);     
}