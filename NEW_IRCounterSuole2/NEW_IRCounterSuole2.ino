#include <Wire.h>
#include <VL53L0X.h>
#include <MedianFilter.h>

VL53L0X sensor;

MedianFilter test(10, 0);
//#define HIGH_SPEED
#define HIGH_ACCURACY

#define MAX_DISTANCE 30
#define LED          13                                                                                     
#define BUTTON        9                                                            

int Work_Point_cm = 0;
int delta_mm = 4;
int Error = 0;
int counter = 0; // -1
unsigned int maxCount = 20;

void setup() {
	Serial.begin(9600);

	pinMode(LED, OUTPUT);
	//pinMode(BUTTON, INPUT);

	Wire.begin();
	sensor.init();
	sensor.setTimeout(500);

	#if defined LONG_RANGE
	  // lower the return signal rate limit (default is 0.25 MCPS)
	  sensor.setSignalRateLimit(0.1);
	  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
	  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
	  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
	#endif

	#if defined HIGH_SPEED
	  // reduce timing budget to 20 ms (default is about 33 ms)
	  sensor.setMeasurementTimingBudget(20000);
	#elif defined HIGH_ACCURACY
	  // increase timing budget to 200 ms
	  sensor.setMeasurementTimingBudget(100000); // 200000
	#endif
}


int IRDist() {
	int o,r = sensor.readRangeSingleMillimeters();
	test.in( r );
	o = test.out();

	Serial.print("getMin: ");
	Serial.println(test.getMin());
	Serial.print("getMax: ");
	Serial.println(test.getMax());
	if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
	return o;
}



void loop() {

    //delay(50);

    float t_cm = IRDist();                                                                          // считываем показания датчика

	Serial.println(t_cm);
          
	if (abs(t_cm - Work_Point_cm) > delta_mm) 
	{                                                 // если большое отклонение
	    if (Work_Point_cm == -1) 																// если забрали руку
	    {                                                      
	        Work_Point_cm = t_cm;                                                               // новая рабочая точка
	        Serial.println("ready to measure");
	    } 
	    else 
	    {                                                                                      // если приклали руку
	        Work_Point_cm = -1;
			Serial.println("   Waiting...   ");
			counter++; 
			Serial.print(counter);
			Serial.print("dist: ");
			Serial.println(t_cm);

	        digitalWrite(LED, HIGH);                                                                  // мигаем
	        delay(100);
	        digitalWrite(LED, LOW);
	    }

	    if (counter >= maxCount ) 
	    {                                                                         // если стопка заполнена
	        digitalWrite(LED, HIGH);
	        delay(1500);                                                                              // пауза для последней стельки
	        float dist = 0.0;                                                                         // временная дистанция
	        do {                                                                                      // цикл ожидания "забирания" стопки стельки
	            delay(100);
	            dist = IRDist();                                                                // считываем показания датчика
	        } while (dist < 24.5);  // 24.5                                                                    // если стопка пустая
			Serial.println("wait end");                        
	        digitalWrite(LED, LOW);                                                                   // выключаем светодиод                
	        delay(4000);                                                                              // пауза для забора стельки
	        counter = 0;                                                                              // обнуляем счетчик
	        Work_Point_cm = IRDist();                                                           // новая рабочая точка
	    }             
	}
}