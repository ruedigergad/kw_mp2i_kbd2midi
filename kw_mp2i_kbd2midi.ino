#include <MIDI.h>

#define NOT_MUTE 53
#define PRINT false

/*
struct MySettings : public midi::DefaultSettings
{
   static const uint16_t SenderActiveSensingPeriodicity = 0;
   static const bool UseRunningStatus = false;
   static const bool UseSenderActiveSensing = false;
   static const long BaudRate = 31250;
};
*/
//MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial3, MIDI, MySettings);
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI);
MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  if (! PRINT) {
    MIDI.begin(MIDI_CHANNEL_OMNI);
    Serial.begin(115200);
  } else {
    Serial.begin(9600);
  }
  
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
  if (PRINT) {
    Serial.print("B: ");
  }
  for (int i = 0; i <= 9; i++) {
    int line = 23 + (2 * i);
    digitalWrite(line, LOW);

    if (PRINT) {
      Serial.print(i);
      Serial.print(") ");
    }
    byte key_press_b = 0;
    for (byte j = 0; j <= 7; j++) {
      int raw = !digitalRead(3 + j);
      key_press_b += (raw << j);
    }
    digitalWrite(line, HIGH);

    if (PRINT) {
      Serial.print(key_press_b);
    }
    
    if (! PRINT and key_press_b != 0) {
      MIDI.sendNoteOn(42, 127, 1);
      delay(10);
      MIDI.sendNoteOff(42, 127, 1);
      //delay(1000);
    }
    
    if (PRINT) {
      Serial.print(" ");
    }
  }

  if (PRINT) {
    Serial.print("   T: ");
  }
  for (int i = 0; i <= 11; i++) {
    int line = 22 + (2 * i);
    digitalWrite(line, LOW);

    if (PRINT) {
      Serial.print(i);
      Serial.print(") ");
    }
    byte key_press_t = 0;
    for (byte j = 0; j <= 7; j++) {
      int raw = !digitalRead(3 + j);
      key_press_t += (raw << j);
    }
    digitalWrite(line, HIGH);

    if (PRINT) {
      Serial.print(key_press_t);
      Serial.print(" ");
    }
  }
  
  if (PRINT) {
    Serial.print("\n");
  }
  delay(100);
}
