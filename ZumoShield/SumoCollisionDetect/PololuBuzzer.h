#pragma once
#include "mbed.h"

#ifndef MBED_BEEP_H
#define MBED_BEEP_H

/*! \brief Specifies that the sequence of notes will play with no further action
 *  required by the user. */
#define PLAY_AUTOMATIC 0


/*! \brief Specified that the user will need to call `playCheck()` regularly. */
#define PLAY_CHECK     1

//                                             n
// Equal Tempered Scale is given by f  = f  * a
//                                   n    o
//
//  where f  is chosen as A above middle C (A4) at f  = 440 Hz
//         o                                        o
//  and a is given by the twelfth root of 2 (~1.059463094359)

/*! \anchor note_macros
 *
 * \name Note Macros
 * \a x specifies the octave of the note
 * @{
 */
#define NOTE_C(x)       ( 0 + (x)*12)
#define NOTE_C_SHARP(x) ( 1 + (x)*12)
#define NOTE_D_FLAT(x)  ( 1 + (x)*12)
#define NOTE_D(x)       ( 2 + (x)*12)
#define NOTE_D_SHARP(x) ( 3 + (x)*12)
#define NOTE_E_FLAT(x)  ( 3 + (x)*12)
#define NOTE_E(x)       ( 4 + (x)*12)
#define NOTE_F(x)       ( 5 + (x)*12)
#define NOTE_F_SHARP(x) ( 6 + (x)*12)
#define NOTE_G_FLAT(x)  ( 6 + (x)*12)
#define NOTE_G(x)       ( 7 + (x)*12)
#define NOTE_G_SHARP(x) ( 8 + (x)*12)
#define NOTE_A_FLAT(x)  ( 8 + (x)*12)
#define NOTE_A(x)       ( 9 + (x)*12)
#define NOTE_A_SHARP(x) (10 + (x)*12)
#define NOTE_B_FLAT(x)  (10 + (x)*12)
#define NOTE_B(x)       (11 + (x)*12)

/*! \brief silences buzzer for the note duration */
#define SILENT_NOTE   0xFF

/*! \brief frequency bit that indicates Hz/10<br>
 * e.g. \a frequency = `(445 | DIV_BY_10)` gives a frequency of 44.5 Hz
 */
#define DIV_BY_10     (1 << 15)
/*! @} */
 
namespace mbed {

class BuzzerValues{
    private:
        float freq;
        float dur;
        float vol;
    public:
    BuzzerValues(float frequency, float duration, float volume);
    BuzzerValues();
    void changeValue(float frequency, float duration, float volume);
    float returnFrequency();
    float returnDuration();
    float returnVolume();
};

/* Class: PololuBuzzer
 *  A class which uses pwm to controle a beeper to generate sounds.
 */
class PololuBuzzer {
private:
    PwmOut _pwm;
    Timeout toff;
    bool playing;

public:
 
/** Create a Beep object connected to the specified PwmOut pin
 *
 * @param pin PwmOut pin to connect to 
 */
    PololuBuzzer (PinName pin);
 
/** Beep with given frequency and duration.
 *
 * @param frequency - the frequency of the tone in Hz
 * @param time - the duration of the tone in seconds
 */
    void beep (float frequency, float time, float volume);
 
/** stop the beep instantaneous 
 * usually not used 
 */
    void nobeep();
    
    static BuzzerValues playFrequency(unsigned int freq, unsigned int duration,
                unsigned char volume);
    
    static BuzzerValues playNote(unsigned char note, unsigned int duration,
          unsigned char volume);
          
    static void play(const char *sequence);

    static void playFromProgramSpace(const char *sequence);

    static void playMode(unsigned char mode);

    static unsigned char playCheck();

    static unsigned char isPlaying();

    bool isPlaying2(int request);

    //static void stopPlaying();

  // initializes timer for buzzer control
//   static void init2();
   static void init();
 };
}
#endif
