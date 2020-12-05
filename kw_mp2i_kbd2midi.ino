#define NOT_MUTE 53

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(NOT_MUTE, OUTPUT);
  digitalWrite(NOT_MUTE, HIGH);

  for (int i = 3; i <= 10; i++) {
    pinMode(i, INPUT_PULLUP);
  }

  // Left "half" of keyboard (Bass).
  for (int i = 23; i <= 41; i += 2) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  // Right "half" of keyboard (Treble).
  for (int i = 22; i <= 44; i += 2) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
}

void loop() {
  Serial.print("B: ");
  for (int i = 0; i <= 9; i++) {
    int line = 23 + (2 * i);
    digitalWrite(line, LOW);

    Serial.print(i);
    Serial.print(") ");
    byte key_press_b = 0;
    for (byte j = 0; j <= 7; j++) {
      int raw = !digitalRead(3 + j);
      key_press_b += (raw << j);
    }
    digitalWrite(line, HIGH);
    
    Serial.print(key_press_b);
    Serial.print(" ");
  }

  Serial.print("   T: ");
  for (int i = 0; i <= 11; i++) {
    int line = 22 + (2 * i);
    digitalWrite(line, LOW);

    Serial.print(i);
    Serial.print(") ");
    byte key_press_t = 0;
    for (byte j = 0; j <= 7; j++) {
      int raw = !digitalRead(3 + j);
      key_press_t += (raw << j);
    }
    digitalWrite(line, HIGH);
    
    Serial.print(key_press_t);
    Serial.print(" ");
  }
  
  Serial.print("\n");
}
