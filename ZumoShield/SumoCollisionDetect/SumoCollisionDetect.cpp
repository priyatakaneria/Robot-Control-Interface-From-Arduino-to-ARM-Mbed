#include "PololuBuzzer.h"
#include "Pushbutton.h"
#include "ZumoMotors.h"
#include "ZumoReflectanceSensorArray.h"
#include "millis.h"
#include "FXOS8700Q.h"
#include "sdCard.h"
#include "mbed.h"
#include<string>

I2C i2c(I2C_SDA, I2C_SCL);
#define LED PTD1
#define PI           3.14159265358979323846
// #define LOG_SERIAL // write log output to serial port

// Accelerometer Settings
#define RA_SIZE 3  // number of readings to include in running average of accelerometer readings
#define XY_ACCELERATION_THRESHOLD 2400  // for detection of contact (~16000 = magnitude of acceleration due to gravity)

// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  1500 // microseconds 1500

// these might need to be tuned for different motor types
#define REVERSE_SPEED     0.5 // 0 is stopped, 400 is full speed
#define TURN_SPEED        0.5
#define FORWARD_SPEED     0.5
#define SEARCH_SPEED      0.5
#define SUSTAINED_SPEED   1 // switches to SUSTAINED_SPEED from FULL_SPEED after FULL_SPEED_DURATION_LIMIT ms
#define FULL_SPEED        1
#define STOP_DURATION     100000 // ms
#define REVERSE_DURATION  200000 // microseconds
#define TURN_DURATION     300000 // microseconds

#define RIGHT 1
#define LEFT -1

enum ForwardSpeed { SearchSpeed, SustainedSpeed, FullSpeed };
ForwardSpeed _forwardSpeed;  // current forward speed setting
unsigned long full_speed_start_time;
#define FULL_SPEED_DURATION_LIMIT     250000  // ms


PololuBuzzer buzzer(PTA1);
ZumoMotors motors(PTD0, PTC12, PTC4, PTC3);
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
Timer timerI2C;
sdCard SD;
// static BufferedSerial pc(USBTX, USBRX);

unsigned long loop_start_time;
unsigned long last_turn_time;
unsigned long contact_made_time;
#define MIN_DELAY_AFTER_TURN         400000  // ms = min delay before detecting contact event
#define MIN_DELAY_BETWEEN_CONTACTS   1000000  // ms = min delay between detecting new contact event

#define NUM_SENSORS 6
unsigned int sensorValues[NUM_SENSORS];

ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);
DigitalOut ledPin(LED, 1);
// Timer t;


// RunningAverage class
// based on RunningAverage library for Arduino
// source:  https://playground.arduino.cc/Main/RunningAverage
template <typename T>
class RunningAverage
{
  public:
    RunningAverage(void);
    RunningAverage(int);
    ~RunningAverage();
    void clear();
    void addValue(T);
    T getAverage() const;
    void fillValue(T, int);
  protected:
    int _size;
    int _cnt;
    int _idx;
    T _sum;
    T * _ar;
    static T zero;
};

// Accelerometer Class -- extends the LSM303 Library to support reading and averaging the x-y acceleration
//   vectors from the onboard LSM303DLHC accelerometer/magnetometer
class Accelerometer : public FXOS8700QAccelerometer 
{
  typedef struct acc_data_xy
  {
    unsigned long timestamp;
    int x;
    int y;
    float dir;
  } acc_data_xy;

  public:
    Accelerometer(I2C &i2c, uint8_t addr) : FXOS8700QAccelerometer(i2c, addr), ra_x(RA_SIZE), ra_y(RA_SIZE) {};
    ~Accelerometer() {};
    // void enable(void);
    void getLogHeader(void);
    void readAcceleration(unsigned long timestamp);
    float len_xy() const;
    float dir_xy() const;
    int x_avg(void) const;
    int y_avg(void) const;
    long ss_xy_avg(void) const;
    float dir_xy_avg(void) const;
  private:
    acc_data_xy last;
    RunningAverage<int> ra_x;
    RunningAverage<int> ra_y;
};

// FXOS8700QAccelerometer acc(i2c, FXOS8700CQ_SLAVE_ADDR1);
Accelerometer acc(i2c, FXOS8700CQ_SLAVE_ADDR1);
bool in_contact;  // set when accelerometer detects contact with opposing robot

void setForwardSpeed(ForwardSpeed speed);

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

  in_contact = false;  // 1 if contact made; 0 if no contact or contact lost
  contact_made_time = 0;
  last_turn_time = (timerI2C.read_us());  // prevents false contact detection on initial acceleration
  _forwardSpeed = SearchSpeed;
  full_speed_start_time = 0;
}

template <typename T>
T RunningAverage<T>::zero = static_cast<T>(0);

template <typename T>
RunningAverage<T>::RunningAverage(int n)
{
  _size = n;
  _ar = (T*) malloc(_size * sizeof(T));
  clear();
}

template <typename T>
RunningAverage<T>::~RunningAverage()
{
  free(_ar);
}

// resets all counters
template <typename T>
void RunningAverage<T>::clear()
{
  _cnt = 0;
  _idx = 0;
  _sum = zero;
  for (int i = 0; i< _size; i++) _ar[i] = zero;  // needed to keep addValue simple
}

// adds a new value to the data-set
template <typename T>
void RunningAverage<T>::addValue(T f)
{
  _sum -= _ar[_idx];
  _ar[_idx] = f;
  _sum += _ar[_idx];
  _idx++;
  if (_idx == _size) _idx = 0;  // faster than %
  if (_cnt < _size) _cnt++;
}

// returns the average of the data-set added so far
template <typename T>
T RunningAverage<T>::getAverage() const
{
  if (_cnt == 0) return zero; // NaN ?  math.h
  return _sum / _cnt;
}

// fill the average with a value
// the param number determines how often value is added (weight)
// number should preferably be between 1 and size
template <typename T>
void RunningAverage<T>::fillValue(T value, int number)
{
  clear();
  for (int i = 0; i < number; i++)
  {
    addValue(value);
  }
}

void setForwardSpeed(ForwardSpeed speed)
{
  _forwardSpeed = speed;
  if (speed == FullSpeed) full_speed_start_time = loop_start_time;
}

float getForwardSpeed()
{
  float speed;
  switch (_forwardSpeed)
  {
    case FullSpeed:
      speed = FULL_SPEED;
    //   for(int i=0; i<3; i++){
    //   printf("Full Speed %f\n\n\n\n\n", speed);
    //   }
      break;
    case SustainedSpeed:
      speed = SUSTAINED_SPEED;
    //   for(int i=0; i<3; i++){
    //   printf("Sustained Speed %f\n\n\n\n\n", speed);
    //   }
      break;
    default:
      speed = SEARCH_SPEED;
    //   for(int i=0; i<3; i++){
    //   printf("Search Speed %f\n\n\n\n\n", speed);
    //   }
      break;
  }
//   for(int i=0; i<100; i++){
//       printf("Speed that should be returned: %f\n", speed);
//       }
  return speed;
}

// check for contact, but ignore readings immediately after turning or losing contact
bool check_for_contact()
{
  static long threshold_squared = (long) XY_ACCELERATION_THRESHOLD * (long) XY_ACCELERATION_THRESHOLD;
//   printf("%lu\n", acc.ss_xy_avg());
  return (acc.ss_xy_avg() >  threshold_squared) && \
    (loop_start_time - last_turn_time > MIN_DELAY_AFTER_TURN) && \
    (loop_start_time - contact_made_time > MIN_DELAY_BETWEEN_CONTACTS);
}

// sound horn and accelerate on contact -- fight or flight
void on_contact_made()
{
#ifdef LOG_SERIAL
  SD.writeToSDCard("contact made\n");
//   printf("contact made");
//   printf("\n");
#endif
  in_contact = true;
  contact_made_time = loop_start_time;
  setForwardSpeed(FullSpeed);
//   buzzer.playFromProgramSpace(sound_effect);
}

// reset forward speed
void on_contact_lost()
{
#ifdef LOG_SERIAL
  SD.writeToSDCard("contact lost\n");
//   printf("contact lost");
//   printf("\n");
#endif
  in_contact = false;
  setForwardSpeed(SearchSpeed);
}

// execute turn
// direction:  RIGHT or LEFT
// randomize: to improve searching
void turn(char direction, bool randomize)
{
#ifdef LOG_SERIAL
  SD.writeToSDCard("turning ...\n");
//   printf("turning ...");
//   printf("\n");
#endif

  // assume contact lost
  on_contact_lost();

  static unsigned int duration_increment = TURN_DURATION / 4;

  // motors.setSpeeds(0,0);
  // delay(STOP_DURATION);
  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  wait_us(REVERSE_DURATION);
  motors.setSpeeds(TURN_SPEED * direction, -TURN_SPEED * direction);
  wait_us(randomize ? TURN_DURATION + ((rand() % 8) - 2) * duration_increment : TURN_DURATION);
  float speed = getForwardSpeed();
  motors.setSpeeds(speed, speed);
  last_turn_time = (timerI2C.read_us());
}

void setup()
{
  // uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
//   wait_us(500000); //?? we dont need this it's only for callibration
  SD.mountFileSystem();

  timerI2C.start();

  // Initiate FXOS8700Q
  acc.enable();

#ifdef LOG_SERIAL
//   Serial.begin(9600);
  acc.getLogHeader();
#endif

  srand((unsigned int)timerI2C.read_us());

  // uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);

  ledPin.write(1);
  waitForButtonAndCountDown();
}

void loop()
{
  if (button.isPressed())
  {
    // if button is pressed, stop and wait for another press to go again
    motors.setSpeeds(0, 0);
    SD.closeAndUnmount();
    button.waitForRelease();
    waitForButtonAndCountDown();
  }

    loop_start_time = timerI2C.read_us();

    acc.readAcceleration(loop_start_time);


    sensors.read(sensorValues);

//   int i;
//   for (i = 0; i < NUM_SENSORS; i++) {
//       printf("%d\n", sensorValues[i]);
//   }

    if ((_forwardSpeed == FullSpeed) && (loop_start_time - full_speed_start_time > FULL_SPEED_DURATION_LIMIT))
  {
    //   for(int i=0; i<100; i++){
    //   printf("set fwd\n");
    // }
    setForwardSpeed(SustainedSpeed);
  }

  if (sensorValues[0] < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    turn(RIGHT, true);
  }
  else if (sensorValues[5] < QTR_THRESHOLD)
  {
    // if rightmost sensor detects line, reverse and turn to the left
    turn(LEFT, true);
  }
  else  // otherwise, go straight
  {
    if (check_for_contact()) on_contact_made();
    float speed = getForwardSpeed();
    motors.setSpeeds(speed, speed);
  }
}

// class Accelerometer -- member function definitions

// enable accelerometer only
// to enable both accelerometer and magnetometer, call enableDefault() instead
void Accelerometer::getLogHeader(void)
{
  SD.writeToSDCard("millis    x      y     len     dir  | len_avg  dir_avg  |  avg_len\n");
//   printf("millis    x      y     len     dir  | len_avg  dir_avg  |  avg_len");
//   printf("\n");
}

void Accelerometer::readAcceleration(unsigned long timestamp)
{
  int16_t x, y, z;

//   printf("%d\n", acc.getX(x));
//   printf("%d\n", acc.getY(y));

  a.x = acc.getX(x);
  a.y = acc.getY(y);
  a.z = acc.getZ(z);
//   readAcc();

// for(int i=0; i<100; i++){
//       printf("acc");
//     }

    // wait_us(1000000);


    // printf("%d\n", a.x);
    // printf("%d\n", a.y);
    
  if (a.x == last.x && a.y == last.y) return;

  last.timestamp = timestamp;
  last.x = a.x;
  last.y = a.y;

  ra_x.addValue(last.x);
  ra_y.addValue(last.y);

#ifdef LOG_SERIAL
 char array[10];
 sprintf(array, "%lu", last.timestamp);
 SD.writeToSDCard(array);
//  printf("%lu", last.timestamp);
 SD.writeToSDCard("  ");
//  printf("  ");
 sprintf(array, "%d", last.x);
 SD.writeToSDCard(array);
//  printf("%d", last.x);
 SD.writeToSDCard("  ");
//  printf("  ");
 sprintf(array, "%d", last.y);
 SD.writeToSDCard(array);
//  printf("%d", last.y);
 SD.writeToSDCard("  ");
//  printf("  ");
 sprintf(array, "%f", len_xy());
 SD.writeToSDCard(array);
//  printf("%f", len_xy());
 SD.writeToSDCard("  ");
//  printf("  ");
 sprintf(array, "%f", dir_xy());
 SD.writeToSDCard(array);
//  printf("%f", dir_xy());
 SD.writeToSDCard("  |  ");
//  printf("  |  ");
 sprintf(array, "%f", sqrt(static_cast<float>(ss_xy_avg())));
 SD.writeToSDCard(array);
//  printf("%f", sqrt(static_cast<float>(ss_xy_avg())));
 SD.writeToSDCard("  ");
//  printf("  ");
 sprintf(array, "%f", dir_xy_avg());
 SD.writeToSDCard(array);
//  printf("%f", dir_xy_avg());
 SD.writeToSDCard("\n");
//  printf("\n");
#endif
}

float Accelerometer::len_xy() const
{
  return sqrt((double)last.x*a.x + last.y*a.y);
}

float Accelerometer::dir_xy() const
{
  return atan2((double)last.x, last.y) * 180.0 / PI;
}

int Accelerometer::x_avg(void) const
{
  return ra_x.getAverage();
}

int Accelerometer::y_avg(void) const
{
  return ra_y.getAverage();
}

long Accelerometer::ss_xy_avg(void) const
{
  long x_avg_long = static_cast<long>(x_avg());
  long y_avg_long = static_cast<long>(y_avg());
  return x_avg_long*x_avg_long + y_avg_long*y_avg_long;
}

float Accelerometer::dir_xy_avg(void) const
{
  return atan2(static_cast<float>(x_avg()), static_cast<float>(y_avg())) * 180.0 / PI;
}

int main(){
    setup();
    while(1){
        loop();
    }
}
