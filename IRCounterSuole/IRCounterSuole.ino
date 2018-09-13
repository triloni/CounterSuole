//#include <NewPing.h>

// #define TRIGGER_PIN  12
// #define ECHO_PIN     11
#define MAX_DISTANCE 30

#define LED          10                                                                                       // светодиод
#define BUTTON        9                                                                                       // кнопка

//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

float Work_Point_cm = 0.0;

float delta_cm = 0.45;

int Error = 0;

int counter = 0; // -1

unsigned long t_refresh_LED = 0;


bool flg1 = false; // test IR
// lcd

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// сетап
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    Serial.begin(9600);

    pinMode(LED, OUTPUT);
    pinMode(BUTTON, INPUT);

    Blink();                                                                                                  // мигаем

    lcd.begin(16, 2);
  // Print a message to the LCD

    lcd.setCursor(4, 0);
    lcd.print("READY...");

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

int LcdEnd() {
  for (int i = 0; i<3; i++) {
    lcd.setCursor(0, 0);
    lcd.print("0000000000000000");
    lcd.setCursor(0, 1);
    lcd.print("0000000000000000");
    delay(400);
    lcd.clear();
    delay(400);
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

	//ultrasonic sensor
    //float test_double_accuracy = 0.1*int((sonar.ping_median(20 , MAX_DISTANCE)+0.0000001)/57/0.1);



	// IR sensor

	float res = 0;
    for (int i = 0; i<50; i++) {
        delay(10);
        int sensor = 3;
        int k = analogRead(sensor); //
        //Serial.println(k);
        res = res + k;
    }
    res = round(res / 50);

	float volts = res*0.0048828125;  // value from sensor * (5/1024)
	float test_double_accuracy = 13*pow(volts, -1); // worked out from datasheet graph
  //delay(300); // slow down serial port 
  
  // if (distance <= 30){
  //   //Serial.println(distance);   // print the distance

  //   Serial.println(analogRead(2));
  // }

	if (test_double_accuracy > 30)
	{
		test_double_accuracy = 30;
	}

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
                    //lcd.clear();
                    lcd.setCursor(0, 2);
                    lcd.print(" Ready for calc ");
                } else {                                                                                      // если приклали руку
                    Work_Point_cm = -1.0;
                    //lcd.clear();
                    lcd.setCursor(0, 2);
                    lcd.print("   Waiting...   ");

                    //counter++;



//IR test
    
	if (analogRead(1) > 900 && analogRead(2) > 900)
	{
		Serial.println("Рука відсутня");
		flg1 = false;
	}
	else if (analogRead(1) < 900 || analogRead(2) < 900)
	{
		Serial.println("Рука");

		 while(flg1 == false)
		{
			if (analogRead(1) > 900 && analogRead(2) > 900)
				flg1 = true;
		}

		counter++; 

    //lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ammount: ");

    lcd.setCursor(10, 0);
    lcd.print("  /20");

    lcd.setCursor(10, 0);
    lcd.print(counter);

    

    
		Serial.print("counter++ ");
		Serial.print(counter);
		
		
	}
	// Serial.print("flg1 - ");
	// Serial.println(flg1);


// END                                                                                // инкримент счетчика

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
                        LcdEnd();                       
                        Serial.println(dist);
                    } while (dist < 29.5);  // 24.5                                                                    // если стопка пустая

Serial.println("wait end");                        

                    digitalWrite(LED, LOW);                                                                   // выключаем светодиод
                                        
                    delay(4000);                                                                              // пауза для забора стельки
                    
                    Blink(); // мигаем 
                    LcdEnd();

                    counter = 0;                                                                              // обнуляем счетчик                    
                    Work_Point_cm = AveradgeDist();                                                           // новая рабочая точка
                } else {
                    delay(1000);                                                                              // пауза для следующей стельки
                }                
            }
        } else {                                                                                              // если ошибка чтения
          Serial.println("ZERO");
//            Error = -1;
        }

        if (digitalRead(BUTTON) == HIGH) {                                                                    // если нажата кнопка во время цикла
            digitalWrite(LED, LOW);                                                                           // выключаем светодиод
            
            delay(4000);                                                                                      // пауза для забора стельки
            
            Blink();                                                                                          // мигаем   
            LcdEnd();

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
