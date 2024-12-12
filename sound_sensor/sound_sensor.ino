#define SOUND_SENSOR_PIN A0
#define SOUND_THRESHOLD 2.5 // Voltage threshold

// SPI pins for Master
#define SS_LCD_PIN 10 // Slave Select for LCD Arduino
#define SS_LED_PIN 9  // Slave Select for LED Arduino
#define SS_SWITCH_PIN 8 // Slave Select for Switch Arduino

void setup() {
    Serial.begin(9600);
    Serial.println("Master: KY-038 Sensor Initialized");

    // SPI Master setup
    pinMode(SS_LCD_PIN, OUTPUT);
    pinMode(SS_LED_PIN, OUTPUT);
    pinMode(SS_SWITCH_PIN, OUTPUT);
    digitalWrite(SS_LCD_PIN, HIGH); // Deselect LCD Slave
    digitalWrite(SS_LED_PIN, HIGH); // Deselect LED Slave
    digitalWrite(SS_SWITCH_PIN, HIGH); // Deselect Switch Slave

    pinMode(11, OUTPUT); // MOSI
    pinMode(12, INPUT);  // MISO
    pinMode(13, OUTPUT); // SCK

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); // Enable SPI as Master
}

void sendToSlave(uint8_t slavePin, const char* data) {
    digitalWrite(slavePin, LOW); // Select slave
    for (int i = 0; data[i] != '\0'; i++) {
        SPDR = data[i]; // Load data into SPI Data Register
        while (!(SPSR & (1 << SPIF))); // Wait for transfer to complete
        Serial.print(data[i]); // Debugging
    }
    Serial.println(" -> Sent");
    digitalWrite(slavePin, HIGH); // Deselect slave
}

void sendToLCD(const char* data) {
    Serial.println(data);  // Send data to LCD Arduino via Serial
}

void loop() {
    // Read analog sensor value
    float analogValue = analogRead(SOUND_SENSOR_PIN) * (5.0 / 1023.0);
    Serial.print("Analog value: ");
    Serial.println(analogValue);

    // Send data to LCD Arduino
    char lcdData[16];
    dtostrf(analogValue, 6, 2, lcdData);
    sendToLCD(lcdData);

    // Send control signal to LED Arduino
    if (analogValue > SOUND_THRESHOLD) {
        sendToSlave(SS_LED_PIN, "O"); // "O" for LED ON
    // sendToSlave(SS_LCD_PIN, "G");
        Serial.println("Threshold reached: LED ON");
    } else {
        sendToSlave(SS_LED_PIN, "F"); // "F" for LED OFF
    // sendToSlave(SS_LCD_PIN, "H");
        Serial.println("Threshold not reached: LED OFF");
    }

    // Request switch state from Switch Arduino
    sendToSlave(SS_SWITCH_PIN, "C");
    _delay_ms(100); // Synchronizing delay
    if (SPDR == '1') {
        Serial.println("Switch Arduino: Switch is ON");
    } else if (SPDR == '0') {
        Serial.println("Switch Arduino: Switch is OFF");
    }

    delay(500); // Delay before next cycle
}
