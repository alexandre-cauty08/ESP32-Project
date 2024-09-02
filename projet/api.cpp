#include "api.h"
#include "utils.h"
#include <Arduino.h>
#include <WebServer.h> 

char buffer[250];
extern WebServer server;

extern int pinTemp;
extern int seuilAlarme;
extern int BUZZER_PIN;
extern int pinLight;
extern int mode;

extern double VCC;
extern double R2;           
extern double adc_resolution;
extern double A;   
extern double B;
extern double C;
extern double VIN;
extern double R;

void allume()
{
  mode = 0;
  create_json("Lumiere allumee",mode,".");
  server.send(200, "application/json", buffer);
}
void eteindre()
{
  mode = 1;
  create_json("Lumiere eteinte",mode,".");
  server.send(200, "application/json", buffer);
}

//envoie des données de la photo-résistance
void getLight(){   
  
    int valeurlum = analogRead(pinLight);
    float Vout0=valeurlum*0.0048828125;   
    float lux = tensToLux(Vout0);
  
    create_json("le taux de lumiere est egale a  ", lux , "lum");
    server.send(200, "application/json", buffer);
}

//envoie des données de la thermorésistance
void getTemp()
{  

    int analgtemp = analogRead(pinTemp);
    double Vout = (analgtemp * VCC) / adc_resolution;
    double Rth = (VCC * R2 / Vout) - R2;

    double temperature = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)),3))));   // Temperature en kelvin
    temperature = temperature - 273.15;  // Temperature en degres celsius

    create_json("la temp est egale a  ", temperature , "degres");
    server.send(200, "application/json", buffer);
}

// Augmentation du seuil de lumière
void AugmenteSeuil()
{
  seuilAlarme += 100;
  create_json("Le seuil a été augmenté de  ", 100 , "lux");
  server.send(200, "application/json", buffer);
}

// Diminution du seuil de lumière
void DiminueSeuil()
{
  seuilAlarme += -100;
  create_json("Le seuil dimune de ", 100 , "lux");
  server.send(200, "application/json", buffer);   
}

// Envoie les capteurs connectés à l'ESP32
void getCapteurs()
{   
    create_json("Capteur de lumière ", 1, ".");
    add_json_object("Capteur de température ", 1, ".");
    add_json_object("Buzzer ", 1, ".");
    server.send(200, "application/json", buffer);
}

int tensToLux(int raw)
{
  // Conversion rule
  float Vout = float(raw) * (VIN / float(1024));// Conversion analog to voltage
  float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
  int phys=500/(RLDR/1000); // Conversion resitance to lumen
  return phys;
}

void handleNotFound()
{  // Page Not found
  server.send(404, "text/plain","404: Not found");
}
