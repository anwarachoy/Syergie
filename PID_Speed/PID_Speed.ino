#define interruptPin  2
#define GreenLED  3
#define RedLED  4
#define RPWM 9
#define LPWM 10

String mySt = "";
boolean stringComplete = false;  // whether the string is complete
boolean motor_start = false;
boolean m_direction = true;

int tdelay = 50;

const int tickmark = 1;
const int rpm_max = 2500;
unsigned long prevmillis = 0; // To store time
unsigned long duration; // To store time difference

double rpm;

double m_speed = 0;
double set_speed = 100;

double kp = 0.00007936;
double ki = 0.0007936;
double kd = 0;

void setup() {
    // put your setup code here, to run once:
    pinMode(RPWM,OUTPUT);
    pinMode(LPWM,OUTPUT);
    pinMode(GreenLED,OUTPUT);
    pinMode(RedLED,OUTPUT);

    digitalWrite(RPWM,LOW);
    digitalWrite(LPWM,LOW);
    digitalWrite(GreenLED,LOW);
    digitalWrite(RedLED,HIGH);
    
    delay(1000);
    
    Serial.begin(115200);
    Serial3.begin(115200);

    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    
    attachInterrupt(digitalPinToInterrupt(interruptPin), RPM, CHANGE);
    
    // TIMER 1 for interrupt frequency 100 Hz:
    cli(); // stop interrupts
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
    sei(); // allow interrupts
}

void loop() {
    if (stringComplete) {
      if (mySt.substring(0,5) == "speed"){
        set_speed = mySt.substring(5,mySt.length()).toInt();  //get string after set_speed
      }
      else if (mySt.substring(0,5) == "start"){
        motor_start = true;
        digitalWrite(13,HIGH);
      }
      else if (mySt.substring(0,4) == "stop"){
        motor_start = false;
        digitalWrite(13,LOW);
      }
      else if (mySt.substring(0,10) == "turn_right"){
        m_direction =  true;
      }
      else if (mySt.substring(0,9) == "turn_left"){
        m_direction =  false;
      }
      
      // clear the string when COM receiving is completed
      mySt = "";  //note: in code below, mySt will not become blank, mySt is blank until '\n' is received
      stringComplete = false;
    }
}

ISR(TIMER1_COMPA_vect){          // timer compare interrupt service routine
    double PTerm;
    double ITerm;
    double DTerm;
    double PIDTerm;

    static double e_speed_pre = 0; // posisi lama
    static double e_speed_buff = 0; // posisi baru
    static double e_speed = 0; // perbedaan feedback dengan input
    static double e_speed_sum = 0; // hasil integral error
    static double pwm_pulse = 0;

    static int counter=0;
    
    if ((micros() - prevmillis) > (60000000/tickmark)){
      rpm = 0;
    }
    if (rpm>=0 && rpm<=rpm_max){
      m_speed = rpm;
    }    

    // Perhitungan PID
    if(motor_start){
        e_speed = set_speed - m_speed;
        e_speed_buff = m_speed; // untuk mencari derivatif
        e_speed_sum = e_speed_sum + e_speed; // hasil integral dari error
        PTerm = e_speed*kp; // Proporsional
        ITerm = e_speed_sum*ki; // Integral
        DTerm = kd*(e_speed_pre - e_speed_buff); // Derivatif
        PIDTerm = PTerm + ITerm + DTerm; // total PID
        
        if (PIDTerm >= 255)
          PIDTerm = 255;
        if (PIDTerm <= 0)
          PIDTerm = 0;
        
        pwm_pulse = PIDTerm; // Hasil PID dijadikan data kecepatan
      
        e_speed_pre = m_speed; // untuk mencari derivatif
        Serial.print("start\t");
    }else{
        m_speed = 0;
        e_speed = 0;
        e_speed_pre = 0;
        e_speed_buff = 0;
        e_speed_sum = 0;
        pwm_pulse = 0;
        prevmillis = 0;
        duration = 0;
        Serial.print("stop\t");
    }

    // Jika m_direction true, motor berputar searah jarum jam
    if (m_direction){
      // putar clockwise
      analogWrite(RPWM, pwm_pulse);
      analogWrite(LPWM, 0);
      Serial.print("right\t");
    }else{
      // putar counterclockwise
      analogWrite(RPWM, 0);
      analogWrite(LPWM, pwm_pulse);
      Serial.print("left\t");
    }

    if (counter>=tdelay){
      //Serial3.println();
      Serial3.println(m_speed);
      counter=0;
    }
    Serial.print(m_speed);
    Serial.print("\t");
    Serial.println(set_speed);
    counter++;
}

void RPM() {
    boolean currentstate = digitalRead(interruptPin); // Current state of Proximity input scan
    
    if( currentstate == LOW ) // If input only changes from HIGH to LOW
    {
      duration = ( micros() - prevmillis ); // Time difference between revolution in microsecond
      prevmillis = micros(); // store time for nect revolution calculation
    }
    else
    {
      rpm = (60000000/duration/tickmark); // rpm = (1/ time millis)*1000*1000*60;
    }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if (inChar != '\n') {
      mySt += inChar;
    }
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void serialEvent3() {
  while (Serial3.available()) {
    // get the new byte:
    char inChar = (char)Serial3.read();
    // add it to the inputString:
    if (inChar != '\n') {
      digitalWrite(GreenLED,HIGH);
      mySt += inChar;
    }
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      digitalWrite(GreenLED,LOW);
    }
  }
}
