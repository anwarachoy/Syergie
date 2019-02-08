#define ledPin 13
#define interruptPin  2

const int tickmark = 4;
const int rpm_max = 5000;
unsigned long prevmillis = 0; // To store time
unsigned long duration; // To store time difference
unsigned long refresh; // To store time for refresh of reading

boolean currentstate; // Current state of Proximity input scan

int rpm_1,rpm;

float EMA_a_low = 0.05;     //initialization of EMA alpha (cutoff-frequency)
float EMA_a_high = 0.4;
 
int EMA_S_low = 0;          //initialization of EMA S
int EMA_S_high = 0;
 
int highpass = 0;
int bandpass = 0;
int bandstop = 0;
 

void setup()
{
    Serial.begin(115200);
    
    attachInterrupt(digitalPinToInterrupt(interruptPin), RPM, CHANGE);
    
    pinMode(ledPin, OUTPUT);
    
    // TIMER 1 for interrupt frequency 100 Hz on Arduino Mega:
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


void loop()
{
  
}

ISR(TIMER1_COMPA_vect){          // timer compare interrupt service routine
    if ((micros() - prevmillis) > (60000000/tickmark)){
      rpm=0;
    }
    if (rpm>=0 && rpm<=rpm_max){
      rpm_1=rpm;
    }    
    EMA_S_low = (EMA_a_low*rpm) + ((1-EMA_a_low)*EMA_S_low);          //run the EMA
    EMA_S_high = (EMA_a_high*rpm) + ((1-EMA_a_high)*EMA_S_high);
     
    bandpass = EMA_S_high - EMA_S_low;        //find the band-pass as before
    
    bandstop = rpm - bandpass;        //find the band-stop signal
    
    //Serial.print(bandstop);
    //Serial.print(" ");
    //Serial.print(bandpass);
    //Serial.print(" ");
    //Serial.print("RPM : ");
    Serial.println(rpm_1);
    digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
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
