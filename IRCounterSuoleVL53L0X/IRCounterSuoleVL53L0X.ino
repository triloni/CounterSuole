#include <Wire.h>
#include <VL53L0X.h>
#include <MedianFilter.h>

#define MAX_DISTANCE 30
#define LED          10                                                                                       // светодиод
#define BUTTON        9                                                                                       // кнопка

MedianFilter test(10, 0);
VL53L0X sensor;
float sum = 0;

float sensorvalue = 0;


//
float WorkPointMm = -1.0;

float delta = 3.0; // 0.45 0.5 0.8

int counter = 0; // -1

unsigned int maxCount = 20;
//


int EndBlink() {
  for (int i = 0; i<3; i++) {
    digitalWrite(LED, HIGH);                                                                              // мигаем
    delay(70);
    digitalWrite(LED, LOW);
    delay(70);
  }
}

int Blink() {
  digitalWrite(LED, HIGH);                                                                              // мигаем
  delay(100);
  digitalWrite(LED, LOW);
}

float AveradgeDist() {
  int val = sensor.readRangeSingleMillimeters();
  test.in(val);
  val = test.out();

  //-val = constrain(val, 100, 320);    // limits range of sensor values to between 10 and 150 //-----

  //-Serial.print("val value = "); Serial.println(val);
  int prevVal = val;
  delay(5);
  sum = 0;
  float  arrayofreadings[2];
  for (int i = 0; i < 2; i++)
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

  sensorvalue = sum  / 2;
  //sensorvalue = measure.RangeMilliMeter;
  //-Serial.print("sensor value = ");

  for(int i = 0; i < 2; i++)
  {
    if(abs(prevVal - sensorvalue) <= 2) // відкидаємо випадкові похибки після зміни відстані
    {
      Serial.println(sensorvalue);
      return sensorvalue;
    }
    else
    {
      //Serial.println("ERROR!");
      for (int i = 0; i < 3; i++)
      {
        sensor.readRangeSingleMillimeters(); // after error we need 2 measurements before we get normal data
      }
    }
  }
  return -1;
}


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
  sensor.setMeasurementTimingBudget(50000); //200000
  sensor.startContinuous();

  sensor.setSignalRateLimit(0.15); // 0.25 MCPS //0.1

  // sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18); // 14 and 10 PCLKs
  //sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);


  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
}


void loop()
{
  
  float t_cm = AveradgeDist(); // считываем показания датчика
  if(AveradgeDist())
  {                                                                


  if (abs(t_cm - WorkPointMm) > delta) {   //abs(t_cm - WorkPointMm) > delta                                                    // если большое отклонение

    if (WorkPointMm == -1.0)                                // если забрали руку
    {                                                                  
        WorkPointMm = t_cm;                                                               // новая рабочая точка
    } 
    else 
    {                                                                                      // если приклали руку
      if(WorkPointMm - t_cm > delta) 
      {
        WorkPointMm = -1.0;
        counter++; 
        Serial.println("+1");
        Serial.print("Counter: ");
        Serial.println(counter);
        Blink();
      }
    }
    }

    if (counter >= maxCount) {                                                                         // если стопка заполнена

        digitalWrite(LED, HIGH);
        delay(1500);                                                                              // пауза для последней стельки
        float dist = 0.0;                                                                         // временная дистанция
        do {                                                                                      // цикл ожидания "забирания" стопки стельки
            delay(100);
            dist = AveradgeDist();                                                                // считываем показания датчика
        } while (dist < 240);  // 24.5                                                                    // если стопка пустая

        digitalWrite(LED, LOW);                                                                   // выключаем светодиод
        delay(4000);                                                                              // пауза для забора стельки
        
        for(int i = 0; i < 3; i++) // after distanse was changed we need 2 measurements before we get normal data
          AveradgeDist(); 

        EndBlink(); // мигаем 

        counter = 0;                                                                              // обнуляем счетчик                    

        WorkPointMm = AveradgeDist();                                                           // новая рабочая точка
    } 

  } // if first


} // loop END
