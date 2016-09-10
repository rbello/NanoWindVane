//============================================================================
// Name        : Girouette.ino
// Author      : R. BELLO <https://github.com/rbello>
// Version     : 1.0
// Copyright   : Creative Commons (by)
// Description : ...
//============================================================================



#define PIN_TACHO 5
#define TACHO_UPDATE_DELAY 100
#define PIN_DHT 11
#define DHT_UPDATE_DELAY 2000 // 302000 // 5 min + 2 sec 

#include <dht.h>

dht DHT;
unsigned long lastDHTupdate = 0; // Time stamp for DHT sensor update
unsigned long lastTachoUpdate = 0; // Time stamp for tachometer sensor update
unsigned int lastTachoValue = HIGH;


void setup()
{
  Serial.begin(9600);
  pinMode(PIN_TACHO, INPUT);
  Serial.println("Ok");
}

void loop()
{
  // DHT update
  if (millis() - lastDHTupdate > DHT_UPDATE_DELAY)
  {
    lastDHTupdate = millis();
    int chk = DHT.read22(PIN_DHT);
    if (chk != DHTLIB_OK)
    {
      Serial.println("DHT: error");
    }
    else
    {
      Serial.print("DHT: Humidity="); 
      Serial.print(DHT.humidity);
      Serial.print("%\t");
      Serial.print("Temperature="); 
      Serial.print(DHT.temperature);
      Serial.println("C ");
    }
  }

  if (digitalRead(PIN_TACHO) != lastTachoValue) {
    lastTachoValue = lastTachoValue > LOW ? LOW : HIGH;
    if (lastTachoValue == LOW && millis() - lastTachoUpdate > TACHO_UPDATE_DELAY)
    {
      lastTachoUpdate = millis();
      Serial.println("Tacho: hit !");
    }
    
  }
  
}
