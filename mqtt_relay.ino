#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/* MQTT constants */
const char* ssid = "Bruno's universe";
const char* password = "halfmoonrun2017";
const char* mqtt_server = "192.168.1.2";// Your PC IP address

WiFiClient espClient;
PubSubClient client(espClient);

const int BLUE_LED = 2;

const int BLUE_RELAY = 5;
const int RED_RELAY = 4;
const int GREEN_RELAY = 0;
const int LAMP_RELAY = 14;

const char blueRelayTopic[] = "BLUE_RELAY";
const char redRelayTopic[] = "RED_RELAY";
const char greenRelayTopic[] = "GREEN_RELAY";
const char lampRelayTopic[] = "LAMP_RELAY";
const char allRelayTopic[] = "ALL_RELAY";

static void initRelays(void)
{
   pinMode(BLUE_RELAY, OUTPUT);
   pinMode(RED_RELAY, OUTPUT);
   pinMode(GREEN_RELAY, OUTPUT);
   pinMode(LAMP_RELAY, OUTPUT);
}

static void turnOffAllRelays(void)
{
  digitalWrite(BLUE_RELAY, HIGH);
  digitalWrite(RED_RELAY, HIGH);
  digitalWrite(GREEN_RELAY, HIGH);
  digitalWrite(LAMP_RELAY, HIGH);
}

void setup() {
  pinMode(BLUE_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BLUE_LED, HIGH);
  initRelays();
  turnOffAllRelays();
  
  Serial.begin(115200);
  // Start the wifi and mqtt
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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

static void controlRelay(String command, int pin)
{
  if(command == "ON")
    digitalWrite(pin, LOW);
  else if(command == "OFF")
    digitalWrite(pin, HIGH);
}

static void processNewCommand(String topic, String command)
{
  if(topic == blueRelayTopic)
    controlRelay(command, BLUE_RELAY);
  else if(topic == redRelayTopic)
    controlRelay(command, RED_RELAY);
  else if(topic == greenRelayTopic)
    controlRelay(command, GREEN_RELAY);
  else if(topic == lampRelayTopic)
    controlRelay(command, LAMP_RELAY);
  else if(topic == allRelayTopic)
  {
    //controlRelay(command, BLUE_RELAY);
    controlRelay(command, RED_RELAY);
    controlRelay(command, GREEN_RELAY);
    controlRelay(command, LAMP_RELAY);
  }
}

static String parseMessage(byte* payload, unsigned int length)
{
  String message = "";
  
  for (int i = 0; i < length; i++)
    message+=(char)payload[i];

  return message;
}
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  String command = parseMessage(payload, length);
 
  Serial.print("Message: ");
  Serial.println(command);
  processNewCommand((String)topic, command);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(blueRelayTopic);
      client.subscribe(redRelayTopic);
      client.subscribe(greenRelayTopic);
      client.subscribe(lampRelayTopic);
      client.subscribe(allRelayTopic);
      digitalWrite(BLUE_LED, LOW);
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
}
