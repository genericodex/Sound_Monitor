#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup (Adjust the address if needed, e.g., 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(9600); // Start communication via RX pin
    lcd.begin(16, 2);   // Initialize the LCD
    lcd.backlight();    // Turn on the LCD backlight
    lcd.clear();        // Clear any previous data on the LCD
    lcd.print("Waiting..."); // Initial message
}

void loop() {
    if (Serial.available() > 0) {
        String receivedMessage = Serial.readStringUntil('\n'); // Read the incoming data
        lcd.clear();        // Clear the LCD
        lcd.setCursor(0, 0);
        lcd.print(receivedMessage.substring(0, 16)); // Display first part of the message
        lcd.setCursor(0, 1);
        if (receivedMessage.length() > 16) {
            lcd.print(receivedMessage.substring(16, 32)); // Display second part if needed
        }
    }
}
