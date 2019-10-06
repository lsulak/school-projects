 /************************************************************|
  |**           Project to subject SEN 2016/2017            **|
  |**   Author: Ladislav Sulak  xsulak04@stud.fit.vutbr.cz  **|
  |**   File:   xsulak04.ino                                **|
  |**                                                       **|
  |**   Description:                                        **|
  |**     Collecting data from accelerometer(s) for         **|
  |**     determining the position of quadrocopter.         **|
  |**     Calculating changes in directionand speed.        **|
  |**                                                       **|
  |**     ####                                              **|
  |**                                                       **|
  |**     Sbírání dat z akcelerometru(ů), pro určení pozice **|
  |**     kvadrokoptéry. Pocitanie zmeny polohy a rychlosti.**|
  |**                                                       **|
  |**   This file handles ADXL345 and calculates all        **|
  |**   necessary values which are send to serial port.     **|
  |**                                                       **|
  |***********************************************************/

#include <Wire.h>
#include "mainArduino.h"
/*
  Registers and some other addresses, from datasheet:
  http://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf
*/
#define ADXL_MODULE     0x53

#define R_THRESH_TAP    0x1D

#define R_DUR           0x21
#define R_THRESH_ACT    0x24
#define R_THRESH_INACT  0x25
#define R_TIME_INACT    0x26
#define R_ACT_INACT_CTL 0x27

#define R_POWER_CTL     0x2D
#define R_INT_ENABLE    0x2E
#define R_INT_MAP       0x2F
#define R_DATA_FORMAT   0x31
#define R_DATAX0        0x32

// This can be anything between 0 and 1, explanation below, in computation.
#define NORMALIZING_FACTOR 0.7f  // higher than 0.5 - prev value won't have such affect.
#define CALIBRATION_SAMPLES 50
#define CONVERSION_TO_G_FACTOR 0.00390625f
#define DISCRETE_VALS_FOR_ENUM_AMOUNT 10   // calculate speed etc. from this amount of values which are time-related.
#define INTER_VAL_MEASUREMENT_TIME 0.05f   // in seconds

// Arduino pin connected to INT1 from the ADXL345.
#define INTERRUPTPIN 2

byte raw_values[6]; // directly from accelerator, ľ bytes for every axis
char output[512];   // buffer for data which are sent to serial port



// Previous values on each axis, already normalized, values in [g].
double prevNormG_X;
double prevNormG_Y;
double prevNormG_Z;

// Offsets are calculated once during start, then they are constant.
double xOffset;
double yOffset;
double zOffset;

double distance;
int iterationCnt;

GData movementChange;

void interruptsInit()
{
  /*
  // TODO. Taken from: 
  // http://www.varesano.net/blog/fabio/exploring-advanced-features-adxl345-accelerometer-single-double-tap-activity-inactivity-f
  pinMode(INTERRUPTPIN, INPUT);
  
  writeTo(DEVICE, R_INT_MAP, 0); // send all interrupts to ADXL345's INT1 pin
  writeTo(DEVICE, R_INT_ENABLE, 0x11011000)); // enable signle and double tap, activity, inactivity and free fall detection
    
  writeTo(DEVICE, R_DUR, 0x1F); // 625us/LSB
  writeTo(DEVICE, R_THRESH_TAP, 48); // 62.5mg/LSB  <==> 3000mg/62.5mg = 48 LSB as datasheet suggestion
  writeTo(DEVICE, R_TAP_AXES, B8(111)); // enable tap detection on x,y,z axes
  
  // inactivity configuration
  writeTo(DEVICE, R_TIME_INACT, 10); // 1s / LSB
  writeTo(DEVICE, R_THRESH_INACT, 3); // 62.5mg / LSB
  
  // activity configuration
  writeTo(DEVICE, R_THRESH_ACT, 8); // 62.5mg / LSB
  
  // activity and inctivity control
  writeTo(DEVICE, R_ACT_INACT_CTL, B8(11111111));
  */
}

// Reads num bytes starting from address register on device in to _buff array
void readFrom(byte address, int num, byte buff[])
{
  Wire.beginTransmission(ADXL_MODULE); // start transmission to device
  Wire.write(address);    // sends address to read from
  Wire.endTransmission(); // end transmission

  Wire.beginTransmission(ADXL_MODULE); // start transmission to device
  Wire.requestFrom(ADXL_MODULE, num);  // request 6 bytes from device Registers: DATAX0-DATAZ1

  int i = 0;
  while (Wire.available()) {  // device may send less than requested (abnormal)
    buff[i] = Wire.read();    // receive a byte
    i++;
  }

  Wire.endTransmission(); // end transmission
}

RawData readRawData()
{
  Wire.beginTransmission(ADXL_MODULE);
  Wire.write(R_DATAX0); // start reading data from there
  Wire.endTransmission();

  Wire.beginTransmission(ADXL_MODULE);
  Wire.requestFrom(ADXL_MODULE, 6); // 6 bytes from device

  int i = 0;
  while (Wire.available()) {
    raw_values[i] = Wire.read(); // receive 1 byte
    i++;
  }
  Wire.endTransmission();

  // Each axis consists of 2 bytes of data.
  RawData raw;
  raw.x = (int)(((int)raw_values[1]) << 8) | raw_values[0];
  raw.y = (int)(((int)raw_values[3]) << 8) | raw_values[2];
  raw.z = (int)(((int)raw_values[5]) << 8) | raw_values[4];

  return raw;
}

GData readAccelG(bool withOffset)
{
  RawData rawData = readRawData();

  double fXg = rawData.x * CONVERSION_TO_G_FACTOR;
  double fYg = rawData.y * CONVERSION_TO_G_FACTOR;
  double fZg = rawData.z * CONVERSION_TO_G_FACTOR;

  if (withOffset) {
    fXg += xOffset;
    fYg += yOffset;
    fZg += zOffset;
  }

  GData normG;

  // The current value is calculated from current (just read) one and previous (normalized) following:
  // If the NORMALIZING_FACTOR=0.5, the previous value is 1 and current is 5, result is basically amean, so 3.
  // If the NORMALIZING_FACTOR=1.0, we won't use previous value.
  // If the NORMALIZING_FACTOR=0.2 or below 0.5, we will consider previous value with a more 'weight' than the actual one.
  // We are using value higher than 0.5, so previous value won't have such a bit impact on a new (normalized) value.
  normG.x = double(fXg * NORMALIZING_FACTOR + (prevNormG_X * (1.0 - NORMALIZING_FACTOR)));
  prevNormG_X = normG.x;

  normG.y = double(fYg * NORMALIZING_FACTOR + (prevNormG_Y * (1.0 - NORMALIZING_FACTOR)));
  prevNormG_Y = normG.y;

  normG.z = double(fZg * NORMALIZING_FACTOR + (prevNormG_Z * (1.0 - NORMALIZING_FACTOR)));
  prevNormG_Z = normG.z;

  return normG;
}

void writeTo(byte address, byte value)
{
  Wire.beginTransmission(ADXL_MODULE); // start transmission to device
  Wire.write(address);                 // send register address
  Wire.write(value);                   // send value to write
  Wire.endTransmission();              // end transmission
}

void calibrateAccelerometer()
{
  double xt = 0, yt = 0, zt = 0;

  // '#' is flag that new line is starting -
  // we may start to read in the middle of line from the serial port 
  Serial.println("#Calibration...");
  delay(100);

  for (int i = 0; i < CALIBRATION_SAMPLES; i++)
  {
    GData accel = readAccelG(false);
    xt += accel.x;
    yt += accel.y;
    zt += accel.z;
    delay(100); // From datasheet, Offset Calibration - recommended is 0.1 s
  }

  Serial.println("#Axis offsets [mG]: ");
  Serial.print("#X: ");
  xOffset = (xt / float(CALIBRATION_SAMPLES));

  Serial.print(xOffset * 1000, 8); // 8 floating points places
  xOffset *= -1;
  //xOffset = xOffset < 0 ? (-1 * xOffset) : xOffset;

  Serial.print(" Y: ");
  yOffset = (yt / float(CALIBRATION_SAMPLES));
  Serial.print(yOffset * 1000, 8);
  yOffset *= -1;
  //yOffset = yOffset < 0 ? (-1 * yOffset) : yOffset;

  Serial.print(" Z: ");
  zOffset = (zt / float(CALIBRATION_SAMPLES));
  Serial.print(zOffset * 1000, 8);
  zOffset *= -1;

  Serial.println();
  Serial.println();
  delay(2500);
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  interruptsInit(); // TODO

  writeTo(R_POWER_CTL, 0x00);   // PCR inicialization
  writeTo(R_POWER_CTL, 0x10);   // PCR set standby (D4)
  writeTo(R_POWER_CTL, 0x08);   // PCR set measure mode (D3)

  writeTo(R_DATA_FORMAT, 0x0B); // 16g range in full resolution = 4 mG / LSB

  // Offsets clearing.
  prevNormG_X = 0.0f; xOffset = 0.0f;
  prevNormG_Y = 0.0f; yOffset = 0.0f;
  prevNormG_Z = 0.0f; zOffset = 0.0f;

  movementChange.x = 0.0f; movementChange.y = 0.0f; movementChange.z = 0.0f;
  iterationCnt = 0.0f;

  // Print calibration values.
  calibrateAccelerometer();
}

void loop()
{
  /*
   * About 10 calculations per second, which means 
   * Number_of_calculated_stats_per_sec = 10 / DISCRETE_VALS_FOR_ENUM_AMOUNT
   *
   * Speed is absolute value - calculated as the SQRT(vx^2 + vy^2 + vz^ 2), vx is the integral of G values by time.
   *
   * Distance is calculated from speed - just speed multiplied by time period:
   * distance = speed * 0.5s (because 0.5s = 0.05s for each iteration * 10 iterations - so 10 values).
   *
   * Motion vector is calculated as an integral of G values by time taken from each axis. 
   * Vector is therefore 3-dimensional. It is basically distance moved in each axis during the time period.
   */
  double actual_speed = 0.0f;
  
  GData gData = readAccelG(true);
  movementChange.x += gData.x * INTER_VAL_MEASUREMENT_TIME;
  movementChange.y += gData.y * INTER_VAL_MEASUREMENT_TIME;
  movementChange.z += gData.z * INTER_VAL_MEASUREMENT_TIME;
   
  iterationCnt++;
  
  if (iterationCnt == DISCRETE_VALS_FOR_ENUM_AMOUNT) {
    actual_speed = sqrt(
      movementChange.x * movementChange.x + 
      movementChange.y * movementChange.y + 
      movementChange.z * movementChange.z);
    
    // '#' is flag that new line is starting -
    // we may start to read in the middle of line from the serial port.
    if(actual_speed > 0.01) {  // Skip low values - no motion, just an interference.
      Serial.print("#Speed [m/s]: ");
      Serial.print(actual_speed);
      Serial.print(" | Distance [m]: ");
      Serial.print(actual_speed*INTER_VAL_MEASUREMENT_TIME*DISCRETE_VALS_FOR_ENUM_AMOUNT);
      Serial.print(" | Motion vector: (");
      Serial.print(movementChange.x*INTER_VAL_MEASUREMENT_TIME*DISCRETE_VALS_FOR_ENUM_AMOUNT);
      Serial.print(",");
      Serial.print(movementChange.y*INTER_VAL_MEASUREMENT_TIME*DISCRETE_VALS_FOR_ENUM_AMOUNT);
      Serial.print(",");
      Serial.print(movementChange.z*INTER_VAL_MEASUREMENT_TIME*DISCRETE_VALS_FOR_ENUM_AMOUNT);
      Serial.println(")");
    }
    movementChange.x = 0.0f; movementChange.y = 0.0f; movementChange.z = 0.0f;
    distance = 0.0f;
    actual_speed = 0.0f;
    iterationCnt = 0;
  }
  delay(1000*INTER_VAL_MEASUREMENT_TIME - 5); // do not spam the console. About 20 calc per second.
}


