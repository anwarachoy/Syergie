/* LCD Library */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
char lcd_buffer0[21];
char lcd_buffer1[21];
char lcd_buffer2[21];
char lcd_buffer3[21];

/* Ethernet Library */
#include <SPI.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(10, 48, 20, 200);  
IPAddress server(10, 48, 20, 70);

// Update these with values suitable for your network.
const char* mqttClient = "ESP8266Client1";

// MQTT Topic
const char* statusPublish = "1/Status";
const char* speedPublish = "1/PropellerSpeed";
const char* positionPublish = "1/Azimuth";
const char* enginePublish = "1/EngineSpeed";
const char* pumpPublish = "1/PumpSpeed";
const char* fuelPublish = "1/Fuel";
const char* responSetPointPositionPublish = "1/Azimuth/setpoint/respon";
const char* responSetPointSpeedPublish = "1/PropellerSpeed/setpoint/respon";
const char* setPointPositionSubscribe = "1/Azimuth/setpoint";
const char* setPointSpeedSubscribe = "1/PropellerSpeed/setpoint";
const char* modeSubscribe = "Mode";
const char* mainSubscribe = "MainControl";

EthernetClient ethClient;
PubSubClient client(ethClient);
char buf[10];

/* Control Pin */
#define flow_control_pin 9
#define right_azimuth_pin 11
#define left_azimuth_pin 12

/* LED PIN */
#define GreenLED_pin  32
#define RedLED_pin  34

/* Speed Control Variable */
#define interruptPin 2 //PIN PEMBACAAN RPM
#define maxPWMspeed 250
boolean stringComplete = false;  // whether the string is complete
boolean motor_start = false;
boolean m_direction = true;
boolean m_direction_pre = true;
boolean enabledegree = true;
boolean enablerpm = false;
int count_rpm0;
int tdelay = 10;
int set_speed_last = 0;
const int tickmark = 1;
const int rpm_max = 2500;
unsigned long prevmillis = 0; // To store time
unsigned long duration = 0; // To store time difference

double rpm;
int m_speed = 0;
int m_speed_last = 0;
int set_speed = 0;
int set_position = 0;

// PID Control Variable
double kp_speed = 0.00007936;
double ki_speed = 0.0007936;
double kd_speed = 0;
double kp_position = 0.00007936;
double ki_position = 0.0007936;
double kd_position = 0;

/* Engine Sensor Pin */
#define fuel_pin A1

/* Button Pin */
#define button1_pin 4
#define button2_pin 5
#define button3_pin 6
#define button4_pin 3

/* Serial Event Variable */
String inputString1 = "";         // a String to hold incoming data
String inputString2 = "";         // a String to hold incoming data
String inputString3 = "";         // a String to hold incoming data

/* RPM Parameter */
int propeller_speed, azimuth_position, fuel_level, mode;
String pump_speed, engine_speed;

void setup() {
    // put your setup code here, to run once:
    pinMode(interruptPin,INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), RPM, CHANGE);
    
    Serial.begin(115200);
    Serial1.begin(115200);
    Serial2.begin(115200);
    Serial3.begin(115200);
  
    inputString1.reserve(200);
    inputString2.reserve(200);
    inputString3.reserve(200);
  
    pinMode(flow_control_pin,OUTPUT);
    
    pinMode(GreenLED_pin,OUTPUT);
    pinMode(RedLED_pin,OUTPUT);
    digitalWrite(GreenLED_pin,LOW);
    digitalWrite(RedLED_pin,HIGH);
  
    client.setServer(server, 1883);
    client.setCallback(callback);
  
    Ethernet.begin(mac, ip);
    
    // TIMER 1 for interrupt frequency 100 Hz:
    noInterrupts();
    // Clear registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
  
    // 100 Hz (16000000/((624+1)*256))
    OCR1A = 624;
    // CTC
    TCCR1B |= (1 << WGM12);
    // Prescaler 256
    TCCR1B |= (1 << CS12);
    // Output Compare Match A Interrupt Enable
    TIMSK1 |= (1 << OCIE1A);
    interrupts();

    lcd.backlight(); // finish with backlight on
    //-------- Write characters on the display ------------------
    // NOTE: Cursor Position: Lines and Characters start at 0
    lcd.setCursor(5,0); //Start at character 5 on line 0
    lcd.print("Syergie");
    lcd.setCursor(1,2); //Start at character 1 on line 2
    lcd.print("Azimuth Thruster");
    lcd.setCursor(5,3); //Start at character 5 on line 3
    lcd.print("Control");
    for(int i = 0; i< 3; i++){
      lcd.noBacklight();
      delay(250);
      lcd.backlight();
      delay(250);
    }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  /* LC Displaying */
  sprintf((char*) lcd_buffer0, "Azimuth=%3d   Mode:%1d",azimuth_position,mode);  
  sprintf((char*) lcd_buffer1, "Propeller=%4dRPM", propeller_speed);
  sprintf((char*) lcd_buffer2, "Engine=%sRPM", engine_speed.c_str());
  sprintf((char*) lcd_buffer3, "Pump=%sRPM", pump_speed.c_str());
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print(lcd_buffer0);
  lcd.setCursor(0,1);
  lcd.print(lcd_buffer1);
  lcd.setCursor(0,2);
  lcd.print(lcd_buffer2);
  lcd.setCursor(0,3);
  lcd.print(lcd_buffer3);
  
  /* RPM Pump, Engine, and Azimuth Sensor, Fuel Reading */
  fuel_level = map(analogRead(fuel_pin),0,1023,0,20);
  
  client.loop();
}

ISR(TIMER1_COMPA_vect){          // timer compare interrupt service routine
    noInterrupts();
    
    if ((micros() - prevmillis) > (60000000/tickmark)){   //Membatasi waiting time max 1menit, selebihnya rpm=0
      propeller_speed = 0;
    } else if (rpm>0 && rpm<=rpm_max){  //Verifikasi nilai in range         
      propeller_speed = rpm;
    }    

    motor_start = true;
    
    PID_Speed();
    PID_Position();
    
    interrupts();
}

void PID_Speed(){
    /* Position Control */
    double PTerm_speed;
    double ITerm_speed;
    double DTerm_speed;
    double PIDTerm_speed;
    static double e_speed_pre = 0;  // error kecepatan lama
    static double e_speed = 0;      // perbedaan feedback dengan input
    static double e_speed_sum = 0;  // hasil integral error
    static double speed_pwm_pulse = 0;
    
    // Perhitungan PID Speed
    if(motor_start){  //Propeller Start
        e_speed       = set_speed - propeller_speed;              // Calculate error
        e_speed_sum  += e_speed;                                  // Calculate integral of error
        PTerm_speed   = e_speed * kp_speed;                       // Proportional term
        ITerm_speed   = e_speed_sum * ki_speed;                   // Integral term
        DTerm_speed   = kd_speed * (e_speed - e_speed_pre);       // Derivative term
        PIDTerm_speed = PTerm_speed + ITerm_speed + DTerm_speed;  // Calculate total output
        e_speed_pre   = e_speed;                                  // Save error to previous error
        
        if (PIDTerm_speed >= maxPWMspeed)                         // Restrict to max/min
          PIDTerm_speed = maxPWMspeed;
        if (PIDTerm_speed <= 0)
          PIDTerm_speed = 0;
        
        speed_pwm_pulse = PIDTerm_speed; // Hasil PID dijadikan data kendali kecepatan
        
        if (speed_pwm_pulse == 0){
            propeller_speed = 0;
            enablerpm = false;
            duration=0;
        }
    }
    else{ //Propeller Stop
        propeller_speed = 0;
        e_speed = 0;
        e_speed_pre = 0;
        e_speed_sum = 0;
        speed_pwm_pulse = 0;
        enablerpm = false;
        duration=0;
    }

    analogWrite(flow_control_pin, speed_pwm_pulse);
}

void PID_Position(){
    /* Position Control */
    double PTerm_position;
    double ITerm_position;
    double DTerm_position;
    double PIDTerm_position;
    static double e_position_pre = 0;   // error posisi lama
    static double e_position = 0;       // perbedaan feedback dengan input
    static double e_position_sum = 0;   // hasil integral error
    static double position_pwm_pulse = 0;
    
    //PID Position Control
    e_position      = set_position - azimuth_position;                  // Calculate error
    e_position_sum += e_position;                                       // Calculate integral of error
    PTerm_position  = e_position * kp_position;                         // Proportional term
    ITerm_position  = e_position_sum * ki_position;                     // Integral term
    DTerm_position  = kd_position * (e_position - e_position_pre);      // Derivative term
    PIDTerm_position= PTerm_position + ITerm_position + DTerm_position; // Calculate total output
    e_position_pre  = e_position;                                       // Save error to previous error
    
    if (PIDTerm_position >= 255)                                        // Restrict to max/min
      PIDTerm_position = 255;
    if (PIDTerm_position <= -255)
      PIDTerm_position = -255;
    
    position_pwm_pulse = PIDTerm_position;  // Hasil PID dijadikan data kontrol posisi

    if (position_pwm_pulse >= 0){  // Jika nilai kecepatan (+), stir berputar ke kanan
      int forwardPWM = position_pwm_pulse;
      analogWrite(right_azimuth_pin, forwardPWM);
      analogWrite(left_azimuth_pin, 0);    
    } 
    else if (position_pwm_pulse < 0){ // Jika nilai kecepatan (-), stir berputar ke kiri
      int reversePWM = -position_pwm_pulse;
      analogWrite(right_azimuth_pin, 0);
      analogWrite(left_azimuth_pin, reversePWM);
    } 

}

void RPM() {
    boolean currentstate = digitalRead(interruptPin); // Current state of Proximity input scan
    
    if( currentstate == LOW ) // If input only changes from HIGH to LOW
    {
      if (enablerpm==true){
        duration = ( micros() - prevmillis ); // Time difference between revolution in microsecond  
      }else{
        duration = 0;
        enablerpm = true;   
      }
      prevmillis = micros(); // store time for nect revolution calculation
    }
    else
    {
      rpm = (60000000/duration/tickmark); // rpm = (1/ time millis)*1000*1000*60;
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String myPayload = "";
  
  for (int i=0;i<length;i++) { //storing payload to integer value
    char inChar = (char)payload[i];
    // add it to the inputString:
    if (inChar != '\n') {
      myPayload += inChar;
    }
  }
  
  if (strcmp(topic,setPointSpeedSubscribe) == 0){
    int value = myPayload.toInt();  //get string after set_speed
    client.publish(responSetPointSpeedPublish,dtostrf(value, 4, 0, buf));
    set_speed = value;
    Serial.print("Propeller Set Point Speed = ");
    Serial.println(set_speed);
    
  }
  else if (strcmp(topic,setPointPositionSubscribe) == 0){
    int value = myPayload.toInt();  //get string after set_azimuth
    client.publish(responSetPointPositionPublish,dtostrf(value, 3, 0, buf));
    set_position = value;
    Serial.print("Azimuth Set Point Position = ");
    Serial.println(set_position);
  }  
  else if(topic[0]=='M'){
    client.publish(speedPublish,dtostrf(propeller_speed, 4, 0, buf));
    client.publish(positionPublish,dtostrf(azimuth_position, 3, 0, buf));
    client.publish(enginePublish,engine_speed.c_str());
    client.publish(pumpPublish,pump_speed.c_str());
    client.publish(fuelPublish,dtostrf(fuel_level, 2, 0, buf));
    client.publish(statusPublish,"connected");    
  }

  Serial.println();
  dtostrf(0, 10, 0, buf);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttClient)) {
      Serial.println("connected");
      client.publish(statusPublish,"connected");
      client.publish(speedPublish,"connected");
      client.publish(positionPublish,"connected");    
      client.publish(enginePublish,"connected");
      client.publish(pumpPublish,"connected");  
      client.publish(fuelPublish,"connected"); 
      client.publish(responSetPointPositionPublish,"connected"); 
      client.publish(responSetPointSpeedPublish,"connected"); 
      client.subscribe(setPointPositionSubscribe);
      client.subscribe(setPointSpeedSubscribe);
      client.subscribe(modeSubscribe);
      client.subscribe(mainSubscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void serialEvent1() {
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read();
    // add it to the inputString:
    inputString1 += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      pump_speed = inputString1;
      inputString1 = "";
      Serial.print("Pump Speed = ");
      Serial.println(pump_speed);
    }
  }
}

void serialEvent2() {
  while (Serial2.available()) {
    // get the new byte:
    char inChar = (char)Serial2.read();
    // add it to the inputString:
    inputString2 += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      engine_speed = inputString2;
      inputString2 = "";
      Serial.print("Engine Speed = ");
      Serial.println(engine_speed);
    }
  }
}

void serialEvent3() {
  while (Serial3.available()) {
    // get the new byte:
    char inChar = (char)Serial3.read();
    // add it to the inputString:
    inputString3 += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      azimuth_position = inputString3.toInt();
      inputString3 = "";
      Serial.print("Azimuth Position = ");
      Serial.println(azimuth_position);
    }
  }
}
