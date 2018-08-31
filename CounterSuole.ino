#include <NewPing.h>

#define TRIGGER_PIN  12
#define ECHO_PIN     11
#define MAX_DISTANCE 30

#define LED          10                                                                                       // светодиод
#define BUTTON        9                                                                                       // кнопка

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

float Work_Point_cm = 0.0;

float delta_cm = 0.6;

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

    Blink();                                                                                                  // мигаем
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// мигаем - знак приветствия/конец цикла
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Blink() {
    for (int i = 0; i<3; i++) {
        digitalWrite(LED, HIGH);                                                                              // мигаем
        delay(70);
        digitalWrite(LED, LOW);
        delay(70);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// считывает среднее 5 значений
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float AveradgeDist() {
//    float res;
//    for (int i = 0; i<5; i++) {
//        delay(5);
//        int k = sonar.ping_cm();
//        res = res + k;
//    }
//    res = round(res / 5.0);
//    return res;
    float test_double_accuracy = 0.1*int((sonar.ping_median(10 , MAX_DISTANCE)+0.0000001)/57/0.1);
    return test_double_accuracy;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// главный цикл
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

    if (Error == 0) {                                                                                         // если нет ошибки

        delay(50);
        float t_cm = AveradgeDist();                                                                          // считываем показания датчика
Serial.println(t_cm);
        if (t_cm != 0.0) {                                                                                    // если показание "удачное"
          
            if (abs(t_cm - Work_Point_cm) > delta_cm) {                                                       // если большое отклонение

                if (Work_Point_cm == -1.0) {                                                                  // если забрали руку

                    Work_Point_cm = t_cm;                                                                     // новая рабочая точка
                    Serial.println("ready to measure");
                } else {                                                                                      // если приклали руку
                    Work_Point_cm = -1.0;

                    counter++;                                                                                // инкримент счетчика

Serial.print("counter++ ");
Serial.print(counter);
Serial.print("  dist ");
Serial.println(t_cm);
    
                    digitalWrite(LED, HIGH);                                                                  // мигаем
                    delay(100);
                    digitalWrite(LED, LOW);
                }

                if (counter >= 20 ) {                                                                         // если стопка заполнена

                    digitalWrite(LED, HIGH);

                    delay(1500);                                                                              // пауза для последней стельки
                    
                    float dist = 0.0;                                                                         // временная дистанция
                    do {                                                                                      // цикл ожидания "забирания" стопки стельки
                        delay(100);
                        dist = AveradgeDist();                                                                // считываем показания датчика
                        Serial.print("wait ");                        
                        Serial.println(dist);
                    } while (dist < 24.5);                                                                      // если стопка пустая

Serial.println("wait end");                        

                    digitalWrite(LED, LOW);                                                                   // выключаем светодиод
                                        
                    delay(4000);                                                                              // пауза для забора стельки
                    
                    Blink();                                                                                  // мигаем

                    counter = 0;                                                                              // обнуляем счетчик                    
                    Work_Point_cm = AveradgeDist();                                                           // новая рабочая точка
                } else {
                    delay(1000);                                                                              // пауза для следующей стельки
                }                
            }
//        } else {                                                                                              // если ошибка чтения
//            Error = -1;
        }

        if (digitalRead(BUTTON) == HIGH) {                                                                    // если нажата кнопка во время цикла
            digitalWrite(LED, LOW);                                                                           // выключаем светодиод
            
            delay(4000);                                                                                      // пауза для забора стельки
            
            Blink();                                                                                          // мигаем   

            counter = 0;                                                                                      // обнуляем счетчик                    
            Work_Point_cm = AveradgeDist();                                                                   // новая рабочая точка
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
        
        Error = 0;                                                                                            // сбрасываем ошибку
        counter = 0;                                                                                          // обнуляем счетчик                    
        Work_Point_cm = AveradgeDist();                                                                       // новая рабочая точка
    }
}
