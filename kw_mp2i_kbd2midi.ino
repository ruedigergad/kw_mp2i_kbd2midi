#include <MIDI.h>

#define NOT_MUTE 53
#define PRINT false
#define NOTE_OFFSET 21
#define NOTE_OFFSET_TREBLE 61

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

/*
 * Due to problems with the MIDI connection, we use Serial to MIDI, for now:
 * https://github.com/cjbarnes18/ttymidi
 * 
 * ./ttymidi -s /dev/ttyACM1 -v
 * aconnect 131:0 128:0
 */

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

unsigned long notes_pre_press[128];
byte notes_playing[128];

void loop() {

  for (int i = 0; i <= 4; i++) {
    byte line_press_started = 23 + (2 * 2 * (i + 1));
    digitalWrite(line_press_started, LOW);
    
    for (byte j = 0; j <= 7; j++) {
      int raw = !digitalRead(3 + j);

      byte note_number = (j + 1) + (i * 8);
      byte note_value = note_number + NOTE_OFFSET;
      if (raw != 0) {
        if (notes_pre_press[note_number] == 0) {
          notes_pre_press[note_number] = millis();
        }
      } else {
        if (notes_pre_press[note_number] == 0) {
          notes_pre_press[note_number] = 0;
        }
      }
    }
    digitalWrite(line_press_started, HIGH);


    byte line_pressed_full = 23 + (2 * 2 * i);
    digitalWrite(line_pressed_full, LOW);

    for (byte j = 0; j <= 7; j++) {
      int raw = !digitalRead(3 + j);

      byte note_number = (j + 1) + (i * 8);
      byte note_value = note_number + NOTE_OFFSET;
      if (raw != 0) {
        if (notes_playing[note_number] == 0) {
          
          unsigned long now = millis();
          unsigned long press_time_delta = now - notes_pre_press[note_number];
          byte velocity = 127 - press_time_delta;
          if (velocity < 0 || velocity > 127) {
            velocity = 0;
          }
          
          if (! PRINT && velocity > 0) {
            MIDI.sendNoteOn(note_value, velocity, 1);
            notes_playing[note_number] = velocity;
          }
        }
      } else {
        if (notes_playing[note_number] != 0) {
          if (! PRINT) {
            byte velocity = notes_playing[note_number];
            MIDI.sendNoteOff(note_value, velocity, 1);
            notes_playing[note_number] = 0;
          }
        }
      }
    }
    digitalWrite(line_pressed_full, HIGH);
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
}
