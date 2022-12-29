/*
  Designed for arduino Nano
  Using RA-01 module (SX1278)
  Require TeleInfo Library
*/
#include <avr/power.h>
#include <SoftwareSerial.h>
#include <TeleInfo.h>

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

#define SENSOR_ID       01
#define CHILD_CONSO     1
#define CHILD_COMPTEUR  2
#define CHILD_IISNT1    3
#define CHILD_IISNT2    4
#define CHILD_IISNT3    5

#define PIN_RX_LINKY    8
#define PIN_TX_LINKY   99 // unused

//#define PIN_STATUS_LED  7

SoftwareSerial TICserial(PIN_RX_LINKY, PIN_TX_LINKY);

TeleInfo teleinfo(&TICserial);

#define AVGSIZE 5
long arrPower[AVGSIZE];
byte idxPower = 0;
bool pwrStart = true;
long counter = 0;
uint32_t curTime;
uint32_t Time1s = 0;
uint32_t ConsoTime;

//#define DEBUG_SERIAL

void setup() {

#ifdef DEBUG_SERIAL
  Serial.begin(57600);
  while (!Serial.availableForWrite());
  Serial.println(F("--- serial debug ON --- "));
#endif

  TICserial.begin(1200);
  teleinfo.begin();

  // initialize SX1278 with default settings
  if (!RLcomm.begin(433E6, NULL, NULL, 15)) {
#ifdef DEBUG_SERIAL
    Serial.println(F("LORA ERROR"));
#endif
  }

  ConsoTime = millis();
}

void loop() {
  uint32_t curTime = millis();

  teleinfo.process();
  if (teleinfo.available()) {
    // close serial to save cpu charge
    TICserial.end();
    long consumption = teleinfo.getLongVal("BASE");
    int power = teleinfo.getLongVal("PAPP");
    int Iinst = teleinfo.getLongVal("IINST");
    int Iinst1 = 0;
    int Iinst2 = 0;
    int Iinst3 = 0;
    if (Iinst == -1) {
      Iinst1 = teleinfo.getLongVal("IINST1");
      Iinst2 = teleinfo.getLongVal("IINST2");
      Iinst3 = teleinfo.getLongVal("IINST3");
    }

#ifdef DEBUG_SERIAL
    Serial.println(F("--- tele info available ---"));
    const char* opTarif = teleinfo.getStringVal("OPTARIF");
    Serial.print(F("Option Tarifaire = "));
    opTarif == NULL ? Serial.println("unknown") : Serial.println(opTarif);
    Serial.print(F("Power = "));
    power < 0 ? Serial.println(F("unknown")) : Serial.println(power);
    Serial.print("Compteur = ");
    consumption < 0 ? Serial.println(F("unknown")) : Serial.println(consumption);
#endif

    if (power > 0)
    {
      // Send power if value exeed 20w difference of agv 5 last values
      arrPower[idxPower++] = power;
      if (idxPower >= AVGSIZE)
      {
        idxPower = 0;
        pwrStart = false; // 1rst array is full
      }
      if (pwrStart == false)
      {
        long avgPower = 0;
        for (byte i = 0; i < AVGSIZE; i++)
        {
          avgPower += arrPower[i];
        }
        avgPower = avgPower / AVGSIZE;

#ifdef DEBUG_SERIAL
        Serial.print(F("Avg Power = "));
        Serial.print(avgPower);
        Serial.print(F(" / "));
        Serial.print(power);
        Serial.print(F(" -> "));
        Serial.println(abs(avgPower - power));
#endif

        // 
        if (abs(avgPower - power) > 20)
        {
          for (byte i = 0; i < AVGSIZE; i++)
          {
            arrPower[i] = power;
          }
          idxPower = 0;
          RLcomm.publishNum(0, SENSOR_ID, CHILD_CONSO, S_NUMERICSENSOR, power);
          if (Iinst >= 0) {
            RLcomm.publishNum(0, SENSOR_ID, CHILD_IISNT1, S_NUMERICSENSOR, Iinst);
          } else {
            RLcomm.publishNum(0, SENSOR_ID, CHILD_IISNT1, S_NUMERICSENSOR, Iinst1);
            RLcomm.publishNum(0, SENSOR_ID, CHILD_IISNT2, S_NUMERICSENSOR, Iinst2);
            RLcomm.publishNum(0, SENSOR_ID, CHILD_IISNT3, S_NUMERICSENSOR, Iinst3);
          }
        }
      }
    }

    // Send total counter every 10 minutes sortie en KWh
    if ((curTime > ConsoTime + 600000) || (curTime < ConsoTime)) {
      if (consumption > 1) {
        RLcomm.publishFloat(0, SENSOR_ID, CHILD_COMPTEUR, S_NUMERICSENSOR, consumption, 1000, 1);
      }
      ConsoTime = curTime;
    }

    // restore serial input
    TICserial.begin(1200);
    teleinfo.begin();
  }

}