#include <Arduino.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
//WiFi
#define WIFI_SSID "TP-Link_165C"
#define WIFI_PASSWORD "99368319"

#define MQTT_HOST IPAddress(192, 168, 0, 105)
#define MQTT_PORT 1883
#define MQTT_PUB_TEST "test"
#define MQTT_PUB_TEMP "temp"
#define MQTT_PUB_HUM "hum"
#define MQTT_PUB_LIGHT "light"
#define MQTT_PUB_GAS "gas"
AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

//DHT11
//definire pinul de output al senzorului DHT
#define DHTPIN 5
//definirea tipului de senzor(bilbioteca este facuta pentru mai multe tipuri de DHT)
#define DHTTYPE DHT11
//Initializare senzor
DHT dht(DHTPIN, DHTTYPE);
//Declaratie variabile
float temperature;
float humidity;
//Declaratie variabile si constante folosite pentru timer-ul de masura
//intervalele de timp se vor masura in milisecunde
unsigned long previousTime = 0; //variabila va stoca timpul la care s-a petrecut ultima masuratoare
const long interval = 2000; // 10000ms = 10s
//SENZOR LUMINA
const int LightSensorPIN = 4; //pinul la care este conectat senzorul
int lightStatus = LOW; // variabila in care se salveaza prezenta luminii
//MQ9
const int analogInputPin = A0; //pinul A0 este pinul analog
int gasSensorValue = 0; //initializare valoare cu 0
//se va schimba dupa calibrare
const int gasThreshold = 500; //pragul la care se detecteaza gaz
//Wifi and MQTT functions
void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_TEST, 1, true, "test");
  Serial.print("Publishing at QoS 1, packetId: ");
  Serial.println(packetIdPub1);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}
void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup() {
    // put your setup code here, to run once:
    //Initializare port serial cu baud rate 115200 kbps
    Serial.begin(115200);
    Serial.println();
    
    //Wifi
    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);

    connectToWifi();
    pinMode(LightSensorPIN, INPUT); // definirea pinului la care este conectat senzorul ca intrare
    dht.begin();
}

void loop() {
    // put your main code here, to run repeatedly:
    //DHT11
    unsigned long currentTime = millis(); // millis() returneaza numarul de milisecunde trecute de la inceperea rularii programului pe placa Arduino
    if(currentTime - previousTime >= interval){ // verificare conditie timer: daca au trecut (interval) secunde
        previousTime = currentTime; //Salveaza timpul curent ca timpul la care s-a facut ultima masuratoare
        //Noile masuratori ale senzorului
        temperature = dht.readTemperature();
        humidity = dht.readHumidity();
        //Afisare pe portul serial a valorilor masurate
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" C");
        uint16_t packetIdPubTemp = mqttClient.publish(MQTT_PUB_TEMP, 1, true, String(temperature).c_str());
        Serial.print("Publishing at QoS 1, packetId: ");
        Serial.println(packetIdPubTemp);

        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println(" %");

        uint16_t packetIdPubHum = mqttClient.publish(MQTT_PUB_HUM, 1, true, String(humidity).c_str());
        Serial.print("Publishing at QoS 1, packetId: ");
        Serial.println(packetIdPubHum);
        gasSensorValue = analogRead(analogInputPin);
        Serial.println(gasSensorValue);
        uint16_t packetIdPubGas = mqttClient.publish(MQTT_PUB_GAS, 1, true, String(gasSensorValue).c_str());
        Serial.print("Publishing at QoS 1, packetId: ");
        Serial.println(packetIdPubGas);
        lightStatus = digitalRead(LightSensorPIN); // citirea valorii pinului la care este conectat senzorul
        uint16_t packetIdPubLight = mqttClient.publish(MQTT_PUB_LIGHT, 1, true, String(lightStatus).c_str());
        Serial.print("Publishing at QoS 1, packetId: ");
        Serial.println(packetIdPubLight);
    }
    //SENZOR LUMINA
    //MQ9
    //Inca nu este calibrat!
}
