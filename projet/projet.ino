#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "api.h"
#include "utils.h"

// Librairie TFT
TFT_eSPI tft = TFT_eSPI();       

double VIN = 5; // voltage
double R = 0.56; //valeur ohm Resistance 
double VCC = 3.3;
double R2 = 560;           
double adc_resolution = 1023;

int seuilAlarme = 1500;
int BUZZER_PIN = 15;
int ledPin = 13;
int pinLight = 39;
int pinTemp = 38;
int mode = 2;

double A = 0.001129148;   
double B = 0.000234125;
double C = 0.0000000876741; 

// Identifiant pour se connecter au réseau
const char* ssid = "SSID";
const char* password = "PASSWORD";


WebServer server(80);


void setup() {

  Serial.begin(115200);
  delay(100);

  // Définition des pins
  pinMode(BUZZER_PIN,OUTPUT);
  pinMode(ledPin, OUTPUT); 

  // Initialisation de l'écran
  tft.init(); 
  tft.fillScreen(TFT_BLACK); 
  tft.setSwapBytes(true); 
  tft.setRotation(3);

  Serial.println("Connecting to ");
  Serial.println(ssid);
  
  //Se connecte  Wi-fi 
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  // Les différentes routes misent en place sur le serveur web 
  server.on("/on", allume);
  server.on("/off", eteindre);
  server.on("/light", getLight);
  server.on("/temp", getTemp);
  server.on("/AugmenteSeuil", AugmenteSeuil);
  server.on("/DiminueSeuil", DiminueSeuil);
  server.on("/getCapteurs", getCapteurs);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {

  server.handleClient();
  tft.setCursor(0,0);
  tft.setTextSize(1);

  double Vout, Rth, adc_value; 

  // Récupération des valeurs
  int valeurlum = analogRead(pinLight);
  int analgtemp = analogRead(pinTemp);

  float Vout0=valeurlum*0.0048828125;   
  float lux = tensToLux(Vout0);
  
  Serial.println(analgtemp);

  Vout = (analgtemp * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;

  float temperature = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)),3))));   // Temperature en kelvin

  temperature = temperature - 273.15;  // Temperature en degres celsius

  // Affichage dans le serial
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" degree celsius");

  Serial.print(F("Physical value from sensor = "));
  Serial.print(lux); 
  Serial.println(F(" lux")); 

  if(mode == 0)
  {
    tft.fillScreen(TFT_BLACK);
    digitalWrite(ledPin, HIGH); // Allume la led
  }
  else if(mode == 1)
  {
    tft.fillScreen(TFT_BLACK); // éteint la led
    digitalWrite(ledPin, LOW);
  }
  else
  {
    if(lux > seuilAlarme)
    {
    tft.fillScreen(TFT_RED);
    digitalWrite(ledPin, LOW); // éteint la LED
    myTone(BUZZER_PIN,100);
    }
    else
    {
    tft.fillScreen(TFT_GREEN);
    digitalWrite(ledPin, HIGH); // Allume la LED
    myTone(BUZZER_PIN,0);
    }
}

  // Affichage des données des capteurs et du seuil
  tft.setCursor(0, 50, 2);
  tft.print("Luminosite :");
  tft.print(lux);
  
  tft.setCursor(0, 70, 2);
  tft.print("Temperature :");
  tft.print(temperature);
  tft.print("° C");

  tft.setCursor(0, 90, 2);
  tft.print("Seuil :");
  tft.print(seuilAlarme);


  server.handleClient();

  delay(500);

}
