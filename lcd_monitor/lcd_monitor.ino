#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust the I2C address as needed

void setup() {
    Serial.begin(9600);  // Start communication
    lcd.begin(16, 2);    // Initialize LCD
    lcd.backlight();     // Turn on backlight
    lcd.print("Waiting...");
}

void loop() {
    if (Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');

        // Check if the data is meant for the LCD
        if (data.startsWith("LCD:")) {
            lcd.clear();
            lcd.print("Sound Level:");
            lcd.setCursor(0, 1);
            lcd.print(data.substring(4));  // Remove "LCD:" and display the value
        }
    }
}
