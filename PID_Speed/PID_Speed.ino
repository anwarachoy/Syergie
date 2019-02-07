#define interruptPin  2
#define switchon 4
#define switchturn 3
#define RPWM 9
#define LPWM 10

String mySt = "";
char myChar;
boolean stringComplete = false;  // whether the string is complete
boolean motor_start = false;
boolean m_direction = true;

const int tickmark = 1;
const int rpm_max = 2500;
unsigned long prevmillis = 0; // To store time

double rpm;

double set_speed = 100;

double kp = 0.00007936;
double ki = 0.0007936;
double kd = 0;

void setup() {
    // put your setup code here, to run once:
    
    pinMode(switchon,INPUT_PULLUP);
    pinMode(switchturn,INPUT_PULLUP);
    for(int i=5;i<9;i++){
        pinMode(i,OUTPUT);
    }
    for(int i=5;i<9;i++){
        digitalWrite(i,LOW);
    }
    delay(1000);
    
    Serial.begin(115200);

    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    
    attachInterrupt(digitalPinToInterrupt(interruptPin), RPM, CHANGE);
    
    // TIMER 1 for interrupt frequency 100 Hz:
    cli(); // stop interrupts
    TCCR1A = 0; // set entire TCCR1A register to 0
    TCCR1B = 0; // same for TCCR1B
    TCNT1  = 0; // initialize counter value to 0
    // set compare match register for 100 Hz increments
    OCR1A = 19999; // = 16000000 / (8 * 100) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS12, CS11 and CS10 bits for 8 prescaler
    TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);
    // enable timer compare interrupt
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
      }
      else if (mySt.substring(0,4) == "stop"){
        motor_start = false;
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
    
    static double m_speed = 0;
    static double e_speed_pre = 0; // posisi lama
    static double e_speed_buff = 0; // posisi baru
    static double e_speed = 0; // perbedaan feedback dengan input
    static double e_speed_sum = 0; // hasil integral error
    static double pwm_pulse = 0;
    
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
    }else{
        m_speed = 0;
        e_speed = 0;
        e_speed_pre = 0;
        e_speed_buff = 0;
        e_speed_sum = 0;
        pwm_pulse = 0;
    }

    // Jika m_direction true, motor berputar searah jarum jam
    if (m_direction){
      // putar clockwise
      analogWrite(RPWM, pwm_pulse);
      analogWrite(LPWM, 0);
    }else{
      // putar counterclockwise
      analogWrite(RPWM, 0);
      analogWrite(LPWM, pwm_pulse);
    }
      
    Serial.print(m_speed);
    Serial.print("\t");
    Serial.println(set_speed);
}

void RPM() {
    boolean currentstate = digitalRead(interruptPin); // Current state of Proximity input scan
    unsigned long duration; // To store time difference
    
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
