/*

  // ATMEL ATTINY84 / ARDUINO
  //
  //                           +-\/-+
  //                     VCC  1|    |14  GND
  //             (D 10)  PB0  2|    |13  AREF (D  0)
  //             (D  9)  PB1  3|    |12  PA1  (D  1)
  //                     PB3  4|    |11  PA2  (D  2)
  //  PWM  INT0  (D  8)  PB2  5|    |10  PA3  (D  3)
  //  PWM        (D  7)  PA7  6|    |9   PA4  (D  4)
  //  PWM        (D  6)  PA6  7|    |8   PA5  (D  5)        PWM
  //                           +----+

  PB0 : XTAL1 (if not mounted TX debug is possible)
  PB1 : XTAL2 
  PB3 : External reset
  PB2 : DIO0
  PA7 : Relay out
  PA6 : MISO
  PA5 : MOSI
  PA4 : SCK
  PA3 : NSS
  PA2 : RST
  PA1 : IO1 (button)
  PA0 : IO0 (free for another IO like debug LED, line 114 in RL_SX1278.cpp)

*/
#define RL_SX1278
#include <RadioLink.h>

RadioLinkClass RLcomm;

//#define DEBUG_SERIAL
#ifdef DEBUG_SERIAL
#include "src/SendOnlySoftwareSerial.h"
#define DEBUG(x) Serial.print(x);
#define DEBUGln(x) Serial.println(x);
SendOnlySoftwareSerial Serial(10,false);
#else
#define DEBUG(x)
#define DEBUGln(x)
#endif

#define VCC 3.3

#define SENSOR_ID       04
#define CHILD_BTN_ID    1
#define CHILD_RELAY_ID  2

#define PIN_RELAY       7
#define PIN_BUTTON      1

int relais = 0;
uint32_t tempoRelais = 0;

bool btnPressed = false;
uint32_t curTime, oldTime = 0;
uint32_t oldBtnTime = 0;

void setup() {
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RELAY, OUTPUT);

#ifdef DEBUG_SERIAL
  Serial.begin(19200);
#endif
  if (RLcomm.begin(433E6, onLoRaReceive, NULL, 17)) {
    //
    DEBUGln("LoRa OK");
  } else {
    //
    DEBUGln("LoRa Error");
  }
}

void onLoRaReceive(rl_packet_t p) {
  DEBUG("IN d:");
  DEBUG(p.destinationID);
  DEBUG(" s:");
  DEBUG(p.senderID);
  DEBUG(" c:");
  DEBUG(p.childID);
  if (p.destinationID == SENSOR_ID) {
    if (p.childID == CHILD_RELAY_ID) {
      DEBUG(" v:");
      DEBUG(p.data.num.value);
      int duree = p.data.num.value;
      if (duree < 100) {
        duree = 100;
      } else if (duree > 1000) {
        duree = 1000;
      }
      tempoRelais = curTime + duree;
      digitalWrite(PIN_RELAY, HIGH);
    }
  }
  DEBUGln(".");
}

void loop() {
  // receive activation switch
  curTime = millis();

  if (curTime > oldTime + 1000) {
    oldTime = curTime;
  }

  if ((tempoRelais > 0) && (curTime > tempoRelais)) {
    tempoRelais = 0;
    digitalWrite(PIN_RELAY, LOW);
  }

  // Gestion du bouton Manuel de temporisation
  bool btnDown = (digitalRead(PIN_BUTTON) == LOW);
  if (btnDown) {
    if (oldBtnTime > 0) {
      if ((btnPressed == false) && (curTime > oldBtnTime + 50)) {
        btnPressed = true;
      }
    } else {
      // antirebond
      oldBtnTime = curTime;
    }
  } else {
    if (btnPressed) {
      int code = 1;
      if ((curTime - oldBtnTime) > 1000) {
        code = 2;
      }
      if ((curTime - oldBtnTime) > 3000) {
        code = 3;
      }
      RLcomm.publishNum(0, SENSOR_ID, CHILD_BTN_ID, S_TRIGGER, code);
    }
    oldBtnTime = 0;
    btnPressed = false;
  }
}