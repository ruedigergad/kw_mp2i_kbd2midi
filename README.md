
Some time ago, I bought a used, partially out of order, Kurzweil Mark Pro TWOi digital/electrical piano.
After trying some repair attempts, it looked like the problems could not be repaired.

The main problem seemed to have been located in the Engine Board.
The Engine Board is, e.g., responsible for generating the sound based on the key presses etc.

As "plan B repair attempt", I decided to give it a try to replace the control logic and audio generation.
As control logic replacement, I use an Arduino Mega 2560.
This repository contains the code for the Arduino controller in [https://github.com/ruedigergad/kw_mp2i_kbd2midi/blob/main/kw_mp2i_kbd2midi.ino](https://github.com/ruedigergad/kw_mp2i_kbd2midi/blob/main/kw_mp2i_kbd2midi.ino).
For the audio generation, I use a Linux-based computer with QSynth.

The Arduino controller reads the keyboard keys and pedals and generates MIDI messages based on the presses etc.
The Linux computer receives the generated MIDI messages and generates audio via QSynth.
The audio signals are send back to the amplifier board in the piano.

I added some pictures in the repository that show some of the work progress.

![Disassembled Keyboard](https://github.com/ruedigergad/kw_mp2i_kbd2midi/blob/main/keyboard_disassembled_scaled.jpg?raw=true)

![Keyboard Internals Controller Replacement](https://github.com/ruedigergad/kw_mp2i_kbd2midi/blob/main/keyboard_internals_replaced_controller_scaled.jpg?raw=true)

![Keyboard Internals Controller Replacement Close-up](https://github.com/ruedigergad/kw_mp2i_kbd2midi/blob/main/keyboard_internals_replaced_controller_2_scaled.jpg?raw=true)


In addition, the Arduino controller also displays some output on the keyboard panel.
However, the displayed output is rather a toy example, at least for now.

I added a video in the repository that shows the keyboard panel in idle mode.
While playing, no "animation" is shown to avoid distracting the player.

![Keyboard Panel Idle Animation](https://github.com/ruedigergad/kw_mp2i_kbd2midi/blob/main/keyboard_panel_idle.gif?raw=true)
