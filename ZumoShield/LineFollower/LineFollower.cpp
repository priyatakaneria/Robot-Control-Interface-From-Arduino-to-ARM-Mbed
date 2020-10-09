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

int sum = 0;
int countPosition = 0;


PololuBuzzer buzzer(PTA1);
BuzzerValues buzzing;

ZumoMotors motors(PTD0, PTC12, PTC4, PTC3);
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
// static BufferedSerial pc(USBTX, USBRX);

int lastError = 0;

// This is the maximum speed the motors will be allowed to turn.
// (400 lets the motors go at top speed; decrease to impose a speed limit)
const int MAX_SPEED = 1;


// #define NUM_SENSORS 6
// unsigned int sensorValues[NUM_SENSORS];

ZumoReflectanceSensorArray reflectanceSensors;
DigitalOut ledPin(LED, 1);

void setup()
{
  // uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
//   wait_us(500000); //??

  reflectanceSensors.init();
  button.waitForButton();

  ledPin.write(1);

  buzzing = buzzer.playNote(NOTE_G(4), 200, 15);
  buzzer.beep(buzzing.returnFrequency(), buzzing.returnDuration(), buzzing.returnVolume());

 wait_us(1000000);
 
 int i;
  for(i = 0; i < 80; i++)
  {
    if ((i > 10 && i <= 30) || (i > 50 && i <= 70))
      motors.setSpeeds(-0.5, 0.5);
    else
      motors.setSpeeds(0.5, -0.5);
    reflectanceSensors.calibrate();

    // Since our counter runs to 80, the total delay will be
    // 80*20 = 1600 ms.
    wait_us(20000);
  }
  motors.setSpeeds(0,0);

  ledPin.write(0);

  buzzing = buzzer.playNote(NOTE_G(4), 500, 15);
  buzzer.beep(buzzing.returnFrequency(), buzzing.returnDuration(), buzzing.returnVolume());
  
  button.waitForButton();
  for (int i = 0; i < 3; i++)
  {
    wait_us(1000000);
    buzzing = buzzer.playNote(NOTE_G(3), 200, 15);
    buzzer.beep(buzzing.returnFrequency(), buzzing.returnDuration(), buzzing.returnVolume());

  }
}

void loop()
{
    unsigned int sensors[6];

    int position = reflectanceSensors.readLine(sensors);
    // for (int i = 0; i < 6; i++){
    //     printf("Sensor %d: %d\n", i, sensors[i]);
    // }
    // printf("Position %d\n", position);
    // sum = sum + position;
    // countPosition++;

    // float avg = (sum/countPosition);
    // printf("Average: %f\n", avg);

    int error = position - 2500;
    // if (error > 1500){
    //     error = 2500;
    // }
    // else if (error < -1500){
    //     error = -2500;
    // }

    // printf("Error %d\n", error);
    int speedDifference = error / 4 + 6 * (error - lastError);
    // int speedDifference = error / 3 + 7 * (error - lastError);



    // int speedDifference = error / 4 + 6 * (error - lastError);
    
    // float speedDifference = error / 3.4 + 6.4 * (error - lastError);

    // float speedDifference = error / 3.4 + 6.38 * (error - lastError);




    // float speedDifference = error / 3.4 + 6.38 * (error - lastError);

    lastError = error;

    int m1Speed = MAX_SPEED + speedDifference;
    int m2Speed = MAX_SPEED - speedDifference;

    if (m1Speed < 0)
    m1Speed = 0;
    if (m2Speed < 0)
    m2Speed = 0;
    if (m1Speed > MAX_SPEED)
    m1Speed = MAX_SPEED;
    if (m2Speed > MAX_SPEED)
    m2Speed = MAX_SPEED;

    // if (m1Speed == 0 && m2Speed ==400){
    //    for(int i = 0; i<30; i++){
    //     motors.setSpeeds(m1Speed, m2Speed);
    //    }
    // }
    // else if (m1Speed == 400 && m2Speed == 0) {
    //    for(int i = 0; i<30; i++){
    //     motors.setSpeeds(m1Speed, m2Speed);
    //    }
    // }


    // printf("%d", m1Speed);
    // printf(" %d\n", m2Speed);
    motors.setSpeeds(m1Speed, m2Speed);

    // if (error == 2500 | error == -2500){
    //     wait_us(10000);
    // }
}

int main(){
    setup();
    while(1){
        loop();
    }
}
