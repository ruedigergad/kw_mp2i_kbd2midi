
Some time ago, I bought a used, partially out of order, Kurzweil Mark Pro TWOi digital/electrical piano.
After trying some repair attempts, it looked like the problems could not be repaired.

The main problem seemed to have been located in the Engine Board.
The Engine Board is, e.g., responsible for generating the sound based on the key presses etc.

As "plan B repair attempt", I decided to give it a try to replace the control logic and audio generation.
As control logic replacement, I use an Arduino Mega 2560.
This repository contains the code for the Arduino controller.
For the audio generation, I use a Linux-based computer with QSynth.

The Arduino controller reads the keyboard keys and pedals and generated MIDI messages based on the presses etc.
The Linux computer receives the generated MIDI messages and generates audio via QSynth.
The audio signals are send back to the amplifier board in the piano.

I added some pictures in the repository that show some of the work progress.

In addition, the Arduino controller also displays some output on the keyboard panel.
However, the displayed output is rather a toy example, at least for now.

I added a video in the repository that shows the keyboard panel in idle mode.
While playing, no "animation" is shown to avoid distracting the player.

