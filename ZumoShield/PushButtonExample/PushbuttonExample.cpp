/*
 * This example uses the Pushbutton library to demonstrate three different
 * methods for detecting a button press and release. It blinks the yellow
 * user LED each time the Zumo user button is pressed and released. You can
 * still use this demo without a Zumo Shield if you connect a normally-open
 * momentary pushbutton between digital pin 12 and ground on your Arduino.
 */

//#include <Wire.h>

#include "mbed.h"
#include "millis.h"
#include "ZumoShield.h"

#define LED_PIN PTD1
I2C i2c(I2C_SDA , I2C_SCL ); 

/*
 * Create a Pushbutton object for pin 12 (the Zumo user pushbutton pin)
 * with default settings, which enables the internal pull-up on the pin
 * and interprets a HIGH pin value as the default (unpressed) state of the
 * button. (Optional arguments can be passed to the constructor to specify
 * other button types and connection methods; see the documentation for
 * details.)
 */
Pushbutton button(ZUMO_BUTTON);

//void setup()
//{
//  pinMode(LED_PIN, OUTPUT);
//}

void loop()
{
  /*
   * Method 1: Directly read the state of the button with the isPressed()
   * function. You must debounce the button yourself with this method.
   */
  do
  {
    while (!button.isPressed());  // wait for button to be pressed
    wait_us(10000);                    // debounce the button press
  }
  while (!button.isPressed());    // if button isn't still pressed, loop

  do
  {
    while (button.isPressed());   // wait for button to be released
    wait_us(10000);                    // debounce the button release
  }
  while (button.isPressed());     // if button isn't still released, loop

  // blink LED
  DigitalOut ledPin(LED_PIN);
  ledPin.write(1);
  wait_us(200000);
  ledPin.write(0);

//   /*
//    * Method 2: Use the waitForButton() function, which blocks and doesn't
//    * return until a button press and release are detected. This function
//    * takes care of button debouncing.
//    */
//   button.waitForButton();

//   // blink LED
//   ledPin.write(1);
//   wait_us(200000);
//   ledPin.write(0);

//   /*
//    * Method 3: Call the getSingleDebouncedRelease() function repeatedly
//    * in a loop, which returns true to report a single button release or false
//    * otherwise. This function takes care of button debouncing. If you have
//    * multiple buttons, you can call getSingleDebouncedRelease() or
//    * getSingleDebouncedPress() in a loop for all of them until one of them
//    * returns true.
//    */
//   while (1)
//   {
//     if (button.getSingleDebouncedRelease())
//     {
//       // blink LED
//       ledPin.write(1);
//       wait_us(200000);
//       ledPin.write(0);

//       break;
//     }
//   }
}

int main(){
    millisStart();
    while(1){
       loop();
    }
}
