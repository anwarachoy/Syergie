//#define ledPin 13
#define interruptPin  3

const int tickmark = 1;
const int rpm_max = 5000;
unsigned long prevmillis = 0; // To store time
unsigned long duration; // To store time difference
unsigned long refresh; // To store time for refresh of reading

boolean currentstate; // Current state of Proximity input scan

int rpm_1,rpm;
 
int highpass = 0;
int bandpass = 0;
int bandstop = 0;
 

void setup()
{
    Serial.begin(115200);
    
    attachInterrupt(digitalPinToInterrupt(interruptPin), RPM, CHANGE);
    
    //pinMode(ledPin, OUTPUT);

    for (int i=2;i<=13;i++){
      pinMode(i, INPUT);
    }
    
    // TIMER 1 for interrupt frequency 1000 Hz:
    cli(); // stop interrupts
    TCCR1A = 0; // set entire TCCR1A register to 0
    TCCR1B = 0; // same for TCCR1B
    TCNT1  = 0; // initialize counter value to 0
    // set compare match register for 1000 Hz increments
    OCR1A = 15999; // = 16000000 / (1 * 1000) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS12, CS11 and CS10 bits for 1 prescaler
    TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    sei(); // allow interrupts
}


void loop()
{
  
}

ISR(TIMER1_COMPA_vect){          // timer compare interrupt service routine
    static int count = 0;
    
    if ((micros() - prevmillis) > (60000000/tickmark)){
      rpm=0;
    }
    if (rpm>=0 && rpm<=rpm_max){
      rpm_1=rpm;
    }    

    count++;
    count%=20;
    
    if(count==15){
      Serial.println(rpm_1);
    }
    //digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
}

void RPM() {
    currentstate = digitalRead(interruptPin);
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
