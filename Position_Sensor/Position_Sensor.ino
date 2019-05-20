#define AzimuthA_Pin  2
#define AzimuthB_Pin  3
#define Set0_Pin      4
#define Set90_Pin     5
#define Set180_Pin    6
#define Set270_Pin    7

int majorvalue,minorvalue;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  for (int i=2;i<=7;i++){
    pinMode(i,INPUT);
  }
  
  attachInterrupt(digitalPinToInterrupt(AzimuthA_Pin), Minor, FALLING);

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

void loop() {
  // put your main code here, to run repeatedly:
  
}

ISR(TIMER1_COMPA_vect){          // timer compare interrupt service routine
  int last=majorvalue;
  static int count = 0;
 
  if (digitalRead(Set0_Pin)==LOW){
    majorvalue = 0;
  }else if (digitalRead(Set90_Pin)==LOW){
    majorvalue = 90;
  }else if (digitalRead(Set180_Pin)==LOW){
    majorvalue = 180;
  }else if (digitalRead(Set270_Pin)==LOW){
    majorvalue = 270;
  }

  if ( last!= majorvalue){
    minorvalue=0;
  }

  int positionvalue=majorvalue+minorvalue;
  positionvalue += 360;
  positionvalue %= 360;

  count++;
  count%=20;
  
  if(count==5){
    Serial.println(positionvalue);
  }
}

void Minor() {
  if (digitalRead(AzimuthB_Pin)==LOW){
    minorvalue+=3; 
  }else{
    minorvalue-=3;
  }
}
