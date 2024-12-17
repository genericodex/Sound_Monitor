#define LED_PIN PB0 // LED connected to PB0

void setup() {
    // Set LED pin as output
    DDRB |= (1 << LED_PIN); 
    
    // SPI Setup in Slave Mode
    DDRB |= (1 << PB4);              // MISO as output
    SPCR = (1 << SPE) | (1 << SPIE); // Enable SPI and SPI interrupt

    // Enable Global Interrupts
    sei();
}

// SPI Interrupt Service Routine
ISR(SPI_STC_vect) {
    char command = SPDR; // Read received data
    
    if (command == 'O') {
        PORTB |= (1 << LED_PIN); // Turn ON LED
    } else if (command == 'F') {
        PORTB &= ~(1 << LED_PIN); // Turn OFF LED
    }
}

void loop() {
    // Main loop is empty; LED control handled via ISR
}
