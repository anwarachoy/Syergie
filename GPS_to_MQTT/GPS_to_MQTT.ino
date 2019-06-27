/*
 * GPS Data Transmitter
 * Converting information of position object from Reach M+ with serial communication LLH format output
 * Publish that information to server with MQTT
 * The information are latitude and longitude points of object
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define maxload 100
#define delimiter ' '

// Update these with values suitable for your network.
const char* mqttClient = "GPS";
const char* WiFi_hostname = "GPS";
const char* ssid     = "Syergie Main";
const char* password = "1234567890";

IPAddress mqtt_server(192, 168, 43, 242);

WiFiClient espClient;
PubSubClient client(espClient);

// MQTT Topic
const char* latitudePublish = "GPS/lat";
const char* longitudePublish = "GPS/long";
const char* stringPublish = "GPS/debug/string";
const char* splitPublish = "GPS/debug/split";
const char* lengthPublish = "GPS/debug/length";
const char* mainSubscribe = "MainControl";

String inputString = "";         // a string to hold incoming data
String splitString = "";         // a string to parsing incoming data
boolean stringComplete = false;  // whether the string is complete
boolean stringStart = false;

char msg[5];
int count = 0;

float latitude = 0;
float longitude = 0;
int longstr = 0;

void setup() {
  // initialize serial:
  Serial.begin(38400);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  splitString.reserve(50);
  
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.hostname(WiFi_hostname);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
   
  if (Serial.available()) {
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    splitString += inChar;
    
    if(inChar=='\n' || inChar==delimiter){
      if(!(splitString.charAt(0)==delimiter && splitString.length())){
        count += 1;
        client.publish(splitPublish,splitString.c_str());
      }

      if(count==1){
        if (splitString.charAt(4)=='/' && splitString.charAt(7)=='/')
          stringStart = true;
        else
          stringStart = false;
      }
      
      if(stringStart){        
        if(count==3){
          client.publish(latitudePublish,splitString.c_str());
        }else if(count==4){
          client.publish(longitudePublish,splitString.c_str());
        }
      }

      splitString = "";
      
      if (inChar=='\n'){ 
         stringComplete = true;
         count = 0;
      }
    }
  }
  
  // print the string when a newline arrives  
  if (stringComplete) {
    
    longstr = inputString.length();

    int nload=0;
    while(longstr>maxload){             
      client.publish(stringPublish,inputString.substring(nload,nload+maxload).c_str());
      nload+=maxload;
      longstr-=maxload;
    }
    client.publish(stringPublish,inputString.substring(nload).c_str());

    Serial.print(inputString);
    
    sprintf(msg, "%d\n", inputString.length());
    client.publish(lengthPublish,msg);
    
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print(mqtt_server);
    Serial.print(" : ");
    // Attempt to connect
    if (client.connect(mqttClient)) {
      Serial.println(" connected");
      client.publish(latitudePublish,"connected");
      client.publish(longitudePublish,"connected");
      client.publish(stringPublish,"connected");
      client.publish(lengthPublish,"connected");
      client.publish(splitPublish,"connected");
      // Once connected, publish an announcement...
      // ... or not...
      // ... and resubscribe
      client.subscribe(mainSubscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}
