// This is a simple implementation of ADC. Requirement:
// Read voltage at A0
// Read at a rate of 1 sample/s
// Use external reference of 5v (AREF)
// Use 10-bit ADC resolution
// Use ADC Prescalar to 128
// Use on-board LED to show voltage in A0

#include <util/delay.h>
#include <avr/io.h>

int main(void)
{

  float voltage = 0;

  // Set on-board LED to output
  DDRB |= (1 << DDB5);

  // Set ADCSRA Register
  ADCSRA |= (1 << ADEN);                                // ENable ADC
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescale factor 128

  while (1)
  {
    // 1 loop/seconds, all delays adds up to 1000ms

    ADCSRA |= (1 << ADSC); // start conversion

    // Convert 10 bit ADC reading to voltage
    voltage = ADC * 5.0 / 1023.0;

    // Let LED show result
    if (voltage < 2.4)
    {
      blinkLED(2);
    }
    else
    {
      blinkLED(4);
    }
  }
}

//The function switch LED and delay n times in 1 second
void blinkLED(uint8_t n)
{
  if (n == 0)
  {
    _delay_ms(1000);
  }
  else
  {
    for (uint8_t i = 0; i < n/2; i++)
    {
      PORTB |= (1 << PORTB5); // turn the LED on
      _delay_ms(1000 / n);

      PORTB &= ~(1 << PORTB5); // turn the LED off
      _delay_ms(1000 / n);
    }
  }
}