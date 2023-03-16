#include <util/delay.h>
#include <avr/interrupt.h>
volatile uint8_t state;

int main(void){
// Set in/out
DDRB |= (1<<DDB1)|(1<<DDB2)|(1<<DDB3); // Set pins 9, 10, 11 to write
DDRD &= ~(1<<DDD2); // Set pin 3 to read


state = 1; // Default on

EICRA |= (1 << ISC00) | (1 << ISC01);  // INT0 will trigger on a rising edge
EIMSK |= (1 << INT0);                  // INT0 enabled

sei(); // All interupts enabled

// Defaults

PORTB |= (1<<DDB1)|(1<<DDB2)|(1<<DDB3); // Default all LEDs to be on

while(1){
    if(state == 1){
        // Blink the LEDs if the state is 1
        PORTB |= (1<<PB3);
        PORTB &= ~(1<<PB1);
        _delay_ms(500);
        PORTB &= ~(1<<PB3);
        PORTB |= (1<<PB1);
        _delay_ms(500);
    } else {
        for (int i = 0; i < 4; i++) {
        
        PORTB |= (1<<PB3);
        PORTB &= ~(1<<PB1);
        _delay_ms(125);
        PORTB &= ~(1<<PB3);
        PORTB |= (1<<PB1);
        _delay_ms(125);
        
        }
    }
}
}


ISR(INT0_vect){
state = ~state;
Serial.print("state switch");
}