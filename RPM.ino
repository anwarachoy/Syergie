#include "Syergie.h" 
Proximity speed_1;
Proximity speed_2;

int rpm_1,rpm_2;

void setup()
{
  speed_1.pinRPM(A2);
  speed_2.pinRPM(A3);
  Serial.begin(9600);
}

void loop()
{
  rpm_1 = speed_1.calcRPM();
  rpm_2 = speed_2.calcRPM();
  Serial.print(rpm_1);
  Serial.print(" -\t- ");
  Serial.println(rpm_1);
}
