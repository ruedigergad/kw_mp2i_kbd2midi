#define NOT_MUTE 36

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(NOT_MUTE, OUTPUT);
  digitalWrite(NOT_MUTE, HIGH);

  // Left "half" of keyboard (Bass).
  for (int i = 23; i <= 31; i++) {
    pinMode(i, OUTPUT);
  }
  for (int i = 39; i <= 53; i += 2) {
    pinMode(i, INPUT);
  }
}

void loop() {
  /*
  digitalWrite(23, LOW);
  int x = digitalRead(39);
  if (x == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  digitalWrite(23, HIGH);

  delay(100);
  */
  
  for (int i = 23; i <= 31; i++) {
    digitalWrite(i, HIGH);
    //delay(1);
    byte key_press = 0;
    for (byte j = 0; j <= 7; j++) {
      byte tmp = digitalRead(39 + (j * 2));
      key_press += (tmp << j);
    }
    digitalWrite(i, LOW);

    Serial.print(i);
    Serial.print(" - ");
    Serial.print(key_press);
    Serial.print("\n");
  }
  Serial.print("\n");
  delay(1000);

  /*
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  */
}
