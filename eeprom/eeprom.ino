#include <EEPROM.h>

void setup() {
    Serial.begin(9600); // Start Serial communication
}

void loop() {
    if (Serial.available() > 0) {
        String message = Serial.readStringUntil('\n');

        // Log data into EEPROM
        for (int i = 0; i < message.length() && i < EEPROM.length(); i++) {
            EEPROM.write(i, message[i]);
        }

        // Indicate data logged
        Serial.println("Data logged to EEPROM.");
    }
}
