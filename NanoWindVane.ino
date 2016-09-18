//============================================================================
// Name        : NanoWindVane.ino
// Author      : R. BELLO <https://github.com/rbello>
// Version     : 1.0
// Copyright   : Creative Commons (by)
// Description : Small anemometer able to send climatic conditions using RF 433.
//============================================================================

#define PIN_TACHO 2
#define PIN_DHT 9
#define PIN_433_EMITTER 3
#define TACHO_UPDATE_DELAY 100
#define UPDATE_DELAY 60000

#include <Arduilink.h>
#include <dht.h>
#include <RCSwitch.h>

unsigned long lastUpdate = 0; // Time stamp for general update
unsigned long lastTachoUpdate = 0; // Time stamp for tachometer sensor update
unsigned int lastTachoValue = HIGH;

// Sensor RF 433Mhz
RCSwitch rf433write = RCSwitch();

// Arduilink
Arduilink lnk = Arduilink(3);

// DHT Sensor
dht DHT;

// Send to RF 433Mhz emitter
void sendRf433(unsigned long data) {
  rf433write.send(data, 32);
}

void setup()
{
  // Add sensors
  lnk.addSensor(1, S_INFO, "Tachometre", "RPM")->verbose = true;
  lnk.addSensor(2, S_INFO, "Temperature", "C")->verbose = true;
  lnk.addSensor(3, S_INFO, "Humidite", "%")->verbose = true;
  
  // Enable RF 433
  rf433write.enableTransmit(PIN_433_EMITTER);
  rf433write.setPulseLength(320);
  rf433write.setProtocol(2);
  rf433write.setRepeatTransmit(5);

  // Tachometer
  pinMode(PIN_TACHO, INPUT);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  lnk.init();
}

// Work variables
double currentTemperature = 0;
double currentHumidity = 0;
unsigned long rpm = 0;

void loop()
{

  // Update
  if (millis() - lastUpdate > UPDATE_DELAY)
  {
    lastUpdate = millis();
    unsigned long rpmCopy = rpm;
    rpm = 0;
    // DHT Sensor
    int chk = DHT.read22(PIN_DHT);
    if (chk != DHTLIB_OK)
    {
      currentTemperature = currentHumidity = -1;
    }
    else
    {
      lnk.setValue(2, currentTemperature = DHT.temperature);
      lnk.setValue(3, currentHumidity = DHT.humidity);
      sendRf433(lnk.getEncoded32(2));
      delay(1000);
      sendRf433(lnk.getEncoded32(3));
      delay(1000);
    }
    // RPM
    lnk.setValue(1, rpmCopy);
    sendRf433(lnk.getEncoded32(1));
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
