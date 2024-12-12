#include <avr/io.h>

#define SOUND_SENSOR_PIN 0  // Analog input pin A0
#define SS_LCD PB2          // Slave Select for LCD (Pin 10)
#define SS_LED PB1          // Slave Select for LED (Pin 9)
#define SS_SWITCH PB0       // Slave Select for Switch (Pin 8)

void setup() {
    // Configure SPI pins
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << SS_LCD) | (1 << SS_LED) | (1 << SS_SWITCH); // MOSI, SCK, and SS pins as outputs
    DDRB &= ~(1 << PB4);  // MISO as input

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); // Enable SPI, Master mode, clock = F_CPU/16

    // Configure analog input for the sound sensor
    ADMUX = (1 << REFS0);                // Use AVcc as reference
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, prescaler = 128
}

float readAnalogValue(uint8_t pin) {
    ADMUX = (ADMUX & 0xF0) | (pin & 0x0F); // Select ADC channel
    ADCSRA |= (1 << ADSC);                 // Start conversion
    while (ADCSRA & (1 << ADSC));          // Wait for conversion to complete
    return ADC * (5.0 / 1023.0);           // Convert to voltage
}

void sendToSlave(uint8_t ssPin, const char *message) {
    PORTB &= ~(1 << ssPin); // Activate slave by pulling SS low
    while (*message) {
        SPDR = *message++;  // Send data byte
        while (!(SPSR & (1 << SPIF))); // Wait for transmission to complete
    }
    SPDR = '\n';            // End of message
    while (!(SPSR & (1 << SPIF)));
    PORTB |= (1 << ssPin);  // Deactivate slave by pulling SS high
}

void loop() {
    float analogValue = readAnalogValue(SOUND_SENSOR_PIN);
    char buffer[16];

    // Send to LCD
    snprintf(buffer, sizeof(buffer), "Voltage: %.2fV", analogValue);
    sendToSlave(SS_LCD, buffer);

    // Send to LED
    sendToSlave(SS_LED, (analogValue > 2.5) ? "ON" : "OFF");

    // Send to Switch
    sendToSlave(SS_SWITCH, "CheckSwitch");

    _delay_ms(500);
}
