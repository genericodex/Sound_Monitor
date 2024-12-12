#include <avr/io.h>
#include <util/delay.h>

#define SWITCH_PIN PD3 // Pin 3 for the switch

volatile uint8_t switchState = 0;

void setup() {
    // Set up switch pin as input
    DDRD &= ~(1 << SWITCH_PIN); // Set PD3 (Pin 3) as input
    PORTD |= (1 << SWITCH_PIN); // Enable pull-up resistor

    // Initialize SPI
    DDRB |= (1 << PB4);              // MISO as output
    SPCR = (1 << SPE) | (1 << SPIE); // Enable SPI and SPI interrupt

    sei(); // Enable global interrupts
}

uint8_t debounceSwitch() {
    static uint8_t state = 0xFF; // 8-bit state variable for debouncing
    state = (state << 1) | ((PIND & (1 << SWITCH_PIN)) >> SWITCH_PIN) | 0xE0;
    return (state == 0xF0); // Returns true if stable HIGH
}

// SPI interrupt service routine
ISR(SPI_STC_vect) {
    char received = SPDR; // Read the received data
    if (received == 'C') { // Master requests switch state
        uint8_t stableState = debounceSwitch();
        SPDR = stableState ? '1' : '0'; // Send switch state back to Master
    }
}

void loop() {
    // The ISR handles SPI communication
}
