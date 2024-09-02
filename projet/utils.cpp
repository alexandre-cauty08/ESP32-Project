#include "utils.h"
#include <ArduinoJson.h>

extern char buffer[250];
StaticJsonDocument<250> jsonDocument;

// Emission d'un son de fréquence 
void myTone(byte pin, int freq) 
{
  ledcSetup(0, 2000, 8); // setup beeper
  ledcAttachPin(pin, 0); // attach beeper
  ledcWriteTone(0, freq); // play tone  
}

// Creation d'un fichier json 
void create_json(char *tag, float value, char *unit) 
{  
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);  
}

// Ajoute d'un champs dans le fichier json existant
void add_json_object(char *tag, float value, char *unit) 
{
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit; 
}


