#include <MIDI.h>

#define PANEL_SENSE 53
#define PANEL_DRIVE 52
#define PANEL_COLA 51
#define PANEL_COLD 50
#define PANEL_COLC 49
#define PANEL_COLB 48
#define PANEL_ROW7 47
#define PANEL_ROW6 46
#define PANEL_ROW5 45
#define PANEL_ROW4 44
#define PANEL_ROW3 43
#define PANEL_ROW2 42
#define PANEL_ROW1 41
#define PANEL_ROW0 40

#define PEDAL_REF A13
#define NOT_MUTE A14
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

int pedals[] = {A0, A4, A8};

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

  // Pedals
  pinMode(PEDAL_REF, OUTPUT);
  digitalWrite(PEDAL_REF, HIGH);
  /*
  for (int i = 0; i <= 2; i++) {
    pinMode(pedals[i], OUTPUT);
    digitalWrite(pedals[i], HIGH);
  }
  */

  // Panel
  for (int i = PANEL_ROW0; i <= PANEL_SENSE; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  //digitalWrite(PANEL_DRIVE, HIGH);
  //digitalWrite(PANEL_COLA, HIGH);
  //digitalWrite(PANEL_COLB, HIGH);
  //digitalWrite(PANEL_COLC, HIGH);
  //digitalWrite(PANEL_COLD, HIGH);
  //digitalWrite(PANEL_ROW0, HIGH);
  //digitalWrite(PANEL_ROW1, HIGH);
  //digitalWrite(PANEL_ROW2, HIGH);
  //digitalWrite(PANEL_ROW3, HIGH);
  //digitalWrite(PANEL_ROW4, HIGH);
  //digitalWrite(PANEL_ROW5, HIGH);
  //digitalWrite(PANEL_ROW6, HIGH);
  //digitalWrite(PANEL_ROW7, HIGH);
  //digitalWrite(PANEL_DRIVE, HIGH);
  //digitalWrite(PANEL_DRIVE, LOW);
  
  //digitalWrite(PANEL_COLA, LOW);
  //digitalWrite(PANEL_COLB, LOW);
  //digitalWrite(PANEL_COLC, LOW);
  //digitalWrite(PANEL_COLD, LOW);
  //digitalWrite(PANEL_ROW1, LOW);
  //digitalWrite(PANEL_ROW2, LOW);
  
  //digitalWrite(PANEL_COLB, HIGH);
  //digitalWrite(PANEL_COLC, HIGH);
  //digitalWrite(PANEL_COLD, HIGH);
  //digitalWrite(PANEL_ROW0, HIGH);
  //digitalWrite(PANEL_ROW1, HIGH);
  //digitalWrite(PANEL_ROW2, HIGH);

  //digitalWrite(PANEL_COLA, HIGH);
  //digitalWrite(PANEL_COLB, HIGH);
  //digitalWrite(PANEL_COLC, HIGH);
  //digitalWrite(PANEL_COLD, HIGH);
  //digitalWrite(PANEL_ROW0, HIGH);
  //digitalWrite(PANEL_ROW1, HIGH);
  //digitalWrite(PANEL_ROW2, HIGH);
  //digitalWrite(PANEL_ROW3, HIGH);
  //digitalWrite(PANEL_ROW4, HIGH);
  //digitalWrite(PANEL_ROW5, HIGH);
  //digitalWrite(PANEL_ROW6, HIGH);
  //digitalWrite(PANEL_ROW7, HIGH);
  //digitalWrite(PANEL_DRIVE, HIGH);
}

unsigned long notes_pre_press[128];
byte notes_playing[128];
byte half_tones[] = {0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1};
byte pedal_read_idx = 0;
byte pedals_state[3];
byte panel_write_idx = 0;

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

  // Pedals
  int raw = analogRead(pedals[pedal_read_idx]);
  
  int x = pedal_read_idx < 2 ? 0 : 1;
  if (raw > 1020) {
    if (pedals_state[pedal_read_idx] == 0) {
      MIDI.sendControlChange((67 - pedal_read_idx) - x, 127, 1);
      pedals_state[pedal_read_idx] = 1;
    }
  } else {
    if (pedals_state[pedal_read_idx] != 0) {
      MIDI.sendControlChange((67 - pedal_read_idx) - x, 0, 1);
      pedals_state[pedal_read_idx] = 0;
    }
  }
  pedal_read_idx = pedal_read_idx == 2 ? 0 : pedal_read_idx + 1;

  // Panel
  if (panel_write_idx % 4 == 0) {
    for (int i = PANEL_ROW0; i <= PANEL_SENSE; i++) {
      digitalWrite(i, HIGH);
    }
  }
  panel_write_idx++;
  switch(panel_write_idx) {
   case 1:
      write_panel(0b1101, 0b00001001);
      break;
   case 5:
      write_panel(0b0010, 0b01000000);
      break;
   case 9:
      write_panel(0b0011, 0b00001111);
      break;
   case 13:
      write_panel(0b0000, 0b00000001);
      break;
   case 17:
      panel_write_idx = 0;
      break;
  }
}

void write_panel(byte address, byte data) {
  digitalWrite(PANEL_DRIVE, HIGH);
  
  for (byte i = 0; i < 4; i++) {
    byte on = (address >> i) & 1;
    digitalWrite(PANEL_COLA - i, on == 1 ? HIGH : LOW);
  }
  
  for (byte i = 0; i < 8; i++) {
    byte on = (data >> i) & 1;
    digitalWrite(PANEL_ROW0 + i, on == 1 ? HIGH : LOW);
  }
  digitalWrite(PANEL_DRIVE, LOW);
}
