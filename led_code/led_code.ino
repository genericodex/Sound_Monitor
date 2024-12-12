#define LED_PIN PB0 // Pin 8 for LED

void setup() {
    Serial.begin(9600);
    Serial.println("Slave: LED Arduino Initialized");

    DDRB |= (1 << LED_PIN); // LED pin as output
    DDRB &= ~(1 << PB4);    // MISO as input
    SPCR = (1 << SPE);      // Enable SPI in Slave mode
}

ISR(SPI_STC_vect) {
    char command = SPDR; // Read received command
    Serial.print("Received command: ");
    Serial.println(command);

    if (command == 'O') {
        PORTB |= (1 << LED_PIN); // Turn on LED
        Serial.println("LED ON");
    } else if (command == 'F') {
        PORTB &= ~(1 << LED_PIN); // Turn off LED
        Serial.println("LED OFF");
    }
}

void loop() {
    // Handled by ISR
}
