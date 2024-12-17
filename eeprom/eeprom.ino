#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SWITCH_PIN PD3

void EEPROM_Write(uint16_t address, uint8_t data) {
    while (EECR & (1 << EEPE)); // Wait for EEPROM to be ready
    EEAR = address;            // Set EEPROM address
    EEDR = data;               // Set EEPROM data
    EECR |= (1 << EEMPE);      // Enable EEPROM write
    EECR |= (1 << EEPE);       // Start EEPROM write
}

uint8_t EEPROM_Read(uint16_t address) {
    while (EECR & (1 << EEPE)); // Wait for EEPROM to be ready
    EEAR = address;            // Set EEPROM address
    EECR |= (1 << EERE);       // Start EEPROM read
    return EEDR;               // Return data
}

uint8_t debounceSwitch() {
    static uint8_t state = 0xFF;
    state = (state << 1) | ((PIND & (1 << SWITCH_PIN)) >> SWITCH_PIN) | 0xE0;
    return (state == 0xF0); // Stable LOW
}

void SPI_Init() {
    DDRB |= (1 << PB4);             // MISO as output
    SPCR = (1 << SPE) | (1 << SPIE); // Enable SPI and SPI interrupt
    sei(); // Enable global interrupts
}

ISR(SPI_STC_vect) {
    uint8_t received = SPDR;
    if (received == 'C') {  // Master requests switch state
        uint8_t state = debounceSwitch();
        SPDR = state ? '1' : '0'; // Send switch state
    }
}

void setup() {
    DDRD &= ~(1 << SWITCH_PIN); // Set switch pin as input
    PORTD |= (1 << SWITCH_PIN); // Enable pull-up resistor
    SPI_Init(); // Initialize SPI
}

void loop() {
    static uint8_t lastSwitchState = 0;
    uint8_t currentState = debounceSwitch();

    if (currentState != lastSwitchState) {
        lastSwitchState = currentState;
        EEPROM_Write(0, currentState); // Store switch state
    }
    _delay_ms(10); // Sampling delay
}
