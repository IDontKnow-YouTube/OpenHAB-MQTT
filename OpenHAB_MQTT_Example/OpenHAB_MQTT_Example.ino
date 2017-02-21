////////
//You need to change the following variables:
//ssid, password, mqtt_server, and client name under the reconnect function



#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_SHT31.h>   //Temp + Humidity

// Update these with values suitable for your network.
const char* ssid = "WirelessAP"; //Wireless network name
const char* password = "Password"; //Wireless password
const char* mqtt_server = "IP Address"; //MQTT IP Address

Adafruit_SHT31 sht31 = Adafruit_SHT31(); //Setup SHT31 Temp + Humidity Sensor

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
  Serial.println("OK!");
  
}

void setup_wifi() {  //For Wireless Connection

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to -WirelessAP-");
  //Serial.println(ssid);

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

void reconnect() {  //For MQTT Connection
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Client Name")) {  //The name of this client (cant have 2 clients with same name)
      Serial.println("connected");
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

  if (!client.connected()) { //If the MQTT Client isnt connected, reconnect
    reconnect();
  }
  client.loop();
  
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  float shtt = t * 9.0 / 5.0 + 32; //Convert from C to F

  //Convert Float to Char Arrays
  char shttChar[10];
  char hChar[10];  
  
  dtostrf(shtt, 4, 2, shttChar);
  dtostrf(h, 4, 2, hChar);

if (shtt >35){
  client.publish("MQTT Channel", shttChar); //Can be /home/livingroom/temp or whatever channel you wish to publish to
}
  Serial.print("Temp: "); Serial.print(shtt);Serial.println("*F");
if (h < 98){
  client.publish("MQTT Channel", hChar);
  }
  Serial.print("Humidity: "); Serial.print(h);Serial.println("%H");
 
  // Repeat every 30 seconds
  delay(30000);
}
