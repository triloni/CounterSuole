#define MAX_DISTANCE 30
#define LED          10                                                                                       // светодиод
#define BUTTON        9                                                                                       // кнопка
#define BUTTON        9                                                                                       // кнопка

float Work_Point_cm = 0.0;

float delta_cm = 0.8; // 0.45 0.5 0.8

int Error = 0;

int counter = 0; // -1

unsigned int maxCount = 20;

int IRSensor = 3; // IR sensor pin

unsigned long t_refresh_LED = 0;

bool flg1 = false; // test IR

int SENSOR[2] =  {A1, A2};

#include <LiquidCrystal.h> // lcd


int preCounter = 0;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// сетап
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    //Serial.begin(9600);

    pinMode(LED, OUTPUT);
    pinMode(BUTTON, INPUT);
    pinMode(SENSOR[0], INPUT);
    pinMode(SENSOR[1], INPUT);

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

void LcdEnd() {
  for (int i = 0; i<3; i++) {
  	lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print("***** END! *****");
    lcd.setCursor(0, 1);
    lcd.print("****************");
    delay(400);
  }
}

void lcdCount(int counter = 0) {

	lcd.clear();

	lcd.setCursor(0, 0);
    lcd.print("Ammount: ");

    lcd.setCursor(10, 0);
    lcd.print("  /20");

    lcd.setCursor(10, 0);
    lcd.print(counter);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// считывает среднее 5 значений
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float AveradgeDist() {
	// IR sensor

	float res = 0;
    for (int i = 0; i<10; i++) {
        //delay(10);

        int k = analogRead(IRSensor); //
 
        res = res + k;
    }
    res = round(res / 10);

	float volts = res*0.0048828125;  // value from sensor * (5/1024)
	float test_double_accuracy = 13*pow(volts, -1); // worked out from datasheet graph

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

//    if (Error == 0) {                                                                                         // если нет ошибки

        //delay(50);

        float t_cm = AveradgeDist();                                                                          // считываем показания датчика

//		Serial.println(t_cm);
//        if (t_cm != 0.0) {                                                                                    // если показание "удачное"
          
            if (abs(t_cm - Work_Point_cm) > delta_cm) {                                                       // если большое отклонение

                if (Work_Point_cm == -1.0) 																// если забрали руку
                {                                                                  
                    Work_Point_cm = t_cm;                                                               // новая рабочая точка
                    //Serial.println("ready to measure");
                    lcd.setCursor(0, 2);
                    lcd.print(" Ready for calc ");
                } 
                else 
                {                                                                                      // если приклали руку
                    Work_Point_cm = -1.0;
                    lcd.setCursor(0, 2);
                    lcd.print("   Waiting...   ");
                    flg1 = false; // 

                    preCounter = counter; // якщо стелька не зафіксована то можливо дискретний датчик спрацював випадково і не потрібно чекати довго

					if ((digitalRead(SENSOR[0]) == LOW) || (digitalRead(SENSOR[1]) == LOW))
  					{
  						//Serial.println("Рука");
  
  						 while(flg1 == false)
  						{
  							if ((digitalRead(SENSOR[0]) == HIGH) && (digitalRead(SENSOR[1]) == HIGH))
  							{
  								delay(20); // затримка на скидання флагу (при проведенні руки інколи зникає контакт)
  								flg1 = true;
  							}

                  			//Serial.println("eeee");
  						}
  
  						counter++; 
  
  					    lcdCount(counter);
  					    
  						//Serial.print("counter++ ");
  						//Serial.print(counter);
  					}
  
  				// END                                                                                // инкримент счетчика
  
  					//Serial.print("dist: ");
  					//Serial.println(t_cm);

                    digitalWrite(LED, HIGH);                                                                  // мигаем
                    delay(100);
                    digitalWrite(LED, LOW);
                }

                if (counter >= maxCount ) {                                                                         // если стопка заполнена

                    digitalWrite(LED, HIGH);

                    LcdEnd(); // lcd end func

                    delay(1500);                                                                              // пауза для последней стельки
                    
                    float dist = 0.0;                                                                         // временная дистанция
                    do {                                                                                      // цикл ожидания "забирания" стопки стельки
                        delay(100);
                        dist = AveradgeDist();                                                                // считываем показания датчика
                        //Serial.print("wait ");                       
                        //Serial.println(dist);
                    } while (dist < 24.5);  // 24.5                                                                    // если стопка пустая

					          //Serial.println("wait end");                        

                    digitalWrite(LED, LOW);                                                                   // выключаем светодиод
                    
                                       
                    delay(4000);                                                                              // пауза для забора стельки
                    
                    Blink(); // мигаем 

                    counter = 0;                                                                              // обнуляем счетчик                    

                    lcdCount(0); // ready to work

                    Work_Point_cm = AveradgeDist();                                                           // новая рабочая точка
                } else {
                	// Serial.print("preCounter = "); 
                	// Serial.print(preCounter);
                	// Serial.print("counter = "); 
                	// Serial.println(counter);  
                	if(preCounter != counter) // якщо стелька не зафіксована то можливо дискретний датчик спрацював випадково тоді не потрібно чекати довго
                    {
                    	delay(400);                                                                              // пауза для следующей стельки
                    }
                    else
                    {
                    	delay(100);
                    }
                }                
            }
}



















//        } else {                                                                                              // если ошибка чтения
//          Serial.println("ZERO");
////            Error = -1;
//        }

//        if (digitalRead(BUTTON) == HIGH) {                                                                    // если нажата кнопка во время цикла
//            digitalWrite(LED, LOW);                                                                           // выключаем светодиод
//            
//            delay(4000);                                                                                      // пауза для забора стельки
//            
//            Blink();                                                                                          // мигаем   
//            LcdEnd();
//
//            counter = 0;  
//
//            //lcd.clear();                                                                                    // обнуляем счетчик     
//
//            lcdCount(counter);
//
//            Work_Point_cm = AveradgeDist();                                                                   // новая рабочая точка
//        }
//    } else {                                                                                                  // если ошибка
//
//        int state_LED = 0;
//        t_refresh_LED = millis();
//
//        do {                                                                                                  // цикл ожидания нажатия на кнопку
//           if (millis() - t_refresh_LED > 500) {
//              state_LED = 1 - state_LED;
//              digitalWrite(LED, state_LED);
//              t_refresh_LED = millis();
//           }
//            
//        } while (digitalRead(BUTTON) == LOW);                
//        
//        digitalWrite(LED, LOW);                                                                               // выключаем светодиод
//        
//        Error = 0;                                                                                            // сбрасываем ошибку
//        counter = 0;                                                                                          // обнуляем счетчик                    
//        Work_Point_cm = AveradgeDist();                                                                       // новая рабочая точка
//    }

