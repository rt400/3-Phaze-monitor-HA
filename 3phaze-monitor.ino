#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <PZEM004T.h>
#include "DHTesp.h"
/**
  Wifi & MQTT Settings
*/
const char* ssid = ""; // need to change
const char* password = ""; // need to change
const char* mqtt_server = "";// need to change
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
float volt = 0;
float currentA = 0;
float watt = 0;
float wattHour = 0 ;
int dhtCount = 0;
float humidity = 0.0f;
float temperature = 0.0f;
DHTesp dht;

PZEM004T pzem1(D4, D3); // (RX,TX) connect to TX,RX of PZEM
PZEM004T pzem2(D2, D1);
PZEM004T pzem3(D6, D5);
IPAddress ip(192, 168, 1, 1);

void setup() {
  dht.setup(D0, DHTesp::DHT11);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pzem1.setAddress(ip);
  pzem2.setAddress(ip);
  pzem3.setAddress(ip);
}
/**
  Connect to wifi
*/
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
void loop() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("3PhazesMonitors", "USER_NAME", "PASSWORD")) { // need to change if you need auth
      Serial.println("connected");
      client.loop();
    } else {
      delay(2000);
    }
  }
  
  gettemperature();
  // Monitor 1
  volt = pzem1.voltage(ip);
  dtostrf(volt, 6, 2, msg);
  client.publish("volt1/3PhazesMonitors", msg);
  currentA = pzem1.current(ip);
  dtostrf(currentA, 6, 2, msg);
  client.publish("current1/3PhazesMonitors", msg);
  watt = pzem1.power(ip);
  dtostrf(watt, 6, 2, msg);
  client.publish("watt1/3PhazesMonitors", msg);
  wattHour = pzem1.energy(ip);
  dtostrf(wattHour, 6, 2, msg);
  client.publish("energy1/3PhazesMonitors", msg);
  delay(100);

  //Monitor 2
  volt = pzem2.voltage(ip);
  dtostrf(volt, 6, 2, msg);
  client.publish("volt2/3PhazesMonitors", msg);
  currentA = pzem2.current(ip);
  dtostrf(currentA, 6, 2, msg);
  client.publish("current2/3PhazesMonitors", msg);
  watt = pzem2.power(ip);
  dtostrf(watt, 6, 2, msg);
  client.publish("watt2/3PhazesMonitors", msg);
  wattHour = pzem2.energy(ip);
  dtostrf(wattHour, 6, 2, msg);
  client.publish("energy2/3PhazesMonitors", msg);
  delay(100);

  // Monitor 3
  volt = pzem3.voltage(ip);
  dtostrf(volt, 6, 2, msg);
  client.publish("volt3/3PhazesMonitors", msg);
  currentA = pzem3.current(ip);
  dtostrf(currentA, 6, 2, msg);
  client.publish("current3/3PhazesMonitors", msg);
  watt = pzem3.power(ip);
  dtostrf(watt, 6, 2, msg);
  client.publish("watt3/3PhazesMonitors", msg);
  wattHour = pzem3.energy(ip);
  dtostrf(wattHour, 6, 2, msg);
  client.publish("energy3/3PhazesMonitors", msg);
  delay(100);

  //Serial.println(temperature);
  dtostrf((int) temperature, 6, 0, msg);
  client.publish("temp/3PhazesMonitors", msg);
  //Serial.println(humidity);
  dtostrf((int) humidity, 6, 0, msg);
  client.publish("humidity/3PhazesMonitors", msg);
  delay(100);
  //Sleep for 30 seconds, then wakeup and send data again
  ESP.deepSleep(30 * 1000000, WAKE_RF_DEFAULT);
}

void gettemperature() {
  bool dhtOK = false;
  humidity = dht.getHumidity();
  dhtCount++;
  while (dhtOK != true) {
    dhtOK = true; // let's assume the reading is OK
    if (isnan(humidity) || isnan(temperature))
      dhtOK = false;
    if ((dhtOK == true) && (humidity == 0.0))
      dhtOK = false;
    if (dhtOK == false)
    { delay(200);
      humidity = dht.getHumidity();          // Read humidity (percent)
      //temperature = dht.getTemperature();     // Read temperature as *C
      dhtCount++;
    }
    if (dhtOK == true)
      temperature = dht.getTemperature();     // Read initial temp
  }
}
