#include <Arduino.h>
#include "DHT.h"

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
void setup() {
// put your setup code here, to run once:
//Initializare port serial cu baud rate 115200 kbps
Serial.begin(115200);
Serial.println();
dht.begin();
}

void loop() {
// put your main code here, to run repeatedly:
unsigned long currentTime = millis(); // millis() returneaza numarul de milisecunde trecute de la inceperea rularii programului pe placa Arduino
if(currentTime - previousTime >= interval){ // verificare conditie timer: daca au trecut (interval) secunde
previousTime = currentTime; //Salveaza timpul curent ca timpul la care s-a facut ultima masuratoare
//Noile masuratori ale senzorului
temperature = dht.readTemperature();
humidity = dht.readHumidity();
//Afisare pe portul serial a valorilor masurate
Serial.print("Temperature is: ");
Serial.println(temperature);
Serial.print("Humidity is: ");
Serial.println(humidity);
}
}
