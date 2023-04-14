/*
  Designed for arduino Nano
  Using RA-01 module (SX1278)
  Require DHT library
*/
#include "DHT.h"

// based on DTH library example
#define DHTPIN 2     // Digital pin connected to the DHT sensor

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

#define RL_SX1278
#include <RadioLink.h>

//--- Nano/RA-01 SPI pin wiring ---
// SCK  : 13
// MISO : 12
// MOSI : 11
// NSS  : 10
// DIO0 : 2
// NRST : 9

RadioLinkClass RLcomm;

#define SENSOR_ID       10
#define CHILD_TEMPERATURE 1
#define CHILD_HUMIDITY    2

//#define DEBUG_SERIAL

void setup()
{

#ifdef DEBUG_SERIAL
  Serial.begin(57600);
  while (!Serial.availableForWrite());
  Serial.println(F("--- serial debug ON --- "));
#endif

  // initialize SX1278 with default settings
  if (!RLcomm.begin(433E6, NULL, NULL, 15)) {
#ifdef DEBUG_SERIAL
    Serial.println(F("LORA ERROR"));
#endif
  }
  dht.begin();
}

void loop()
{
  delay(60000); // 1 min

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

#ifdef DEBUG_SERIAL
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.println("°C");
#endif

  RLcomm.publishFloat(0, SENSOR_ID, CHILD_TEMPERATURE, S_NUMERICSENSOR, t * 10, 10, 1); // 0.1 precision
  RLcomm.publishNum(0, SENSOR_ID, CHILD_HUMIDITY, S_NUMERICSENSOR, h); // send as interger 0..100%
}