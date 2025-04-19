const int soundSensor = A0;
const int greenLED = 8;
const int blueLED = 9;
const int redLED = 10;
const int buzzer = 11;

void setup() {
  Serial.begin(9600);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  int level = analogRead(soundSensor);
  Serial.println(level); // Send to ESP32

  // Control LEDs and Buzzer
  if (level < 25) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(blueLED, LOW);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);
  } else if (level >= 25 && level < 50) {
    digitalWrite(greenLED, LOW);
    digitalWrite(blueLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);
  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(blueLED, LOW);
    digitalWrite(redLED, HIGH);
    digitalWrite(buzzer, HIGH); // Buzz when noise is high
  }

  delay(500);
}
