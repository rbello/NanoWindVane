//============================================================================
// Name        : NanoWindVane.ino
// Author      : R. BELLO <https://github.com/rbello>
// Version     : 1.1
// Copyright   : Creative Commons (by)
// Description : Small anemometer able to send climatic conditions using RF 433.
//============================================================================

#define PIN_TACHO 2
#define PIN_433_EMITTER 3
#define PIN_DHT 4
#define PIN_STANDBY 5

#define TACHO_SENSIBILITY_DELAY 100 //  1/10 sec
#define TACHO_RECORD_DELAY 30000 //  30 sec
#define STANDBY_DELAY 600000 //  10 mins
#define RF_EMIT_INTERVAL 1500 // 1,5 sec

#define SERIAL 1
#define DEBUG 0

#include <Arduilink.h>
#include <dht.h>
#include <RCSwitch.h>

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
  lnk.addSensor(1, S_INFO, "Tachometre", "RPM");
  lnk.addSensor(2, S_INFO, "Temperature", "C");
  lnk.addSensor(3, S_INFO, "Humidite", "%");
  #if SERIAL == 1
  lnk.getSensor(1)->verbose = true;
  lnk.getSensor(2)->verbose = true;
  lnk.getSensor(3)->verbose = true;
  #endif
  
  // Enable RF 433
  rf433write.enableTransmit(PIN_433_EMITTER);
  rf433write.setPulseLength(320);
  rf433write.setProtocol(2);
  rf433write.setRepeatTransmit(5);

  // Tachometer
  pinMode(PIN_TACHO, INPUT);

  // Standby transistor
  pinMode(PIN_STANDBY, OUTPUT);

  #if SERIAL == 1
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  #endif
  
  lnk.init();
}

void loop()
{

  unsigned long runTime = millis();
  
  #if DEBUG == 1
  Serial.println("Awake!");
  #endif
  digitalWrite(PIN_STANDBY, HIGH);

  #if DEBUG == 1
  Serial.print("Read tachometer for "); Serial.print(TACHO_RECORD_DELAY / 1000.0); Serial.println(" seconds...");
  #endif
  execute_tachometer();

  #if DEBUG == 1
  Serial.println("Read DH11");
  #endif
  execute_temperature();

  #if DEBUG == 1
  Serial.println("Send data to RF433...");
  #endif
  execute_sending();

  // Sleep
  runTime = millis() - runTime;
  #if DEBUG == 1
  Serial.print("Run time was "); Serial.print(runTime / 1000.0); Serial.println(" seconds"); 
  Serial.print("Going to sleep for "); Serial.print((STANDBY_DELAY - runTime) / 1000.0); Serial.println(" seconds...");
  #endif
  digitalWrite(PIN_STANDBY, LOW);
  delay(STANDBY_DELAY - runTime);

}

void execute_tachometer() {
  unsigned long stopTime = millis() + TACHO_RECORD_DELAY;
  unsigned long lastTachoUpdate = 0; // Time stamp for tachometer sensor update
  unsigned int lastTachoValue = HIGH;
  unsigned long rpt = 0;
  while (stopTime > millis()) {
    if (digitalRead(PIN_TACHO) != lastTachoValue) {
      lastTachoValue = lastTachoValue > LOW ? LOW : HIGH;
      if (lastTachoValue == LOW && millis() - lastTachoUpdate > TACHO_SENSIBILITY_DELAY)
      {
        lastTachoUpdate = millis();
        rpt += 1;
      }
    }
  }
  // Convert to RPM
  lnk.setValue(1, 60000.0 / TACHO_RECORD_DELAY * rpt);
}

void execute_temperature() {
  // DHT Sensor
  int chk = DHT.read11(PIN_DHT);
  if (chk != DHTLIB_OK)
  {
    #if DEBUG == 1
    Serial.println("DHT Failure!");
    #endif
  }
  else
  {
    lnk.setValue(2, DHT.temperature);
    lnk.setValue(3, DHT.humidity);
  }
}

void execute_sending() {
    sendRf433(lnk.getEncoded32(3));
    delay(RF_EMIT_INTERVAL);
    sendRf433(lnk.getEncoded32(2));
    delay(RF_EMIT_INTERVAL);
    sendRf433(lnk.getEncoded32(1));
}

