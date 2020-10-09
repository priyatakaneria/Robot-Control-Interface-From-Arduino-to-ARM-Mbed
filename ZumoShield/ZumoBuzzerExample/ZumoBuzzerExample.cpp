/*
 * This example uses the ZumoBuzzer library to play a series of notes on
 * the buzzer.  It also uses Pushbutton library to allow the user to
 * stop/reset the melody with the user pushbutton.
 */

//#include <Wire.h>
#include "mbed.h" 
#include "ZumoShield.h"
I2C i2c(I2C_SDA , I2C_SCL );

#define MELODY_LENGTH 95

// These arrays take up a total of 285 bytes of RAM (out of a limit of 1k (ATmega168), 2k (ATmega328), or 2.5k(ATmega32U4))
unsigned char note[MELODY_LENGTH] =
{
  NOTE_E(5), SILENT_NOTE, NOTE_E(5), SILENT_NOTE, NOTE_E(5), SILENT_NOTE, NOTE_C(5), NOTE_E(5),
  NOTE_G(5), SILENT_NOTE, NOTE_G(4), SILENT_NOTE,

  NOTE_C(5), NOTE_G(4), SILENT_NOTE, NOTE_E(4), NOTE_A(4), NOTE_B(4), NOTE_B_FLAT(4), NOTE_A(4), NOTE_G(4),
  NOTE_E(5), NOTE_G(5), NOTE_A(5), NOTE_F(5), NOTE_G(5), SILENT_NOTE, NOTE_E(5), NOTE_C(5), NOTE_D(5), NOTE_B(4),

  NOTE_C(5), NOTE_G(4), SILENT_NOTE, NOTE_E(4), NOTE_A(4), NOTE_B(4), NOTE_B_FLAT(4), NOTE_A(4), NOTE_G(4),
  NOTE_E(5), NOTE_G(5), NOTE_A(5), NOTE_F(5), NOTE_G(5), SILENT_NOTE, NOTE_E(5), NOTE_C(5), NOTE_D(5), NOTE_B(4),

  SILENT_NOTE, NOTE_G(5), NOTE_F_SHARP(5), NOTE_F(5), NOTE_D_SHARP(5), NOTE_E(5), SILENT_NOTE,
  NOTE_G_SHARP(4), NOTE_A(4), NOTE_C(5), SILENT_NOTE, NOTE_A(4), NOTE_C(5), NOTE_D(5),

  SILENT_NOTE, NOTE_G(5), NOTE_F_SHARP(5), NOTE_F(5), NOTE_D_SHARP(5), NOTE_E(5), SILENT_NOTE,
  NOTE_C(6), SILENT_NOTE, NOTE_C(6), SILENT_NOTE, NOTE_C(6),

  SILENT_NOTE, NOTE_G(5), NOTE_F_SHARP(5), NOTE_F(5), NOTE_D_SHARP(5), NOTE_E(5), SILENT_NOTE,
  NOTE_G_SHARP(4), NOTE_A(4), NOTE_C(5), SILENT_NOTE, NOTE_A(4), NOTE_C(5), NOTE_D(5),

  SILENT_NOTE, NOTE_E_FLAT(5), SILENT_NOTE, NOTE_D(5), NOTE_C(5)
};

unsigned int duration[MELODY_LENGTH] =
{
  100, 25, 125, 125, 125, 125, 125, 250, 250, 250, 250, 250,

  375, 125, 250, 375, 250, 250, 125, 250, 167, 167, 167, 250, 125, 125,
  125, 250, 125, 125, 375,

  375, 125, 250, 375, 250, 250, 125, 250, 167, 167, 167, 250, 125, 125,
  125, 250, 125, 125, 375,

  250, 125, 125, 125, 250, 125, 125, 125, 125, 125, 125, 125, 125, 125,

  250, 125, 125, 125, 250, 125, 125, 200, 50, 100, 25, 500,

  250, 125, 125, 125, 250, 125, 125, 125, 125, 125, 125, 125, 125, 125,

  250, 250, 125, 375, 500
};

PololuBuzzer buzzer(PTA1);
Pushbutton button(ZUMO_BUTTON);
BuzzerValues buzzing;
unsigned char currentIdx;

void setup()                    // run once, when the sketch starts
{
  currentIdx = 0;
}

void loop()                     // run over and over again
{
  // if we haven't finished playing the song and
  // the buzzer is ready for the next note, play the next note
  if (currentIdx < MELODY_LENGTH)
    {
    // play note at max volume
    buzzing = buzzer.playNote(note[currentIdx], duration[currentIdx], 15);
    if (buzzing.returnFrequency() == 1000){
        wait_us(buzzing.returnDuration() * 1000);
    }
    else {
    buzzer.beep(buzzing.returnFrequency(), buzzing.returnDuration(), buzzing.returnVolume());
    buzzer.isPlaying2(1);
    bool isItPlaying = true;
    while (isItPlaying == true){
        isItPlaying = buzzer.isPlaying2(2);
        }
    }
    currentIdx++;
  }

  // Insert some other useful code here...
  // the melody will play normally while the rest of your code executes
  // as long as it executes quickly enough to keep from inserting delays
  // between the notes.

  // For example, let the user pushbutton function as a stop/reset melody button
  if (button.isPressed())
  {
    // buzzer.stopPlaying(); // silence the buzzer
    buzzer.nobeep();
    if (currentIdx < MELODY_LENGTH)
      currentIdx = MELODY_LENGTH;        // terminate the melody
    else
      currentIdx = 0;                    // restart the melody
    button.waitForRelease();  // wait here for the button to be released
  }
}

int main() {
    setup();
    while(1){
        loop();  
    }
}