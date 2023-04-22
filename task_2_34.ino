// This script implements a FSM of ADC with 2 states Requirement:
// Reset ring buffer before go to state 1
// State 1: Continuously record to ring  buffer
// State 2: Stop recording and transfers the contents of the ring buffer to PC
// Stay in stage 2 until a button press
// Make button responsive all time
// Blink LED to show status

#include <util/delay.h>
#include <avr/io.h>

// Declare state variable
#define S0 0
#define S1 1
#define S2 2
int state = S0;
int readnow = S0;

// Declare Ring buffer variables
#define SIZE_OF_BUFFER 1000
volatile int start = -1;
volatile int end = -1;
volatile int printfrom = -1;
volatile int numofvalue = 0;
uint8_t volatile database[SIZE_OF_BUFFER] = {0};

// Other variables
volatile uint8_t val;

ISR(INT0_vect)
{
  if (state == S0)
  {
    printfrom = start;
    state = S1;
  }
  else
  {
    start = -1;
    end = -1;
    numofvalue = 0;
    state = S0;
  }
}

ISR(TIMER1_COMPA_vect)
{
  readnow = S1;
}

int main()
{
  cli();

  // Button setting
  //  Set pin as input
  DDRD &= ~(1 << DDD2);
  // Enable external interrupt on rising edge of INT0
  EICRA |= (1 << ISC00) | (1 << ISC01);
  EIMSK |= (1 << INT0);

  // Set timer 0 - for LED

  // Set timer 1 - for ADC
  TCCR1B |= (1 << WGM12);              // CTC Mode
  TCCR1B |= (1 << CS11) | (1 << CS10); // Set prescalar to 64
  TCNT1 = 0;                           // Initialise timer to 0
  TCCR1A |= (1 << COM1A0);             // OC1A is toggled on match
  OCR1A = 1250;                        // Set compare value Correspond to 200Hz count
  TIMSK1 |= (1 << OCIE1A);             // Enable CTC interrupt for OCR1A

  // Set timer 2 - for printing

  // Set ADC
  ADMUX |= (1 << ADLAR);
  ADCSRA |= (1 << ADEN);                                // ENable ADC
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescale factor 128

  sei();

  // Set on-board LED to output
  DDRB |= (1 << DDB5);

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  while (1)
  {
    switch (state)
    {
    case S0:
      switch (readnow)
      {
      case S0:
        break;
      case S1:
        ADCstart();
        readnow = S0;
        break;
      }
      break;
    case S1:
      switch (readnow)
      {
      case S0:
        break;
      case S1:
        send_data();
        readnow = S0;
        break;
      }
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
  if (start == -1)
  {
    start = 0;
  }
  end = (end + 1) % SIZE_OF_BUFFER;
  database[end] = value;
  numofvalue++;
}

// Remove value from array
void removeFirst()
{
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

  if(numofvalue!=0){
    Serial.println(database[printfrom]);
    printfrom = (printfrom + 1) % SIZE_OF_BUFFER;
    numofvalue--;
  }
}
