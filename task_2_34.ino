#include <util/delay.h>
#include <avr/io.h>   //allows for register commands to be understood

//Store at least 1000 result in a ring buffer (unsigned integers arrays): Done
//sample rate 200sample/s DONE
//External 5V at AREF: DONE 
//Left Adjust ADC: DONE
//Use only 8 bit resolution: DONE
//Use ADC prescalar factor to 128: DONE

//Modify the circuit layout DONE
//Implement push button with two states: 1 to records and 2 to send data to PC
//Reset buffer when turn state 2 to 1
//Blink LED to show status
//Use PUTTY tools for input logging


// Declare variables
volatile bool state = 1;
volatile bool printdata = 1;
volatile uint8_t val;
volatile uint16_t reading;
volatile float voltage; 

//Declare variable for ring buffer
#define SIZE_OF_BUFFER 1000
volatile int start = -1;
int volatile end   = -1;
uint8_t volatile database[SIZE_OF_BUFFER] = {0};

// Check if the array is full
bool isFull() {
  if ((start == (end + 1)) || ((start == 0) && (end == SIZE_OF_BUFFER - 1))){
    return 1;
  }
  return 0;
}

//Add value to array
void add(uint8_t value){
    if (start == -1){
      start = 0;
    }
    end = (end + 1) % SIZE_OF_BUFFER;
    database[end] = value;
}

//Remove value from array
void removeFirst(){
  if (start == end) {
      start = -1;
      end = -1;
  }else {
      start = (start + 1) % SIZE_OF_BUFFER;
  } 
}

//Interrupt actions
ISR(TIMER1_COMPA_vect){
  
  if (state == 1) {
    //Serial.println("Reading...");
    ADCSRA |= (1 << ADSC);//start conversion
    //Use Ring Buffer to store data

    val = ADCH;
    if(isFull()){
      removeFirst();
      add(val);
    }else{
      add(val);
    }

    // print out the value you read:
    //Serial.println(database[end]);
  }
}

ISR(INT0_vect) {
  //Serial.println("Enter button ISR...");
  state = !state;
  printdata = 1;

  //Serial.println("Exit button ISR...");
  //Serial.print("State is ");
  //Serial.println(state);
}

int main(void) {
  cli();

  // Set Pin 2 on Port D to read
  DDRD &= ~(1 << DDD2);
  
  //Set Left Adjust (8 bit ADC)
  ADMUX |= (1 << ADLAR);
  
  // Start the ADC, and set the LSBs in ADCSRA to 111, which corresponds to a division factor of 128. This should give us 200Sa/s
  ADCSRA |= 0b10000001; // ADCSRA |= (1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2)|(1 << ADEN);
  
  //Set timer
  TCCR1B |= (1<<WGM12); //CTC Mode
  TCCR1B |= (1<<CS11)|(1<<CS10);//Set prescalar to 64
  TCNT1 = 0; //Initialise timer to 0
  TCCR1A |= (1<<COM1A0);//OC1A is toggled on match
  OCR1A = 1250; //Set compare value Correspond to 200Hz count
  TIMSK1 |= (1<<OCIE1A);//Enable CTC interrupt for OCR1A

  //Set external interupt 0 (push button)
  EICRA |= (1 << ISC00)|(1 << ISC01);
  EIMSK |= (1 << INT0);

  sei();
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600); 


  // the loop routine runs over and over again forever:
  while(1) {
    if ((state == 0)&&(printdata==1)) {
      //Serial.println("Begin data readout...");
      for (int i = start; i < SIZE_OF_BUFFER+start; i++) { // Check whether 2nd statement needs to be i != end + 1 or something
        int count = (i+1)%SIZE_OF_BUFFER;
        Serial.println(database[count]);
        //reading = (database[count]<<2)|(0b11);
        //voltage = reading*5.0/1023.0;
          
        //Serial.println(voltage);
        //_delay_ms(5);
      }

      start = -1;
      end = -1;
      printdata = 0;
      
      //Serial.println("End data readout...");
   }
  }
}