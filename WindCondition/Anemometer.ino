#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

const int windDirectPin = A1;  
const int windSpeedPin = A0;

int sensorValue1 = 0;        
int sensorValue2 = 0;      
int windDirect = 0;        
int windSpeed = 0;

char buf[6];

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x01 };
IPAddress ip(123, 45, 0, 12);
IPAddress server(123, 45, 0, 10);

EthernetClient ethClient;
PubSubClient client(ethClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if(topic[0]="M"){
    sprintf(buf, "%d", windDirect);
    client.publish("WindDirection",buf);
    sprintf(buf, "%d", windSpeed);
    client.publish("WindSpeed",buf);
    client.publish("WindBox","connected");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("WindCondition")) {
      Serial.println("connected");
      client.publish("WindDirection","connected");
      client.publish("WindSpeed","connected");
      client.publish("WindBox","connected");
      // Once connected, publish an announcement...
      client.subscribe("MainControl");
      // ... and resubscribe
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(57600);

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  // read the analog in value:
  sensorValue1 = analogRead(windDirectPin);
  sensorValue2 = analogRead(windSpeedPin);
  // map it to the range of the analog out:
  windDirect = map(sensorValue1, 0, 1023, 0, 360);
  windSpeed = map(sensorValue2, 0, 1023, 0, 30);
  
  if (!client.connected()) {
    reconnect();
  }
  Serial.print(".");
  client.loop();
}
