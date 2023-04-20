//This function store the ADC result into ring buffer Requirement: 
//Read A0 value
//Read at a rate of 200 samples/s (5ms to overflow)
//Use external reference of 5V at AREF
//Left adjust ADC
//Use only 8-bits ADC
//Use Prescalar factor to 128
//Store value to ring buffer of unsigned integers to be at least 1000 samples. 


#include <util/delay.h>
#include <avr/io.h>   //allows for register commands to be understood

//Declare variable for ring buffer
#define SIZE_OF_BUFFER 1000
int start = -1;
int end   = -1;
uint8_t database[SIZE_OF_BUFFER] = {0};
 
//Set ADC variables
bool state = 0;
uint8_t value;

// Check if the array is full
bool isFull() {
  if ((start == (end + 1)) || ((start == 0) && (end == SIZE_OF_BUFFER - 1))){
    return 1;
  }
  return 0;
}

//Add value to end of array
void add(uint16_t value){
    if (start == -1){
      start = 0;
    }
    end = (end + 1) % SIZE_OF_BUFFER;
    database[end] = value;
}

//Remove first value from array
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
 state=1;
}

int main(void) {
  cli();
  
  //Set Left Adjust
  ADMUX |= (1 << ADLAR);
  
  //Set ADCSRA Register
  ADCSRA |=(1 << ADEN);//ENable ADC
  ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);//Prescale factor 128
  
  //Set timer
  TCCR1B |= (1<<WGM12); //CTC Mode
  TCCR1B |= (1<<CS11)|(1<<CS10);//Set prescalar to 64
  TCNT1 = 0; //Initialise timer to 0
  TCCR1A |= (1<<COM1A0);//OC1A is toggled on match
  OCR1A = 1250; //Set compare value Correspond to 200Hz count
  TIMSK1 |= (1<<OCIE1A);//Enable CTC interrupt for OCR1A
  
  sei();
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600); 


  // the loop routine runs over and over again forever:
  while(1) {
    if(state){
      ADCSRA |= (1 << ADSC);//start conversion
      value = ADCH;
      //Use Ring Buffer to store data
      if(isFull()){
        removeFirst();
        add(value);
      }else{
        add(value);
      }

      // print out the value you read:
      Serial.println(database[end]);
      state = 0;
    }
    
  }
}