#define SOUND_SENSOR_PIN 0 // ADC0 Pin
#define SOUND_THRESHOLD 2.5 // Voltage threshold

// SPI Slave Select Pins (on PORTB)
#define SS_LCD_PIN PB2   // Slave Select for LCD (Pin 10)
#define SS_LED_PIN PB1   // Slave Select for LED (Pin 9)
#define SS_SWITCH_PIN PB0 // Slave Select for Switch (Pin 8)

void setup() {
    // Serial Setup
    UBRR0 = 103; // 9600 baud rate for 16 MHz clock
    UCSR0B |= (1 << TXEN0); // Enable UART TX

    // SPI Setup as Master
    DDRB |= (1 << SS_LCD_PIN) | (1 << SS_LED_PIN) | (1 << SS_SWITCH_PIN); // SS pins as output
    PORTB |= (1 << SS_LCD_PIN) | (1 << SS_LED_PIN) | (1 << SS_SWITCH_PIN); // Deselect slaves
    
    DDRB |= (1 << PB3) | (1 << PB5); // MOSI (PB3) and SCK (PB5) as output
    DDRB &= ~(1 << PB4); // MISO as input
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); // Enable SPI, Master mode, Clock = fosc/16

    // ADC Setup for Analog Input
    ADMUX = (1 << REFS0); // AVcc with external capacitor at AREF pin
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, prescaler 128
}

float readADC(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Select ADC channel
    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    return ADC * (5.0 / 1023.0); // Return voltage value
}

void sendToSlave(volatile uint8_t *slavePORT, uint8_t slavePin, const char *data) {
    *slavePORT &= ~(1 << slavePin); // Select slave
    for (int i = 0; data[i] != '\0'; i++) {
        SPDR = data[i]; // Start SPI transfer
        while (!(SPSR & (1 << SPIF))); // Wait for transfer to complete
    }
    *slavePORT |= (1 << slavePin); // Deselect slave
}

void sendToLCD(const char *data) {
    for (int i = 0; data[i] != '\0'; i++) {
        while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
        UDR0 = data[i]; // Send character
    }
}

void loop() {
    float analogValue = readADC(SOUND_SENSOR_PIN);
    
    char lcdData[16];
    dtostrf(analogValue, 6, 2, lcdData);
    sendToLCD(lcdData);

    // Send control signal to LED Arduino
    if (analogValue > SOUND_THRESHOLD) {
        sendToSlave(&PORTB, SS_LED_PIN, "O"); // LED ON
    } else {
        sendToSlave(&PORTB, SS_LED_PIN, "F"); // LED OFF
    }
    
    _delay_ms(500);
}
