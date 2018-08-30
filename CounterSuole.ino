#include <NewPing.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 30 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define LED 10
#define BUTTON 9

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

int Work_Point_cm = 0;
int delta_cm = 3;

int Error = 0;

int counter = -1;

unsigned long t_refresh_LED = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// сетап
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    Serial.begin(9600);

    pinMode(LED, OUTPUT);
    pinMode(BUTTON, INPUT);

    Blink();                                                                                              // мигаем
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// мигаем - знак приветствия/конец цикла
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Blink() {
    for (int i = 0; i<3; i++) {
        digitalWrite(LED, HIGH);                                                                          // мигаем
        delay(70);
        digitalWrite(LED, LOW);
        delay(70);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// считывает среднее 5 значений
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int AveradgeDist() {
    float res;
    for (int i = 0; i<5; i++) {
        delay(5);
        int k = sonar.ping_cm();
        res = res + k;
    }
    res = round(res / 5.0);
    return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// главный цикл
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

    if (Error == 0) {                                                                                         // если нет ошибки
        delay(50);
        int t_cm = AveradgeDist();                                                                            // считываем показания датчика

Serial.println(t_cm);

        if (t_cm != 0) {                                                                                      // если показание "удачное"
            if (abs(t_cm - Work_Point_cm) > delta_cm) {                                                       // если большое отклонение

                if (Work_Point_cm == -1) {                                                                    // если забрали руку
                    Work_Point_cm = t_cm;                                                                     // новая рабочая точка
                    Serial.println("back to good value");
                } else {                                                                                      // если приклали руку
                    Work_Point_cm = -1;

                    counter++;                                                                                // инкримент счетчика

                    Serial.print("counter++ ");
Serial.println(counter);
    
                    digitalWrite(LED, HIGH);                                                                  // мигаем
                    delay(100);
                    digitalWrite(LED, LOW);
                }

                if (counter == 4 ) {                                                                          // если стопка заполнена

                    digitalWrite(LED, HIGH);

                    delay(1500);                                                                              // пауза для последней стельки
                    
                    int dist = 0;                                                                             // временная дистанция
                    do {                                                                                      // цикл ожидания "забирания" стопки стельки
                        delay(100);
                        dist = AveradgeDist();                                                                // считываем показания датчика
                        Serial.print("wait ");                        
                        Serial.println(dist);
                    } while (dist < 24);                                                                      // если стопка пустая

                    Serial.println("wait end");                        

                    digitalWrite(LED, LOW);                                                                   // выключаем светодиод
                    counter = 0;                                                                              // обнуляем счетчик                    
                    
                    delay(4000);                                                                              // пауза для забора стельки
                    
                    Blink();                                                                                  // мигаем
                } else {
                    delay(1000);                                                                              // пауза для следующей стельки
                }                
            }
        } else {                                                                                              // если ошибка чтения
            Error = -1;
        }
    } else {                                                                                                  // если ошибка

        int state_LED = 0;
        t_refresh_LED = millis();

        do {                                                                                                  // цикл ожидания нажатия на кнопку
           if (millis() - t_refresh_LED > 500) {
              state_LED = 1 - state_LED;
              digitalWrite(LED, state_LED);
              t_refresh_LED = millis();
           }
            
        } while (digitalRead(BUTTON) == LOW);                
        
        digitalWrite(LED, LOW);                                                                               // выключаем светодиод
        
        Error = 0;
        Work_Point_cm = AveradgeDist();                                                                       // новая рабочая точка
        counter = 0;                                                                                          // обнуляем счетчик                    
    }
}
