#define NOT_MUTE 2

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(NOT_MUTE, OUTPUT);
  digitalWrite(NOT_MUTE, HIGH);

  // Left "half" of keyboard (Bass).
  for (int i = 23; i <= 41; i += 2) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  // Right "half" of keyboard (Treble).
  for (int i = 22; i <= 44; i += 2) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

void loop() {
  Serial.print("B: ");
  for (int i = 0; i <= 9; i++) {
    int line = 23 + (2 * i);
    digitalWrite(line, HIGH);

    Serial.print(i);
    Serial.print(") ");
    byte key_press_b = 0;
    for (byte j = 0; j <= 7; j++) {
      int raw = analogRead(54 + j);
      byte tmp = raw > 900 ? 1 : 0;
      key_press_b += (tmp << j);
    }
    digitalWrite(line, LOW);
    delay(3);
    
    Serial.print(key_press_b);
    Serial.print(" ");
  }

  Serial.print("    T: ");
  for (int i = 0; i <= 11; i++) {
    int line = 22 + (2 * i);
    digitalWrite(line, HIGH);

    Serial.print(i);
    Serial.print(") ");
    byte key_press_b = 0;
    for (byte j = 0; j <= 7; j++) {
      int raw = analogRead(54 + j);
      byte tmp = raw > 900 ? 1 : 0;
      key_press_b += (tmp << j);
    }
    digitalWrite(line, LOW);
    delay(3);
    
    Serial.print(key_press_b);
    Serial.print(" ");
  }
  
  Serial.print("\n");
}
