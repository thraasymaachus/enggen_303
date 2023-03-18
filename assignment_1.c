#include <util/delay.h>
#include <avr/interrupt.h>
volatile uint8_t state;

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

    threshold = 2; // Voltage threshold to 

    while(1){
        // If pin A0 is greater than threshold, blink LED twice in a second. Otherwise, blink LED once in a second.
        if ((PINC && (1 << DDC0)) > threshold){
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