#include <avr/interrupt.h>
volatile uint8_t timer_state;
volatile uint8_t mode;
volatile uint8_t unpowered;

// The CTC interrupt, triggered at OCR1A matching
ISR(TIMER1_COMPA_vect) {
  timer_state ^= 0x01;  // Step 6: toggle a timer state variable to be used in main loop
}
// External interrupt INT0
ISR(INT0_vect) {
  if (!unpowered) {
    mode ^= 0x01;
  }
}
// External interrupt INT1
ISR(INT1_vect) {
  unpowered ^= 0x01;
}
int main(void) {
  // Configure I/O using DDRB and DDRD (reuse Task 1)
  DDRB |= (1 << DDB3);
  DDRB |= (1 << DDB1);
  DDRD &= ~(1 << DDD2);
  DDRD &= ~(1 << DDD3);
  mode = 1;       // Default to alternating
  unpowered = 0;  // Default to powered on
  timer_state = 0;


  cli();  // Disable all interrupts during configuration
  // Configure external interrupts using EICRA and EIMSK (reuse Task 1)
   EIMSK |= (1<<INT0); // Enables INT0 interupt
  EIMSK |= (1<<INT1); // Enables INT0 interupt
  // Configure EICRA here...
  EICRA |= (1<<ISC00)|(1<<ISC01)|(1<<ISC10)|(1<<ISC11); // Interupt on rising edge for both things
  // Step 2-5: configure timer interrupt related registers
  TCCR1A &= ~((1 << WGM11) | (1 << WGM10));  // With the next lines,
  TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS12);
  TCCR1B &= ~((1 << WGM13) | (1 << CS11));  // Set the timer to CTC mode and set prescaler to 1024

  // see Datasheet Table 15-5 and 15-6
  TCNT1 = 1;                // Initialize timer to 0
  OCR1A = 7812;             // Set CTC compare value
  TIMSK1 |= (1 << OCIE1A);  // Enable CTC interrupt for OCR1A compare match
  sei();                    // Enable all interrupts
  while (1) {
    if (unpowered) {
      PORTB &= ~(1 << PB3);
      PORTB &= ~(1 << PB1);
    } else if (mode == 1) {
      if (timer_state == 1) {
        PORTB |= (1 << PB3);
        PORTB &= ~(1 << PB1);
      } else {
        PORTB &= ~(1 << PB3);
        PORTB |= (1 << PB1);
      }

    } else {
      if (timer_state == 1) {
        PORTB |= (1 << PB3);
        PORTB |= (1 << PB1);
      } else {
        PORTB &= ~(1 << PB3);
        PORTB &= ~(1 << PB1);
      }
    }
  }
}
