#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN PB0  // Pin 8 for LED

void setup() {
    // Set up LED pin as output
    DDRB |= (1 << LED_PIN); // Set PB0 (Pin 8) as output

    // Initialize SPI
    DDRB |= (1 << PB4);              // MISO as output
    SPCR = (1 << SPE) | (1 << SPIE); // Enable SPI and SPI interrupt

    sei(); // Enable global interrupts
}

// SPI interrupt service routine
ISR(SPI_STC_vect) {
    char received = SPDR; // Read the data received
    if (received == 'O') {
        PORTB |= (1 << LED_PIN); // Turn on LED (HIGH)
    } else if (received == 'F') {
        PORTB &= ~(1 << LED_PIN); // Turn off LED (LOW)
    }
}

void loop() {
    // The ISR handles LED control
}
