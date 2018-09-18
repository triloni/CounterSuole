#define MAX_DISTANCE 30
#define MAX_COUNT    20
#define LED          10
#define IR_SENSOR     3
#define DELTA_CM     0.5  // 0.45 0.5 0.8
#define distMeasurementsAmmount 20

#include <LiquidCrystal.h> // lcd

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int counter = 0;
int preCounter = 0;
int workPointCm = 0;

int pakageCounter = 0;
int availiableError = 0;
unsigned long int t_refresh_counter = 0;

bool state_LED = true;
float preIRAverageDist = 0.0;
float CurrentDist = 0.0;
int percentage = 0;
int iterator = 0;
int grade = 6;
int val_percentage = pow(2, (grade-1));




unsigned long int min_mil = 0;
unsigned long int max_mil = 0;


float IRAverageDist(int ammount = distMeasurementsAmmount) {
	float res = 0;
  for (int i = 0; i<ammount; i++) {
    delay(1);
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

int activityCounter(int pakageSize = 30, int persentage = 80) {

  if (millis() - t_refresh_counter > 10) {
    CurrentDist = IRAverageDist();

    if(percentage == 0)
    {
      percentage = 1;
    }

    t_refresh_counter = millis();
    if(abs(CurrentDist - preIRAverageDist) > DELTA_CM)
    {
      state_LED = 1 - state_LED;
      digitalWrite(LED, state_LED);
      preIRAverageDist = CurrentDist;
      return 2;
    } 
    iterator++;
  }

  if(iterator >= grade)
  {
    iterator = 0;
    return 0;
  }

  return 1;
}

void setup() {
  Serial.begin(9600);

  pinMode(LED, OUTPUT);                                                                                              // мигаем

  lcd.begin(16, 2);
}

void loop() {
  percentage *= activityCounter();
  
  // unsigned long int temp_mil = millis();

  // max_mil = temp_mil;
 if(percentage >= val_percentage)
  {
  //   if((temp_mil - max_mil) > 100) // end of movement
  //   {
  //     // Serial.print("min_mil: ");
  //     // Serial.println(min_mil);

  //     Serial.print("max_mil: ");
  //     Serial.println(max_mil);

  //     Serial.println("_____________");
  //   }
  //   else
  //   {
  //     //max_mil = temp_mil;
  //   }

    Serial.println("_____________");

  }
}