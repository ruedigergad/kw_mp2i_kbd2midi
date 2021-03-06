/*
 * Copyright 2020 Ruediger Gad
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// This is used with an Arduino Mega 2560.

#define PRINT false

#if not PRINT
// Use the following MIDI Library:
// https://github.com/FortySevenEffects/arduino_midi_library
// Tested with library version: 4.3.1
#include <MIDI.h>
#endif

#define IO_PANEL_SENSE 53
#define IO_PANEL_DRIVE 52
#define IO_PANEL_COLA 51
#define IO_PANEL_COLD 50
#define IO_PANEL_COLC 49
#define IO_PANEL_COLB 48
#define IO_PANEL_ROW7 47
#define IO_PANEL_ROW6 46
#define IO_PANEL_ROW5 45
#define IO_PANEL_ROW4 44
#define IO_PANEL_ROW3 43
#define IO_PANEL_ROW2 42
#define IO_PANEL_ROW1 41
#define IO_PANEL_ROW0 40

#define IO_PEDAL_SUSTAIN A0
#define IO_PEDAL_SOSTENUTO A4
#define IO_PEDAL_SOFT A8
#define IO_PEDAL_REF A13

#define IO_NOT_MUTE A14

#define NOTE_OFFSET 20
#define NOTE_OFFSET_TREBLE 60

#if not PRINT
/*
 * Keeping some leftovers of some experiments regarding different MIDI
 * configurations here. In the end, using "proper" MIDI did not work, so far.
 * Serial to MIDI is used as fallback solution. See also MIDI/Serial.begin below.
 */
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
#endif

int pedals[] = {IO_PEDAL_SUSTAIN, IO_PEDAL_SOSTENUTO, IO_PEDAL_SOFT};

void setup() {
#if not PRINT
  MIDI.begin(MIDI_CHANNEL_OMNI);
/*
 * Due to problems with the MIDI connection, we use Serial to MIDI, for now:
 * https://github.com/cjbarnes18/ttymidi
 * 
 * ./ttymidi -s /dev/ttyACM1 -v
 * aconnect 131:0 128:0
 */
  Serial.begin(115200);
#else
  Serial.begin(9600);
#endif
  
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(IO_NOT_MUTE, OUTPUT);
  digitalWrite(IO_NOT_MUTE, HIGH);

  /*
   * Comparing to the Mark Pro TWOi Service Manual, p.57 "Scanner Board 38869 Interface",
   * these are the pins:
   * Flat Ribbon Cable    Arduino
   * Bass       Treble    I/O Port
   * MK0    =   MK5    =  2
   * BR0    =   BR5    =  3
   * MK1    =   MK6    =  4
   * BR1    =   BR6    =  5
   * MK2    =   MK7    =  6
   * BR2    =   BR7    =  7
   * MK3    =   MK8    =  8
   * BR3    =   BR8    =  9
   * MK4    =   MK9    =  10
   * BR4    =   BR9    =  11
   *            MK10   =  12
   *            BR10   =  13
   */
  for (int i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  /*
   * Left "half" of keyboard (Bass).
   * 
   * Comparing to the Mark Pro TWOi Service Manual, p.57 "Scanner Board 38869 Interface",
   * these are the pins:
   * Flat Ribbon      Arduino
   * Cable Bass       I/O Port
   * BASS0/T0       = 22
   * BASS1/T1       = 24
   * BASS2/T2       = 26
   * BASS3/T3       = 28
   * BASS4/T4       = 30
   * BASS5/T5       = 32
   * BASS6/T6       = 34
   * BASS7/T7       = 36
   */
  for (int i = 22; i <= 36; i += 2) {
    pinMode(i, INPUT_PULLUP);
  }

  /*
   * Right "half" of keyboard (Treble).
   * 
   * Comparing to the Mark Pro TWOi Service Manual, p.57 "Scanner Board 38869 Interface",
   * these are the pins:
   * Flat Ribbon      Arduino
   * Cable Treble     I/O Port
   * PORT50/T0      = 23
   * PORT51/T1      = 25
   * PORT52/T2      = 27
   * PORT53/T3      = 29
   * PORT54/T4      = 31
   * PORT55/T5      = 33
   * PORT56/T6      = 35
   * PORT57/T7      = 37
   */
  for (int i = 23; i <= 37; i += 2) {
    pinMode(i, INPUT_PULLUP);
  }

  // Pedals
  pinMode(IO_PEDAL_REF, OUTPUT);
  digitalWrite(IO_PEDAL_REF, LOW);
  
  for (int i = 0; i <= 2; i++) {
    pinMode(pedals[i], INPUT_PULLUP);
    //digitalWrite(pedals[i], HIGH);
  }
  

  // Panel
  for (int i = IO_PANEL_ROW0; i <= IO_PANEL_SENSE; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

unsigned long notes_pre_press[128];
byte notes_playing[128];
byte half_tones[] = {0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1};
byte pedal_read_idx = 0;
byte pedals_state[3];
byte panel_write_idx = 0;

unsigned long time_last_note_played = 0;
unsigned long time_first_note_played = 0;
unsigned long seconds = 0;

void loop() {
  seconds = millis() / 1000;

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

            if (velocity > 0) {
#if not PRINT
              MIDI.sendNoteOn(note_value, velocity, 1);
#endif
              notes_playing[note_value] = velocity;
            }
          }
        } else {
          if (notes_playing[note_value] != 0) {
            byte velocity = notes_playing[note_value];
#if not PRINT
            MIDI.sendNoteOff(note_value, velocity, 1);
#endif
            notes_playing[note_value] = 0;
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
#if not PRINT
            MIDI.sendNoteOn(note_value, velocity, 1);
#endif
            notes_playing[note_value] = velocity;
          }
        }
      } else {
        if (notes_playing[note_value] != 0) {
          byte velocity = notes_playing[note_value];
#if not PRINT
          MIDI.sendNoteOff(note_value, velocity, 1);
#endif
          notes_playing[note_value] = 0;
          notes_pre_press[note_value] = 0;
        }
      }
    }

    digitalWrite(line_pressed_full, HIGH);
  }

  // Pedals
  int raw = !digitalRead(pedals[pedal_read_idx]);
  
  int x = pedal_read_idx < 2 ? 0 : 1;
  if (raw) {
    if (pedals_state[pedal_read_idx] == 0) {
#if not PRINT
      MIDI.sendControlChange((67 - pedal_read_idx) - x, 127, 1);
#endif
      pedals_state[pedal_read_idx] = 1;
    }
  } else {
    if (pedals_state[pedal_read_idx] != 0) {
#if not PRINT
      MIDI.sendControlChange((67 - pedal_read_idx) - x, 0, 1);
#endif
      pedals_state[pedal_read_idx] = 0;
    }
  }
  pedal_read_idx = pedal_read_idx == 2 ? 0 : pedal_read_idx + 1;

  // Panel
  if (time_last_note_played != seconds) {
    for (byte i = 0; i < 128; i++) {
      if (notes_playing[i] != 0) {
        time_last_note_played = seconds;
      }
    }
  }

  if ((seconds - time_last_note_played) < 60) {
    if (time_first_note_played == 0) time_first_note_played = seconds;
    
    display_playing_panel();
  } else {
    time_first_note_played = 0;
    
    display_idle_panel();
  }
}

void display_playing_panel() {
  if (panel_write_idx % 3 == 0) {
    for (int i = IO_PANEL_ROW0; i <= IO_PANEL_SENSE; i++) {
      digitalWrite(i, HIGH);
    }
  }
  panel_write_idx++;

  if ((seconds - time_first_note_played) < 10) {
    switch(panel_write_idx) {
      case 1:
        // G
        write_panel(0b1101, 0b00111101);
        break;
      case 4:
        // o
        write_panel(0b0010, 0b01011100);
        break;
      case 7:
        // !
        write_panel(0b0011, 0b10000110);
        break;
      case 10:
        write_panel(0b0000, 0b00000001);
        break;
      case 13:
        break;
      case 16:
        panel_write_idx = 0;
        break;
    }
  } else {
    switch(panel_write_idx) {
      case 1:
        // Eyes
        write_panel(0b1101, 0b00001001);
        break;
      case 4:
        write_panel(0b0010, 0b01000000);
        break;
      case 7:
        // Smile
        write_panel(0b0011, 0b00001111);
        break;
      case 10:
        write_panel(0b0000, 0b00000001);
        break;
      case 13:
        break;
      case 16:
        panel_write_idx = 0;
        break;
    }
  }
}

void display_idle_panel() {
  if (panel_write_idx % 3 == 0) {
    for (int i = IO_PANEL_ROW0; i <= IO_PANEL_SENSE; i++) {
      digitalWrite(i, HIGH);
    }
  }
  panel_write_idx++;
  byte chasing_light_offset = 0;
  switch(panel_write_idx) {
    case 1:
      if ((seconds % 11) > 9 && (seconds % 17) > 15) {
        // Wink both
        write_panel(0b1101, 0b00000110);
      } else if ((seconds % 11) > 9) {
        // Wink left
        write_panel(0b1101, 0b00000101);
      } else if ((seconds % 17) > 15) {
        // Wink right
        write_panel(0b1101, 0b00001010);
      } else {
        // Eyes
        write_panel(0b1101, 0b00001001);
      }
      break;
    case 4:
      write_panel(0b0010, 0b01000000);
      break;
    case 7:
      if ((seconds % 23) > 20) {
        // Tounge right
        write_panel(0b0011, 0b01110011);
      } else {
        // Smile
        write_panel(0b0011, 0b00001111);
      }
      break;
    case 10:
      write_panel(0b0000, 0b00000001);
      break;
    case 13:
      // Right upper row
      //write_panel(0b1001, 1 << chasing_light_offset);
      
      // Right lower row
      switch ((seconds / 30) % 4) {
        case 0:
          chasing_light_offset = (millis()/100) % 8;
          write_panel(0b0100, 1 << chasing_light_offset);
          break;
        case 1:
          chasing_light_offset = (millis()/100) % 8;
          write_panel(0b0100, 0b10000000 >> chasing_light_offset);
          break;
        // First half of animation with two dots.
        case 2:
          chasing_light_offset = (millis()/150) % 4;
          write_panel(0b0100, 1 << chasing_light_offset);
          break;
        case 3:
          chasing_light_offset = (millis()/150) % 4;
          write_panel(0b0100, 0b00001000 >> chasing_light_offset);
          break;
      }
      break;
    case 16:
      // Second half of animation with two dots.
      switch ((seconds / 30) % 4) {
        case 2:
          chasing_light_offset = (millis()/150) % 4;
          write_panel(0b0100, 0b10000000 >> chasing_light_offset);
          break;
        case 3:
          chasing_light_offset = (millis()/150) % 4;
          write_panel(0b0100, 0b00010000 << chasing_light_offset);
          break;
      }

      panel_write_idx = 0;
      break;
  }
}

void write_panel(byte address, byte data) {
  digitalWrite(IO_PANEL_DRIVE, HIGH);
  
  for (byte i = 0; i < 4; i++) {
    byte on = (address >> i) & 1;
    digitalWrite(IO_PANEL_COLA - i, on == 1 ? HIGH : LOW);
  }
  
  for (byte i = 0; i < 8; i++) {
    byte on = (data >> i) & 1;
    digitalWrite(IO_PANEL_ROW0 + i, on == 1 ? HIGH : LOW);
  }
  
  digitalWrite(IO_PANEL_DRIVE, LOW);
}
