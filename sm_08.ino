//https://www.tinkercad.com/things/0icaedCVyGh-sm-08

#define F_CPU 16000000UL // define a frequencia do microcontrolador - 16MHz
#include <stdio.h>
#include <string.h>

#define BAUD 2400 // taxa de 2400 bps
#define MYUBRR F_CPU / 16 / BAUD - 1

#define BUFFLEN 16
// buffer de dados de envio na porta serial
char out_buffer[BUFFLEN];
// buffer que recebe os dados pela porta serial
char in_buffer[4];
// pos indica a posição corrente no buffer
// end indica a posição final no buffer com dados válidos
// note que end pode ser menor que pos, já que o buffer é circular
// cntbuff indica quantos caracteres válidos há no buffer
short pos = 0, end = 0, cntbuff = 0;

void USART_Inic(unsigned int ubrr0)
{
    UBRR0H = (unsigned char)(ubrr0 >> 8); // ajusta a taxa de transmissão
    UBRR0L = (unsigned char)ubrr0;

    // desabilitar velocidade dupla (no Arduino é habilitado por padrão)
    UCSR0A = 0;                                           
    // Habilita a transmissão e a recepção
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); 
    /*modo assíncrono, 8 bits de dados, 1 bit de parada, sem paridade*/
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);               

    sei();
}

void EscreveAsync(char *dados, short len)
{
    // desabilita temporariamente interrupção
    UCSR0B &= ~(1 << UDRIE0);
    // não deixa enfileirar mais dados que o tamanho do buffer
    len %= BUFFLEN;

    // separa a cópia dos dados para o buffer em duas
    // partes se necessário for
    cntbuff += len;
    short cnt = BUFFLEN - end;
    cnt = cnt > len ? len : cnt;

    memcpy(out_buffer + end, dados, cnt);
    if (cnt < len)
        memcpy(out_buffer, dados + cnt, len - cnt);

    end += len;
    end %= BUFFLEN;
    // habilita interrupção para (re)iniciar o envio
    UCSR0B |= (1 << UDRIE0);
}

ISR(USART_UDRE_vect)
{
    // enquanto houver dados no buffer
    if (cntbuff > 0)
    {
        UDR0 = out_buffer[pos]; // envia
        pos++;
        cntbuff--;
        pos %= BUFFLEN; // buffer circular
    }
    else
    {
        // se não houver o que enviar, desliga interrupção
        UCSR0B &= ~(1 << UDRIE0);
    }
}

ISR(USART_RX_vect)
{
    // desloca caracteres do buffer de entrada em uma posição
 	memmove(in_buffer, &in_buffer[1], 3);
    // coloca ultimo caracter recebido no espaço gerado
    in_buffer[2] = UDR0;

    // verifica se a string forma um comando válido
    if (!strcmp(in_buffer, "A13") || !strcmp(in_buffer, "a13"))
        PORTB |= (1 << PB5);
    
    else if (!strcmp(in_buffer, "S13") || !strcmp(in_buffer, "s13"))
        PORTB &= ~(1 << PB5);

    else if (!strcmp(in_buffer, "D12") || !strcmp(in_buffer, "d12"))
        PORTB ^= (1 << PB4);
}

void envia_millis(unsigned long ms)
{
    char str[12];
    sprintf(str, "%lu\n", ms);
    EscreveAsync(str, strlen(str));
}

int main()
{
    init();

    // pinos 12 e 13 como saída
    DDRB |= (1 << DDB4) | (1 << DDB5); 
  	// coloca '\0' no fim da string
	in_buffer[3] = 0;

    USART_Inic(MYUBRR);

    while(1)
    {
        _delay_ms(100);
        envia_millis(millis()); // não bloqueante
    }
}
