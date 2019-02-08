#include <ESP8266WiFi.h>  
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* mqttClient = "ESP8266Client4";
const char* WiFi_hostname = "Control-4";
const char* ssid     = "Syergie Main Control";
const char* password = "1234567890";
//const char* mqtt_server = "syergie-ai";

// MQTT Topic
const char* speedPublish = "speed4";
const char* speedSubscribe = "Set_Speed4";
const char* motorSubscribe = "Motor4";

IPAddress mqtt_server(123, 45, 0, 100);

WiFiClient espClient;
PubSubClient client(espClient);


String mySt = "";
boolean stringComplete = false;  // whether the string is complete

void setup() {
  Serial.begin(115200);
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
  if (stringComplete) {
    client.publish(speedPublish,mySt.c_str());
    Serial.println(mySt);
    // clear the string when COM receiving is completed
    mySt = "";  //note: in code below, mySt will not become blank, mySt is blank until '\n' is received
    stringComplete = false;
  }
  client.loop();
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
  
  if (strcmp(topic,speedSubscribe) == 0){
    int value = myPayload.toInt();  //get string after set_speed
    Serial.println();Serial.print("speed");Serial.print(value);
  } 
  else if (strcmp(topic,motorSubscribe) == 0){
    Serial.println();Serial.print(myPayload);
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
      client.publish(speedPublish,"connected");
      // Once connected, publish an announcement...
      // ... or not...
      // ... and resubscribe
      client.subscribe(speedSubscribe);
      client.subscribe(motorSubscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}

void serialEvent() {
}
