//Wiring Guide
//Using Arduino Mega 2560
//LCD 20x4 I2C: SDA -> PIN 20; SCL -> PIN 21; 

//LIBRARIES
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Syergie.h"
#include <Arduino_FreeRTOS.h>
#include <croutine.h>
#include <event_groups.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOSVariant.h>
#include <list.h>
#include <mpu_wrappers.h>
#include <portable.h>
#include <portmacro.h>
#include <projdefs.h>
#include <queue.h>
#include <semphr.h>
#include <stack_macros.h>
#include <task.h>
#include <timers.h>

//Serial Comunication Transmit Delay
#define tdelay 1

//PIN DEFINITIONS
#define pwm_right   3
#define pwm_left    5
#define direct_valve_1_1 8
#define direct_valve_1_2 9

#define pin_position_out  A0
#define pinRPM_1          A1
#define pinRPM_2          A2
#define pin_level_tanki   A3

//DEFINITIONS FOR SERIAL COMMUNICATION ESP ID
#define rpm_engine_ID 'R'
#define rpm_propeller_ID 'P'
#define rpm_pump_ID 'M'
#define tangki_ID 'T'
#define steer_ID 'A'
#define speed_ID 'S'
#define Kp_ID 'P'
#define Ki_ID 'I'
#define Kd_ID 'D'

/*-----( Define LCD )-----*/
// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

// Define Tasks
void TaskRpmMeasurement( void *pvParameters );
void TaskSerial( void *pvParameters );
void TaskPID( void *pvParameters );
void TaskLCD( void *pvParameters );
void TaskRPM( void *pvParameters );

/* Fuel Tank Level, Steer, Depth, and PWM*/
int level_tangki = 0;
int steer = 0;
int depth = 0;

int pwm1;
int pwm2;

//Buffers for Serial Communication ESP
char st[10];

//Buffers for Displaying to LCD
char lcd_buffer0[21];
char lcd_buffer1[21];
char lcd_buffer2[21];
char lcd_buffer3[21];

/* RPM Measurement */
Proximity speed_1;
Proximity speed_2;
int rpm_1 = 0;
int rpm_2 = 0;
int Speed = 1500;

//PID CONSTANTS
double P = 2.5209;
double I = 0.0000293;
double D = 0.2353;

double Buff; // posisi baru
double Last = 0; // posisi lama
double PTerm;
double ITerm;
double DTerm;
double PIDTerm;
double Ts = 1;

double cepat = 0;
double position_out = 0 ;
double position_in = 500;
double Error; // perbedaan feedback dengan input
double Sum = 0; // hasil integral error

char buff[100];
int i=0,counter=0,idx=0;
//END OF PID CONSTANTS

void setPWMfrequency0(int freq){
   TCCR0B = TCCR0B & 0b11111000 | freq ;
}
void setPWMfrequency1(int freq){
   TCCR1B = TCCR1B & 0b11111000 | freq ;
}
void setPWMfrequency2(int freq){
   TCCR2B = TCCR2B & 0b11111000 | freq ;
}

// the setup function runs once when you press reset or power the board
void setup() {  
  speed_1.pinRPM(pinRPM_1);
  speed_2.pinRPM(pinRPM_2);
  
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for UART ESP module
  Serial2.begin(115200);
  Serial3.begin(115200);
  
  lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines, turn on backlight

  // ------- Quick 3 blinks of backlight  -------------
  for(int i = 0; i< 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight(); // finish with backlight on  

  //-------- Write characters on the display ------------------
  // NOTE: Cursor Position: Lines and Characters start at 0  
  lcd.setCursor(4,0); //Start at character 4 on line 0
  lcd.print("Salam KAPAL!");
  delay(1000);

  setPWMfrequency0(0x02);// timer 0 , 3.92KHz
  setPWMfrequency1(0x02);// timer 1 , 3.92KHz
  setPWMfrequency2(0x02);// timer 2 , 3.92KHz

  // Now set up tasks to run independently.
  xTaskCreate(
    TaskRpmMeasurement
    ,  (const portCHAR *) "RpmMeasurement"   // A name just for humans
    ,  1024  // This stack size can be checked & adjusted by reading the Stack Highwater, original value 128
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskSerial
    ,  (const portCHAR *) "SERIAL"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  xTaskCreate(
    TaskPID
    ,  (const portCHAR *) "PID"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
    
  xTaskCreate(
    TaskLCD
    ,  (const portCHAR *) "LCD"
    ,  1024  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
    
  xTaskCreate(
    TaskRPM
    ,  (const portCHAR *) "RPM"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop(){} // Empty. Things are done in Tasks. 

/*---------------------- Tasks ---------------------*/
void TaskRpmMeasurement(void *pvParameters)  // Task for RPM and Measurements
{
  (void) pvParameters;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  
  int countA, countB;
  for (;;) // A Task shall never return or exit.
  {
    countB = millis() - countA; countA = millis();
//    Serial.print(F("Time TaskRPM: "));Serial.println(countB);
    
    rpm_1 = speed_1.calcRPM();
    rpm_2 = speed_2.calcRPM();

    //Steer dan Depth masih dummy
    steer += 10;
    if (steer > 360) {
      steer = 0;
    }
    depth += 13;
    if (depth > 99) {
      depth = 0;
    }
    
    //Serial.print(F("RPM 1 : "));
    
    //Serial.print(F("RPM 2 : "));
    
    
    //Fuel Measurement
    level_tangki = map(analogRead(pin_level_tanki),0,1023,0,20);
    //Serial.print(F("Level Tangki : "));
    
    
    //Governor Control
    switch(Speed){
      case 1900: pwm1 = 200; pwm2 = 0; break; //Speed UP
      case 1500: pwm1 = 0; pwm2 = 0; break; //Steady
      case 1300: pwm1 = 0; pwm2 = 200; break; //Speed Down
    }
    Serial.print(F("Speed : "));
    
    Serial.print(F("PWM 1 : "));
    
    Serial.print(F("PWM 2 : "));
    
    analogWrite(pwm_right, pwm1);
    analogWrite(pwm_left, pwm2);
    
    vTaskDelayUntil( &xLastWakeTime, 10);
  }
}

void TaskSerial(void *pvParameters)  // Task Serial
{
  (void) pvParameters;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  
  int countC, countD;
  for (;;)
  {
    countD = millis() - countC; countC = millis();
//    Serial.print(F("Time TaskMQT: "));Serial.println(countD);
    
    rpm_1 = speed_1.calcRPM();
    rpm_2 = speed_2.calcRPM();

    if (Serial2.available()) {
      buff[i] = Serial2.read();
      if(buff[i]=='\n'){
          i=-1;
          Serial.print("Serial Message arrived ");
          Serial.print(buff);
          counter = 0;
          dtostrf(0, 100, 0, buff);
      }
      else if(buff[i]=='\t'){
          idx=0;
          dtostrf(0, 10, 0, st);
      }
      else if(buff[i]==speed_ID){
          Serial.print("Speed = ");
          Speed = atoi(st);
          Serial.println(Speed);
      }
      else if(buff[i]==steer_ID){
          Serial.print("Steer = ");
          Serial.println(atoi(st));
      }
      else if(buff[i]==Kp_ID){
          Serial.print("Kp = ");
          P = atof(st);
          Serial.println(P);
      }
      else if(buff[i]==Kd_ID){
          Serial.print("Kd = ");
          D  = atof(st);
          Serial.println(D);
      }
      else if(buff[i]==Ki_ID){
          Serial.print("Ki = ");
          I = atof(st);
          Serial.println(I);
      }
      st[idx]=buff[i];
      i++;
      idx++;
    }
    else{
      Serial.print("No Data Serial #");
      Serial.println(counter);
    }

    Serial3.print("\t");
    Serial3.print(rpm_1);Serial3.print(rpm_engine_ID);Serial3.print("\t");
    Serial3.print(rpm_2);Serial3.print(rpm_propeller_ID);Serial3.print("\t");
    Serial3.print(level_tangki);Serial3.print(tangki_ID);Serial3.print("\t");
    Serial3.print(analogRead(position_out));Serial3.print(rpm_engine_ID);Serial3.println("\t");

    counter++;
    
    vTaskDelayUntil( &xLastWakeTime, 10);
  }
}

void TaskPID(void *pvParameters)  // Task PID
{
  (void) pvParameters;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  
  int countE, countF;
  for (;;)
  {
    countF = millis() - countE; countE = millis();
//    Serial.print(F("Time TaskPID: "));Serial.println(countF);

    rpm_1 = speed_1.calcRPM();
    rpm_2 = speed_2.calcRPM();

    double position_out = analogRead(pin_position_out);
    // Perhitungan PID
    Error = position_in - position_out;
    Buff = position_out; // untuk mencari derivatif
    Sum = Sum + Error; // hasil integral dari error
    PTerm = Error*P; // Proporsional
    ITerm = Sum*I*Ts; // Integral
    DTerm = D*(Last - Buff)/Ts; // Derivatif
    PIDTerm = PTerm + DTerm; // total PID
  
    if (PIDTerm >= 255)
      PIDTerm = 255;
    if (PIDTerm <= -255)
      PIDTerm = -255;
  
    cepat = PIDTerm; // Hasil PID dijadikan data kecepatan
    //Serial.print("Position in :");
    
    //Serial.print("Position_out : ");
  
    // Jika nilai kecepatan (-), stir berputar ke kanan
    if (cepat < 0) {
      // putar kanan
      int reversePWM = -cepat;
      analogWrite(direct_valve_1_1, 0);
      analogWrite(direct_valve_1_2, reversePWM);
    }
    // Jika nilai kecepatan (+), stir berputar ke kanan
    else if (cepat >= 0) {
      // putar kiri
      int forwardPWM = cepat;
      analogWrite(direct_valve_1_1, forwardPWM);
      analogWrite(direct_valve_1_2, 0);
    }
    Last = position_out; // untuk mencari derivatif
    
    // Delay sebesar 11 tick ~ 198 ms
    vTaskDelayUntil( &xLastWakeTime, 11);
  }
}

void TaskLCD(void *pvParameters)  // Task LCD
{
  (void) pvParameters;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  
  int countG, countH;
  for (;;)
  {
    countH = millis() - countG; countG = millis();
//    Serial.print(F("Time TaskLCD: "));Serial.println(countH);

    rpm_1 = speed_1.calcRPM();
    rpm_2 = speed_2.calcRPM();

    sprintf((char*) lcd_buffer0, "RPM Engine: %4d    ", rpm_1);
    sprintf((char*) lcd_buffer1, "RPM Propeller: %4d ", rpm_2);
    sprintf((char*) lcd_buffer2, "Level Tangki: %2d L  ", level_tangki);
    sprintf((char*) lcd_buffer3, "Steer:%3d%c Depth:%2dm", steer, 223, depth);

    lcd.setCursor(0,0); //Start at character 0 on line 0
    lcd.print(lcd_buffer0);
    lcd.setCursor(0,1);
    lcd.print(lcd_buffer1);
    lcd.setCursor(0,2);
    lcd.print(lcd_buffer2);
    lcd.setCursor(0,3);
    lcd.print(lcd_buffer3);

    // Delay sebesar 28 tick, 1 tick ~ 18 ms
    vTaskDelayUntil( &xLastWakeTime, 28);//( 120 / portTICK_PERIOD_MS ) );
  }
}

void TaskRPM(void *pvParameters)  // Task khusus RPM
{
  (void) pvParameters;
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  
  int countG, countH;
  for (;;)
  {
    rpm_1 = speed_1.calcRPM();
    rpm_2 = speed_2.calcRPM();

    vTaskDelayUntil( &xLastWakeTime, 1);
  }
}
