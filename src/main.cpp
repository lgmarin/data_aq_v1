#include <Arduino.h>
#include <SPI.h>

//Set the number of thermocouples and allocate the CS pins
const byte max6675_num = 2;
const int max6675_cs[max6675_num] = {4, 5}; //MAX675 pins: 4, 5, 6, 7, 8
//Pinout, extra devices
// Pin D9 (9) - push button
// Pin A5 - DHT header (DATA) 

//Define SPI settings
SPISettings max6675_spi(1000000, MSBFIRST, SPI_MODE1);

void setup() {
  //Initialize SPI and diable all max connected
  for (int i = 0; i < max6675_num ; i++)
  {
    pinMode(max6675_cs[i], OUTPUT);
    digitalWrite(max6675_cs[i], HIGH);
  }
  SPI.begin();
  Serial.begin(115200);
  // Serial.print("Basic configuration initialized");
  // Serial.println("\t");
}

//Read data from the max6675 through SPI bus
float max6675Read(int max6675_cs_pin){
  unsigned int data;

  SPI.beginTransaction(max6675_spi);
  digitalWrite(max6675_cs_pin, LOW);

  data = SPI.transfer16(0); //read a 16bit response

  digitalWrite(max6675_cs_pin, HIGH);
  SPI.endTransaction();
  
  if (data & 0x0004)
  {
    return NAN;
  } else {
    return (data >> 3) / 4; //remove first 3 digits and couple the max resolutio (0.25)
  }
}

//Create a moving averages smooth function
float movAverage(float value){
  const float mov_avg_alpha = 0.1;
  static float mov_avg = -100;

  if (mov_avg == -100)
  {
    mov_avg = value;
  }

  mov_avg = mov_avg_alpha * value + (1-mov_avg_alpha) * mov_avg;

  return mov_avg;
}

void loop() {

  float value[max6675_num];

  delay(250); //default = 220

  for (int i = 0; i < max6675_num; i++)
  {
    value[i] = max6675Read(max6675_cs[i]);
    //Serial.print(round(movAverage(value[i]))); remove for now
  }

  Serial.print(value[0]); //Just print normal data
  Serial.print(",");
  Serial.println(value[1]); //Just print normal data
}