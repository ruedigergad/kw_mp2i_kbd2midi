#define NOT_MUTE 2

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(NOT_MUTE, OUTPUT);
  digitalWrite(NOT_MUTE, HIGH);

  // Left "half" of keyboard (Bass).
  for (int i = 23; i <= 33; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  /*
  for (int i = 39; i <= 53; i += 2) {
    pinMode(i, INPUT);
  }
  */
}

void loop() {
  for (int i = 23; i <= 33; i++) {
    digitalWrite(i, HIGH);

    Serial.print(i);
    Serial.print(") B:");
    byte key_press_b = 0;
    for (byte j = 0; j <= 7; j++) {
      int raw = analogRead(54 + j);
      byte tmp = raw > 900 ? 1 : 0;
      key_press_b += (tmp << j);
    }
    Serial.print(key_press_b);
    
    Serial.print(" T:");
    byte key_press_t = 0;
    for (byte j = 0; j <= 7; j++) {
      int raw = analogRead(62 + j);
      byte tmp = raw > 900 ? 1 : 0;
      key_press_t += (tmp << j);
    }
    Serial.print(key_press_t);
    
    digitalWrite(i, LOW);

    Serial.print("   ");
  }
  Serial.print("\n");
}
