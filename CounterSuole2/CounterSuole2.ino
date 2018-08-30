#include <NewPing.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 40 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define LED 10
#define BUTTON 9

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

int Work_Point_cm = 0;
int delta_cm = 3;

int Error = 0;

int counter = 0;

unsigned long t_refresh_LED = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// сетап
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    Serial.begin(9600);

    pinMode(LED, OUTPUT);
    pinMode(BUTTON, INPUT);

    for (int i = 0; i<3; i++) {
        digitalWrite(LED, HIGH);                                                                      // мигаем
        delay(100);
        digitalWrite(LED, LOW);
        delay(100);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// считывает среднее 5 значений
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int AveradgeDist() {
    float res;
    Serial.print("Value: ");
    for (int i = 0; i<5; i++) {
        delay(100);
        int k = sonar.ping_cm();
        res = res + k;

        Serial.print(k); Serial.print(" ");
        
    }
    res = round(res / 5.0);

    Serial.print(" srednee ");
    Serial.println(res);
    return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// главный цикл
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

    if (Error == 0) {                                                                                         // если нет ошибки
        delay(2000);
        int t_cm = AveradgeDist();                                                                            // считываем показания датчика

        if (t_cm != 0) {                                                                                      // если показание "удачное"
            if (abs(t_cm - Work_Point_cm) > delta_cm) {                                                       // если большое отклонение
Serial.println("Change value");              
                Work_Point_cm = t_cm;                                                                         // новая рабочая точка

                counter++;                                                                                    // инкримент счетчика

                digitalWrite(LED, HIGH);                                                                      // мигаем
                delay(100);
                digitalWrite(LED, LOW);

                if (counter == 4 ) {                                                                          // если стопка заполнена

Serial.println("counter = 4");
                  
                    digitalWrite(LED, HIGH);
                    
                    int dist = 0;                                                                             // временная дистанция
                    do {                                                                                      // цикл ожидания "забирания" стопки стельки
                        delay(100);
                        dist = AveradgeDist();                                                               // считываем показания датчика
Serial.println("wait");                        
                    } while (dist < 25);                                                                      // 

Serial.println("wait end");                        

                    digitalWrite(LED, LOW);


                    counter = -1;
                    Work_Point_cm = AveradgeDist();

                    delay(2000);
                    
                    
                } else {
                    delay(2500);                                                                              // пауза для следующей стельки
                }                
            }
        } else {                                                                                              // если ошибка чтения
            Error = -1;
        }
    } else {                                                                                                  // если ошибка

        int state_LED = 0;
        t_refresh_LED = millis();

        do {                                                                                            // цикл ожидания нажатия на кнопку
           if (millis() - t_refresh_LED > 500) {
              state_LED = 1 - state_LED;
              digitalWrite(LED, state_LED);
              Serial.println("Error");
              t_refresh_LED = millis();
           }
            
        } while (digitalRead(BUTTON) == LOW);                

        Error = 0;
        counter = -1;
        digitalWrite(LED, LOW);
    }
}
