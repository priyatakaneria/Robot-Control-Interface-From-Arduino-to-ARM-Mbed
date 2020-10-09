#include "PololuBuzzer.h"
#include "Pushbutton.h"
#include "ZumoMotors.h"
#include "ZumoReflectanceSensorArray.h"
#include "millis.h"
#include "mbed.h"
#include<string>

I2C i2c(I2C_SDA, I2C_SCL);
#define LED PTD1

// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  1500 // microseconds 1500

// these might need to be tuned for different motor types
#define REVERSE_SPEED     0.5 // 0 is stopped, 400 is full speed
#define TURN_SPEED        0.5
#define FORWARD_SPEED     0.5
#define REVERSE_DURATION  200000 // microseconds
#define TURN_DURATION     300000 // microseconds

PololuBuzzer buzzer(PTA1);
ZumoMotors motors(PTD0, PTC12, PTC4, PTC3);
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
// static BufferedSerial pc(USBTX, USBRX);

#define NUM_SENSORS 6
unsigned int sensorValues[NUM_SENSORS];

ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);
DigitalOut ledPin(LED, 1);
// Timer t;

void waitForButtonAndCountDown()
{
  ledPin.write(1);
  button.waitForButton();
  ledPin.write(0);

  BuzzerValues buzzing;
  
  // play audible countdown
  for (int i = 0; i < 3; i++)
  {
    wait_us(1000000);
    buzzing = buzzer.playNote(NOTE_G(3), 200, 15);
    buzzer.beep(buzzing.returnFrequency(), buzzing.returnDuration(), buzzing.returnVolume());

  }
  wait_us(1000000);
  buzzing = buzzer.playNote(NOTE_G(4), 500, 15);
  buzzer.beep(buzzing.returnFrequency(), buzzing.returnDuration(), buzzing.returnVolume());
  wait_us(1000000);
}

void setup()
{
  // uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
//   wait_us(500000); //?? we dont need this it's only for callibration

  ledPin.write(1);
  waitForButtonAndCountDown();
}

void loop()
{
  if (button.isPressed())
  {
    // if button is pressed, stop and wait for another press to go again
    motors.setSpeeds(0, 0);
    button.waitForRelease();
    waitForButtonAndCountDown();
  }

    sensors.read(sensorValues);

  if (sensorValues[0] < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    wait_us(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    wait_us(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (sensorValues[5] < QTR_THRESHOLD)
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    wait_us(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    wait_us(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
    // otherwise, go straight
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
}

int main(){
    setup();
    while(1){
        loop();
    }
}
