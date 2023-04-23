// This script implements a FSM of ADC with 2 states Requirement:
// Reset ring buffer before go to state 1
// State 1: Continuously record to ring  buffer
// State 2: Stop recording and transfers the contents of the ring buffer to PC
// Stay in stage 2 until a button press
// Make button responsive all time
// Blink LED to show status

#include <util/delay.h>
#include <avr/io.h>

// Declare states variable
#define S0 0
#define S1 1
int ButtonState = S0;
int actnow = S0;
int LEDstate = S0;
int count = 0;
int compare = 490;

// Declare Ring buffer variables
#define SIZE_OF_BUFFER 5
volatile int start = -1;
volatile int end = -1;
volatile int printfrom = -1;
volatile int numofvalue = 0;
uint8_t volatile database[SIZE_OF_BUFFER] = {0};
volatile uint8_t val;

ISR(INT0_vect)
{
  if (ButtonState == S0)
  {
    // Switch to send data

    printfrom = start; // The start of sending
    compare = 245;     // Adjust LED blinking frequency(quicker)
    ButtonState = S1;
  }
  else
  {
    // Switch to record data

    // Reset ring buffer
    start = -1;
    end = -1;
    numofvalue = 0;

    compare = 490; // Adjust LED blinking frequency(Slower)
    ButtonState = S0;
  }
}

ISR(TIMER1_COMPA_vect)
{
  // Act once when timer is triggered
  actnow = S1;
}

ISR(TIMER0_COMPA_vect)
{
  // Switch LED on/off at certain frequency
  count++;
  if (count >= compare)
  {
    if (LEDstate == S0)
    {
      LEDstate = S1;
    }
    else
    {
      LEDstate = S0;
    }
    count = 0;
  }
}

int main()
{
  cli();

  // Button setting
  DDRD &= ~(1 << DDD2); //  Set pin as input
  // Enable external interrupt on rising edge of INT0
  EICRA |= (1 << ISC00) | (1 << ISC01);
  EIMSK |= (1 << INT0);

  // Set timer 0 - for LED
  TCCR0B |= (1 << CS01) | (1 << CS00); // prescaler of 64
  TCNT0 = 0;                           // Initialise timer to 0
  TCCR0A |= (1 << COM0A0);             // OC0A is toggled on match
  TIMSK0 |= (1 << OCIE0A);             // Enable interrupt

  // Set timer 1 - for ADC reocrding and sending
  TCCR1B |= (1 << WGM12);              // CTC Mode
  TCCR1B |= (1 << CS11) | (1 << CS10); // Set prescalar to 64
  TCNT1 = 0;                           // Initialise timer to 0
  TCCR1A |= (1 << COM1A0);             // OC1A is toggled on match
  OCR1A = 1250;                        // Set compare value Correspond to 200Hz count
  TIMSK1 |= (1 << OCIE1A);             // Enable CTC interrupt for OCR1A

  // Set on-board LED to output
  DDRB |= (1 << DDB5);

  // Set ADC
  ADMUX |= (1 << ADLAR);                                // Left adjust ADLAR
  ADCSRA |= (1 << ADEN);                                // ENable ADC
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescale factor 128

  sei();

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  while (1)
  {
    // Record or send according to button state and timer state
    switch (actnow)
    {
    case S0:
      break;
    case S1:
      switch (ButtonState)
      {
      case S0:
        ADCstart();
        break;
      case S1:
        send_data();
        break;
      }
      actnow = S0;
      break;
    }

    // Blink LED
    switch (LEDstate)
    {
    case S0:
      PORTB |= (1 << PORTB5); // turn the LED on
      break;
    case S1:
      PORTB &= ~(1 << PORTB5); // turn the LED off
      break;
    }
  }
  return 0;
}

// Check if the array is full
bool isFull()
{
  if (numofvalue == SIZE_OF_BUFFER)
  {
    return 1;
  }
  return 0;
}

// Add value to array
void add(uint8_t value)
{
  //When there is no element
  if (start == -1)
  {
    start = 0;
  }
  //When there is more than 1 element
  end = (end + 1) % SIZE_OF_BUFFER;
  database[end] = value;
  numofvalue++;
}

// Remove value from array
void removeFirst()
{
  //When there is only 1 element
  if (start == end)
  {
    start = -1;
    end = -1;
  }
  else
  {
    start = (start + 1) % SIZE_OF_BUFFER;
  }
  numofvalue--;
}

void ADCstart()
{
  ADCSRA |= (1 << ADSC); // start conversion
  // Use Ring Buffer to store data

  val = ADCH;
  if (isFull())
  {
    removeFirst();
    add(val);
  }
  else
  {
    add(val);
  }
}

void send_data()
{

  if (numofvalue != 0)
  {
    Serial.println(database[printfrom]);
    printfrom = (printfrom + 1) % SIZE_OF_BUFFER;
    numofvalue--;
  }
}