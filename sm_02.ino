//https://www.tinkercad.com/things/eMnAkYqCIvO-sm-02

void setup()
{
  //seta todas as portas D como saída
  DDRD |= 0xFFF;                          
}

void a()
{
  for(int i = 0; i < 8; i++)
  {
    //ativa as portas D de 0 a 7, uma de cada vez.
    //por usar o operador cumulativo |=
    //as portas ativadas anteriormente continuam ativas.
  	PORTD |= (1 << i); 	
    //	00000001
    //	00000010
    //	00000100 OU
    //  ...
    //	10000000
    //------------
    //  11111111
   
    delay(300);
  }
  
  //desativa todas as portas D de uma vez
  PORTD &= 0;
  //  11111111 E
  //  00000000
  //------------
  //  00000000
  
  delay(300);
}

void b()
{
  for(int i = 7; i >= 0; i--)
  {
    //ativa as portas D de 7 a 0, uma de cada vez.
    //por usar o operador cumulativo |=
    //as portas ativadas anteriormente continuam ativas.
  	PORTD |= (1 << i);
    //	10000000
    //	01000000
    //	00100000 OU
    //  ...
    //	00000001
    //------------
    //  11111111
    
    delay(300);
  }
  
  //desativa todas as portas D de uma vez
  PORTD &= 0;
  //  11111111 E
  //  00000000
  //------------
  //  00000000
  
  delay(300);
}

void c()
{
  for(int i = 0; i < 8; i++)
  {
    //ativa as portas D de 0 a 7, uma de cada vez.
  	PORTD |= (1 << i);
    delay(300);
    
    //desativa as portas D de 0 a 7, uma de cada vez.
    PORTD &= ~(1 << i);
    //EX:
    //	00100000 E
    //  11011111
    //------------
    //  00000000
    
    delay(300);
  }
}

void d()
{
  for(int i = 0; i < 8; i++)
  {
    //ativa as portas D de 0 a 7, uma de cada vez.
  	PORTD |= (1 << i);
    delay(300);
    
    //desativa as portas D de 0 a 7, uma de cada vez.
    PORTD &= ~(1 << i);    
    delay(300);
  }

  for(int i = 7; i >= 0; i--)
  {
    //ativa as portas D de 7 a 0, uma de cada vez.
  	PORTD |= (1 << i);
    delay(300);
    
    //desativa as portas D de 7 a 0, uma de cada vez.
    PORTD &= ~(1 << i);
    delay(300);
  }
}

void loop()                              
{
  a();
  delay(1500);
  
  b();
  delay(1500);
  
  c();
  delay(1500);
  
  d(); 
  delay(1500);
}