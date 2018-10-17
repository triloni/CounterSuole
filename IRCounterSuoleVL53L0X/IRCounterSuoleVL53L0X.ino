#include <Wire.h>
#include <VL53L0X.h>
#include <MedianFilter.h>

MedianFilter test(10, 0);
VL53L0X sensor;
float sum = 0;

float  sensorvalue = 0;


void setup()
{
  Serial.begin(115200);
  Wire.begin();

  sensor.init();
  sensor.setTimeout(500);

  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  sensor.setMeasurementTimingBudget(200000);
  sensor.startContinuous();

  sensor.setSignalRateLimit(0.1); // 0.25 MCPS

  // sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18); // 14 and 10 PCLKs
  //sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
}

void loop()
{
  

  int val = sensor.readRangeSingleMillimeters();
  test.in(val);
  val = test.out();
  //-Serial.print("val value = "); Serial.println(val);
  int prevVal = val;
  delay(5);
  sum = 0;
  float  arrayofreadings[5];
  for (int i = 0; i < 5; i++)
  {
    int val = sensor.readRangeSingleMillimeters();
    test.in(val);
    val = test.out();
    arrayofreadings[i] = val;
    if (sensor.timeoutOccurred()) {
      Serial.println(" TIMEOUT");
    }
    sum = sum + arrayofreadings[i];
      //-Serial.print("sensor value = ");Serial.print(val); //-Serial.print("     ");
    //-Serial.print("sum value = ");Serial.println(sum);
  }

  sensorvalue = sum  / 5;
  //sensorvalue = measure.RangeMilliMeter;
  //-Serial.print("sensor value = ");

  if(abs(prevVal - sensorvalue) <= 4) // відкидаємо випадкові похибки після зміни відстані
  {
    Serial.println(sensorvalue);
  }
  else
  {
    Serial.println("ERROR!");
  }

}