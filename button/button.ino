void setup() {
    pinMode(12, INPUT_PULLUP);
    Serial.begin(9600);
}

void loop() {
    if (digitalRead(12) == LOW) {
        //The button is pressed!
        Serial.println("PRESSED");
        
 }

    delay(500);
}
