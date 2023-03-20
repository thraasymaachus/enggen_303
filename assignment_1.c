#include <util/delay.h>
#include <avr/interrupt.h>
#define THRESHOLD 102

/*
Your program should loop, reading the voltage at pin A0 at a rate of 1 Sa/s. Use an external
reference voltage of 5 V (applied at pin AREF), and use the full 10-bit resolution of the ADC. Set
the ADC’s prescale factor to 128.

Here is a suggested hardware set-up: Connect the 5 V pin on the Arduino Uno to two resistors
in series, and in turn connect this series resistance to the Arduino Uno’s ground pin. Start
simple: use two 1 kOhm resistors. Connect the node between the two resistors to pin A0 of the
Arduino Uno’s ADC. You’ve built a simple voltage divider; in theory, what’s the voltage
appearing at pin A0? It's half of the voltage of the 5 V pin, i.e 2.5V.

Program the Arduino Uno’s on-board LED to verify that your program works to spec. If the
sampled voltage at pin A0 is >= 2 V, then on each 1-second cycle of your program blink the LED
twice; if the sampled voltage is < 2 V, then blink the LED once. In software, change this
threshold value (2 V) to convince yourself that your program works
*/

int main(void){

    // Set in/out
    DDRC |= (1 << DDC0) // Set pin A0 to output

    // Note: We don't need to configure MUX3..0 as this defaults to 0000, i.e ADC0
    // ADMUX |= (1 << REFS0);   // Don't adjust voltage reference for now, as we want our 5V external input as the reference (register default)
    // ADMUX |= (1 << MUX0);    // Don't adjust MUX3...0, as the default is correct
    // ADMUX |= (1 << ADLAR);  // We want to read in the full 10 bits, so don't adjust the ADLAR bit for now

    // Note: We can execute this as a single instruction, ato reduce the number of clock cycles. Same for the above.
    ADCSRA |= (1 << ADEN);  // Start the ADC
    ADCSRA |= (1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2); // Set the LSBs in ADCSRA to 111, which corresponds to a division factor of 128. This should give us 200Sa/s

    while(1){
        ADCSRA |= (1 << ADSC);  // Begin single conversion

        A0Read = (ADCL + (ADCH << 8));  // Our entire ADC value is contained within the ADCH register

        Serial.print("%i", &A0Read);

        // If pin A0 is greater than threshold, blink LED twice in a second. Otherwise, blink LED once in a second.
        if (A0Read > THRESHOLD){  // A0Read is (Vin * 2^8)/(Vref). We'll set the threshold to 2V (102 in DEC) at first, and move it up to check our expectations that behaviour changes at 2.5V.
            for (int i = 0; i < 2; i++){
                digitalWrite(LED, HIGH);
                _delay_ms(100);
                digitalWrite(LED, LOW);
                _delay_ms(100);
            }

            _delay_ms(600);

        } else {
            digitalWrite(LED, HIGH);
            _delay_ms(100);
            digitalWrite(LED, LOW);
            _delay_ms(900);
        }
    }
}