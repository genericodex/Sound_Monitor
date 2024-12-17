#include <avr/io.h>
#include <util/delay.h>

// LCD I2C Definitions (using direct registers for communication)
#define I2C_WRITE 0x27 // 7-bit I2C address for the LCD

// I2C Initialization
void I2C_Init() {
    TWSR = 0x00;              // Prescaler set to 1
    TWBR = 0x48;              // SCL frequency ~100kHz
    TWCR = (1 << TWEN);       // Enable TWI (I2C)
}

// Start I2C Communication
void I2C_Start() {
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT))); // Wait for transmission
}

// Send I2C Data
void I2C_Write(uint8_t data) {
    TWDR = data; // Load data
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT))); // Wait for transmission
}

// Stop I2C Communication
void I2C_Stop() {
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
}

// LCD Send Command
void LCD_Command(uint8_t cmd) {
    I2C_Start();
    I2C_Write(I2C_WRITE << 1);
    I2C_Write(cmd & 0xF0);    // High nibble
    I2C_Write((cmd << 4) & 0xF0); // Low nibble
    I2C_Stop();
    _delay_ms(2);
}

// LCD Send Data
void LCD_Data(char data) {
    I2C_Start();
    I2C_Write(I2C_WRITE << 1);
    I2C_Write((data & 0xF0) | 0x01); // High nibble with RS=1
    I2C_Write(((data << 4) & 0xF0) | 0x01); // Low nibble with RS=1
    I2C_Stop();
    _delay_ms(2);
}

// LCD Initialization
void LCD_Init() {
    _delay_ms(50);
    LCD_Command(0x30);
    LCD_Command(0x20);
    LCD_Command(0x28); // Function set: 4-bit, 2-line
    LCD_Command(0x0C); // Display ON, Cursor OFF
    LCD_Command(0x01); // Clear display
    _delay_ms(2);
}

void LCD_Print(const char* str) {
    while (*str) {
        LCD_Data(*str++);
    }
}

void setup() {
    // UART Initialization
    UBRR0H = 0;
    UBRR0L = 103;             // Baud rate 9600
    UCSR0B |= (1 << RXEN0);   // Enable UART receiver
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data

    I2C_Init();  // Initialize I2C
    LCD_Init();  // Initialize LCD
    LCD_Print("Waiting...");
}

void loop() {
    if (UCSR0A & (1 << RXC0)) {  // Check if data is received
        char receivedData[16];
        int index = 0;
        while (UCSR0A & (1 << RXC0)) {
            receivedData[index++] = UDR0;
        }
        receivedData[index] = '\0'; // Null-terminate the string

        LCD_Command(0x01);  // Clear Display
        LCD_Print("Voltage: ");
        LCD_Print(receivedData);
    }
}
