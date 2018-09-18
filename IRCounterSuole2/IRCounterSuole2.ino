#define MAX_DISTANCE 30
#define MAX_COUNT    20
#define LED          10
#define IR_SENSOR     3
#define DELTA_CM     0.5  // 0.45 0.5 0.8
#define distMeasurementsAmmount 10

#include <LiquidCrystal.h> // lcd

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int counter = 0;
int preCounter = 0;
int workPointCm = 0;

int pakageCounter = 0;
int availiableError = 0;


int Blink() {
  for (int i = 0; i<3; i++) {
    digitalWrite(LED, HIGH);
    delay(70);
    digitalWrite(LED, LOW);
    delay(70);
  }
}

void LcdEnd() {
  	lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("***** END! *****");
    lcd.setCursor(0, 1);
    lcd.print("****************");
}

void lcdCount(int counter = 0) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ammount: ");
  lcd.setCursor(10, 0);
  lcd.print("  /");
  lcd.setCursor(13, 0);
  lcd.print(MAX_COUNT);
  lcd.setCursor(10, 0);
  lcd.print(counter);
}

float IRAveradgeDist(int ammount = distMeasurementsAmmount) {
	float res = 0;
  for (int i = 0; i<ammount; i++) {
    //delay(10);
    int k = analogRead(IR_SENSOR);
    res = res + k;
  }
  res = round(res / ammount);
	float volts = res*0.0048828125;  // value from sensor * (5/1024)
	float test_double_accuracy = 13*pow(volts, -1); // worked out from datasheet graph
	if (test_double_accuracy > MAX_DISTANCE)
	{
		test_double_accuracy = MAX_DISTANCE;
	}
  return test_double_accuracy;
}

int activityCounter(int pakageSize = 10, int persentage = 30) {
  float intPercentage = 0;
  for(int i = 0; i < pakageSize; i++)
  {
    if(abs(IRAveradgeDist(1) - ::workPointCm) > DELTA_CM)
    {
      intPercentage++;

      if(i >= pakageSize)
      {
        if (::workPointCm == -1.0)                                // если забрали руку
        {                                                                  
            ::workPointCm = IRAveradgeDist();                     // новая рабочая точка
        } 
        else 
        {                                                       // если приклали руку
          ::workPointCm = -1.0;
        }
      }
    }
    else
    {
      ::workPointCm = IRAveradgeDist();
    }
  } 

  intPercentage*=100.0/pakageSize;

  // Serial.print("workPointCm: ");
  // Serial.println(::workPointCm);

  //delay(300);

  if(intPercentage >= persentage)
  {
    pakageCounter++;
    Serial.print("pakageCounter: ");
    Serial.println(pakageCounter);

    availiableError = 0;

    return intPercentage;
  }
  else 
  {
    if(availiableError < 10)
    {
      availiableError = 0;
      pakageCounter = 0;
      return 0;
    }
    else
    {
    availiableError++;
    return -1;
    }
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(LED, OUTPUT);

  Blink();                                                                                                  // мигаем

  lcd.begin(16, 2);
}

void loop() {


  //Serial.println(activityCounter(10));
  workPointCm = IRAveradgeDist();

  int test = activityCounter(10);
  if(test)
  {
    Serial.print("persentage: ");
    Serial.println(test);
  }





















  // if (counter >= MAX_COUNT) {                                                                         // если стопка заполнена

  //   digitalWrite(LED, HIGH);
  //   LcdEnd(); // lcd end func
  //   delay(1500);                                                                              // пауза для последней стельки
    
  //   float dist = 0.0;                                                                         // временная дистанция
  //   do {                                                                                      // цикл ожидания "забирания" стопки стельки
  //       delay(100);
  //       dist = IRAveradgeDist();                                                                // считываем показания датчика
  //   } while (dist < 24.5);  // 24.5                                                                    // если стопка пустая                     
  //   delay(4000);  
  //   digitalWrite(LED, LOW);
  //   Blink(); // мигаем 
  //   counter = 0;                                                                              // обнуляем счетчик                    
  //   lcdCount(); // 0
  //   workPointCm = IRAveradgeDist();                                                           // новая рабочая точка
  // }
  // else 
  // {

  //   float t_cm = IRAveradgeDist();                                                                          // считываем показания датчика

  //   if (abs(t_cm - workPointCm) > DELTA_CM) {                                                       // если большое отклонение
  //     if (workPointCm == -1.0)                                // если забрали руку
  //     {                                                                  
  //         workPointCm = t_cm;                                                               // новая рабочая точка
  //     } 
  //     else 
  //     {                                                                                      // если приклали руку
  //       workPointCm = -1.0;
  //       lcd.setCursor(0, 2);
  //       lcd.print("   Waiting...   ");

  //       // 111111111111111111111111111

  //       counter++; // инкримент счетчика
  //       lcdCount(counter);

  //       Serial.print("dist: ");
  //       Serial.println(t_cm);

  //       digitalWrite(LED, HIGH);
  //       delay(100);
  //       digitalWrite(LED, LOW);
  //     }           
  //   }
  //   else
  //   {
  //     workPointCm = IRAveradgeDist();
  //   }

  // }
}

                             // цикл ожидания нажатия на кнопку
//           if (millis() - t_refresh_LED > 500) {
//              state_LED = 1 - state_LED;
//              digitalWrite(LED, state_LED);
//              t_refresh_LED = millis();
