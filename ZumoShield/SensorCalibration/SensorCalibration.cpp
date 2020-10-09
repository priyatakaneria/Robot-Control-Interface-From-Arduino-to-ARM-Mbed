#include "mbed.h"
#include "ZumoReflectanceSensorArray.h"

ZumoReflectanceSensorArray reflectanceSensors;

#define LED PTD1
#define NUM_SENSORS 6
unsigned int sensorValues[NUM_SENSORS];
DigitalOut ledPin(LED, 1);
Timer t;

// main() runs in its own thread in the OS
void setup()
{

  reflectanceSensors.init();
  wait_us(500000);

  t.start();

  unsigned long startTime = t.read_ms();
  while(t.read_ms() - startTime < 10000)   // make the calibration take 10 seconds
  {
    reflectanceSensors.calibrate();
  }
  ledPin.write(0);  

  // To get raw sensor values instead, call:
  //reflectanceSensors.read(sensorValues);

  for (unsigned int i = 0; i < NUM_SENSORS; i++)
  {
    printf("%d", reflectanceSensors.calibratedMinimumOn[i]);
    printf(" ");
  }

  printf("\n");

  for (unsigned int i = 0; i < NUM_SENSORS; i++)
  {
    printf("%d", reflectanceSensors.calibratedMaximumOn[i]);
    printf(" ");
  }
  printf("\n");
  printf("\n");
  wait_us(1000000);
}

void loop(){
  unsigned int position = reflectanceSensors.readLine(sensorValues);

  for (unsigned long i = 0; i < NUM_SENSORS; i++)
  {
    printf("%d", sensorValues[i]);
    printf(" ");
  }
  printf("    \n");
  printf("%d", position);

  wait_us(250000);
}

int main(){
    setup();
    while(1){
        loop();
    }
}


