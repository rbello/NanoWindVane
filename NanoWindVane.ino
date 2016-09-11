//============================================================================
// Name        : Girouette.ino
// Author      : R. BELLO <https://github.com/rbello>
// Version     : 1.0
// Copyright   : Creative Commons (by)
// Description : ...
//============================================================================

#define PIN_TACHO 2
#define TACHO_UPDATE_DELAY 100
#define PIN_DHT 9
#define DHT_UPDATE_DELAY 2000 // 302000 // 5 min + 2 sec
#define PIN_RADIO_CE 5
#define PIN_RADIO_CSN 4

#include <dht.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(PIN_RADIO_CE, PIN_RADIO_CSN);

dht DHT;
unsigned long lastUpdate = 0; // Time stamp for general update
unsigned long lastTachoUpdate = 0; // Time stamp for tachometer sensor update
unsigned int lastTachoValue = HIGH;

void setup()
{
  Serial.begin(9600);
  printf_begin();
  pinMode(PIN_TACHO, INPUT);
  radio.begin();
  radio.setRetries(15, 15);
  radio.setPayloadSize(8);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.stopListening();
  radio.printDetails();
  Serial.println("Ready!");
}

// Work variables
double currentTemperature = 0;
double currentHumidity = 0;
int rpm = 0;

void loop()
{

  // DHT update
  if (millis() - lastUpdate > DHT_UPDATE_DELAY)
  {
    int chk = DHT.read22(PIN_DHT);
    if (chk != DHTLIB_OK)
    {
      currentTemperature = currentHumidity = -1;
    }
    else
    {
      currentTemperature = DHT.temperature;
      currentHumidity = DHT.humidity;
    }
    Serial.print("Update: Humidity="); 
    Serial.print(currentHumidity);
    Serial.print("%\t");
    Serial.print("Temperature="); 
    Serial.print(currentTemperature);
    Serial.print("C\tRPM=");
    Serial.println(rpm);
    unsigned long payload = 6666;
    radio.write(&payload, sizeof(unsigned long));
    rpm = 0;
    lastUpdate = millis();
  }

  if (digitalRead(PIN_TACHO) != lastTachoValue) {
    lastTachoValue = lastTachoValue > LOW ? LOW : HIGH;
    if (lastTachoValue == LOW && millis() - lastTachoUpdate > TACHO_UPDATE_DELAY)
    {
      lastTachoUpdate = millis();
      rpm += 1;
    }
  }
  
}
