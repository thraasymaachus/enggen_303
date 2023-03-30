#include <util/delay.h>
#include <avr/io.h>   //allows for register commands to be understood


int main(void)
{
  
  //Declare variables
  unsigned int analogDataA,analogDataB,result;
  float voltage = 0;
  
  //Set PIn13 to output
  DDRB |= (1<<DDB5);
  
  //Set ADMUX Register,use ADC0(A0) pin
  ADMUX |= (1 << REFS0);//use external reference voltage of 5V at pinAREF
  
  //Set ADCSRA Register
  ADCSRA |=(1 << ADEN);//ENable ADC
  ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);//Prescale factor 128

  while(1){
    //1 loop/seconds, all delays adds up to 1000ms
    
    ADCSRA |= (1 << ADSC);//start conversion
    
    // read the analog value:
    analogDataB = ADCL;
    analogDataA = ADCH;
    
    //Combine 2 reading to 10 bit
    result = (analogDataA<<8)|(analogDataB);
    
    //Convert reading to voltage
    voltage = result*5.0/1023.0;
    
    //Let LED show result
    if(voltage < 2.4){
       
    	PORTB |= (1<<PORTB5);// turn the LED on
    	_delay_ms(500);

    	PORTB &= ~(1<<PORTB5);// turn the LED off
    	_delay_ms(500);
    }else{
      	PORTB |= (1<<PORTB5);// turn the LED on
    	_delay_ms(250);

    	PORTB &= ~(1<<PORTB5);// turn the LED off
    	_delay_ms(250);
      
      	PORTB |= (1<<PORTB5);// turn the LED on
    	_delay_ms(250);

    	PORTB &= ~(1<<PORTB5);// turn the LED off
    	_delay_ms(250);
    }
 }
}