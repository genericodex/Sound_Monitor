void setup() {
    Serial.begin(9600);  // Start communication via TX pin
    Serial.println("KY-038 Noise detection started");

    // Set up the analog input pin (A0) and digital input pin (Pin 3) as input
    // Using direct port manipulation, we set the respective bits for the input pins
    // For A0, it's automatically handled as an analog input, no need for pinMode
    // For Pin 3, we set the corresponding bit in the DDRD register for input mode
    DDRD &= ~(1 << PD3);  // Set PD3 (Pin 3) as input (clear bit)
    PORTD |= (1 << PD3);  // Enable the internal pull-up resistor on Pin 3

    ADMUX = (1 << REFS0);                // Use AVcc as reference voltage
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

float readAnalogValue(uint8_t pin) {
    // Select the input pin by setting the ADMUX register appropriately
    ADMUX = (ADMUX & 0xF0) | (pin & 0x0F);

    // Start ADC conversion
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete

    // Return analog value (scaled voltage)
    return ADC * (5.0 / 1023.0);
}

void loop() {
    float analog_value;
    uint8_t digital_value;

    // Read analog value from the sound sensor (convert it to voltage)
    analog_value = readAnalogValue(A0);

    // Read the digital value from Pin 3 (threshold detection)
    // Using direct register access to read the value of PD3 (Pin 3)
    digital_value = (PIND & (1 << PD3)) != 0;  // Check if Pin 3 is high or low

    // Send formatted data to the serial monitor
    Serial.print(analog_value, 4);  // Send analog value with 4 decimal places
    Serial.print(" V, T.hold: ");
    Serial.println(digital_value ? "Reached" : "Not Reached");

    delay(100);  // Wait for 100ms before sending again
}
