void setup() {
    Serial.begin(9600);  // Initialize Serial communication

    // Set PB0 (Pin 8) as output for LED
    DDRB |= (1 << PB0);
}

void loop() {
    if (Serial.available() > 0) {
        String message = Serial.readStringUntil('\n');

        // Parse the message to extract the voltage level
        float soundLevel = message.toFloat();

        if (soundLevel > 3.0) { // Example threshold for high sound level
            PORTB |= (1 << PB0);  // Turn on LED
        } else {
            PORTB &= ~(1 << PB0); // Turn off LED
        }
    }
}
