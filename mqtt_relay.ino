#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* MQTT constants */
const char* ssid = "Bruno's universe";
const char* password = "halfmoonrun2017";
const char* mqtt_server = "192.168.1.2";// Your Raspberry Pi IP address

WiFiClient espClient;
PubSubClient client(espClient);
long tickStart;
bool toggle = true;
const int BLUE_LED = 2;

float temperature = 0;

void setup() {
  pinMode(BLUE_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  // Start the wifi and mqtt
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  digitalWrite(BLUE_LED, LOW);
  delay(2000);
  tickStart = millis();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
/*
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}
*/
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
    Serial.println();
   
  String message = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message+=(char)payload[i];
  }
  Serial.println("-----------------------");
  if(String(topic)=="LED"){
    if(message=="LED ON"){
      digitalWrite(BLUE_LED,HIGH);
      Serial.println("LED IS ON");
    }
    else{
      digitalWrite(BLUE_LED,LOW);
    }
  }
     
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("LED");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected())
    reconnect();
    
  client.loop();

  if ((millis() - tickStart) >= 2000) 
  {
    if(toggle)
      digitalWrite(BLUE_LED, HIGH);
    else
      digitalWrite(BLUE_LED, LOW);

    toggle ^= true;
    tickStart = millis();
  }
}
