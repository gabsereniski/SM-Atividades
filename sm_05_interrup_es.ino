//https://www.tinkercad.com/things/88XBshpF5dV-sm-05-interrupcoes

volatile long long last_click = 0;

void setup()
{
    DDRB |= (1 << DDB3) | (1 << DDB4) | (1 << DDB5);

    DDRD &= ~(1 << DDD2);
    PORTD |= (1 << PD2);

    DDRB &= ~(1 << DDB1);
    PORTB |= (1 << PB1);
    DDRC &= ~(1 << DDC3);
    PORTC |= (1 << PC3);
    DDRD &= ~(1 << DDD7);
    PORTD |= (1 << PD7);

    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01);

    PCICR |= (1 << PCIE0) | (1 << PCIE1) | (1 << PCIE2);

    PCMSK0 |= (1 << PCINT1);
    PCMSK1 |= (1 << PCINT11);
    PCMSK2 |= (1 << PCINT23);
}

void loop()
{
    delay(20);
}

ISR(INT0_vect)
{
    if (millis() - last_click > 100)
    {
        if (PORTB > 0b000111)
            PORTB &= 0b000111;
        else
            PORTB |= 0b111000;
    }
    last_click = millis();
}

ISR(PCINT0_vect)
{
    if (millis() - last_click > 100)
    {
        if (0 == (PINB & (1 << PB1)))
            PORTB ^= (1 << DDB5);
    }
    last_click = millis();
}

ISR(PCINT1_vect)
{
    if (millis() - last_click > 100)
    {
        if (0 == (PINC & (1 << PC3)))
            PORTB ^= (1 << DDB4);
    }
    last_click = millis();
}

ISR(PCINT2_vect)
{
    if (millis() - last_click > 100)
    {
        if (0 == (PIND & (1 << PD7)))
            PORTB ^= (1 << DDB3);
    }
    last_click = millis();
}