#include <Arduino.h>
#include "DHT.h"
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

void setup() {
// put your setup code here, to run once:
//Initializare port serial cu baud rate 115200 kbps
Serial.begin(115200);
Serial.println();
dht.begin();
pinMode(LightSensorPIN, INPUT); // definirea pinului la care este conectat senzorul ca intrare
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
Serial.print("Humidity: ");
Serial.print(humidity);
Serial.println(" %");
//SENZOR LUMINA
lightStatus = digitalRead(LightSensorPIN); // citirea valorii pinului la care este conectat senzorul
if(lightStatus == HIGH){
Serial.println("No light detected"); 
}
else{
Serial.println("Light detected");
}
}
}
