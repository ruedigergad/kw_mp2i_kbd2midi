#include <MIDI.h>

#define NOT_MUTE 53
#define PRINT false
#define NOTE_OFFSET 20
#define NOTE_OFFSET_TREBLE 60

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

  for (int i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  // Left "half" of keyboard (Bass).
  for (int i = 22; i <= 36; i += 2) {
    pinMode(i, INPUT_PULLUP);
  }
  // Left "Right" of keyboard (Treble).
  for (int i = 23; i <= 37; i += 2) {
    pinMode(i, INPUT_PULLUP);
  }  
}

unsigned long notes_pre_press[128];
byte notes_playing[128];
byte half_tones[] = {0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1};

void loop() {

  for (int i = 0; i <= 5; i++) {
    byte line_press_started = 3 + (2 * i);
    digitalWrite(line_press_started, LOW);
    
    // Bass
    if (i <= 4) {  
      for (byte j = 0; j <= 7; j++) {
        int raw = !digitalRead(22 + (j * 2));
  
        byte note_number = (j + 1) + (i * 8);
        byte note_value = note_number + NOTE_OFFSET;
        if (raw != 0) {
          if (notes_pre_press[note_value] == 0) {
            notes_pre_press[note_value] = millis();
          }
        } else {
          if (notes_pre_press[note_value] != 0) {
            notes_pre_press[note_value] = 0;
          }
        }
      }
    }
    // Treble
    for (byte j = 0; j <= 7; j++) {
      int raw = !digitalRead(23 + (j * 2));

      byte note_number = (j + 1) + (i * 8);
      byte note_value = note_number + NOTE_OFFSET_TREBLE;
      if (raw != 0) {
        if (notes_pre_press[note_value] == 0) {
          notes_pre_press[note_value] = millis();
        }
      } else {
        if (notes_pre_press[note_value] != 0) {
          notes_pre_press[note_value] = 0;
        }
      }
    }
    
    digitalWrite(line_press_started, HIGH);
  
  
    byte line_pressed_full = 2 + (2 * i);
    digitalWrite(line_pressed_full, LOW);

    // Bass
    if (i <= 4) {
      for (byte j = 0; j <= 7; j++) {
        int raw = !digitalRead(22 + (j * 2));
  
        byte note_number = (j + 1) + (i * 8);
        byte note_value = note_number + NOTE_OFFSET;
        if (raw != 0) {
          if (notes_playing[note_value] == 0) {
            byte velocity = 0;
            if (notes_pre_press[note_value] != 0) {
              unsigned long now = millis();
              double press_time_delta = now - notes_pre_press[note_value];
              if (press_time_delta == 0) press_time_delta = 1;

              byte half_tone = half_tones[(note_number - 1) % 12];
              double x = 0;
              if (half_tone == 1) {
                x = 3 / press_time_delta;
              } else {
                x = 6 / press_time_delta;
              }
              
              long tmp_velocity = 127 * x;
              if (tmp_velocity < 0) tmp_velocity = 0;
              if (tmp_velocity > 127) tmp_velocity = 127;
              velocity = tmp_velocity;
            } else {
              velocity = 127;
            }
            
            if (! PRINT && velocity > 0) {
              MIDI.sendNoteOn(note_value, velocity, 1);
              notes_playing[note_value] = velocity;
            }
          }
        } else {
          if (notes_playing[note_value] != 0) {
            if (! PRINT) {
              byte velocity = notes_playing[note_value];
              MIDI.sendNoteOff(note_value, velocity, 1);
              notes_playing[note_value] = 0;
              notes_pre_press[note_value] = 0;
            }
          }
        }
      }
    }

    // Treble
    for (byte j = 0; j <= 7; j++) {
      int raw = !digitalRead(23 + (j * 2));

      byte note_number = (j + 1) + (i * 8);
      byte note_value = note_number + NOTE_OFFSET_TREBLE;
      if (raw != 0) {
        if (notes_playing[note_value] == 0) {
          byte velocity = 0;
          if (notes_pre_press[note_value] != 0) {
            unsigned long now = millis();
            double press_time_delta = now - notes_pre_press[note_value];
            if (press_time_delta == 0) press_time_delta = 1;
            
            byte half_tone = half_tones[(note_number + 39) % 12];
            double x = 0;
            if (half_tone == 1) {
              x = 3 / press_time_delta;
            } else {
              x = 6 / press_time_delta;
            }
              
            long tmp_velocity = 127 * x;
            if (tmp_velocity < 0) tmp_velocity = 0;
            if (tmp_velocity > 127) tmp_velocity = 127;
            velocity = tmp_velocity;
          } else {
            velocity = 127;
          }
          
          if (! PRINT && velocity > 0) {
            MIDI.sendNoteOn(note_value, velocity, 1);
            notes_playing[note_value] = velocity;
          }
        }
      } else {
        if (notes_playing[note_value] != 0) {
          if (! PRINT) {
            byte velocity = notes_playing[note_value];
            MIDI.sendNoteOff(note_value, velocity, 1);
            notes_playing[note_value] = 0;
            notes_pre_press[note_value] = 0;
          }
        }
      }
    }

    digitalWrite(line_pressed_full, HIGH);
  }
}
