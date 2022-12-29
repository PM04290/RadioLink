/*
	By Matiere&lumiere
	https://github.com/PM04290/

	Version 0.1 : initial
	
	Supported hardware radio:
	- SX1278 (Ra-01)
*/
#ifndef RADIOLINK_H_
#define RADIOLINK_H_


#ifdef RL_SX1278

  #include "RL_SX1278.h"

#elif RL_NRF24

  #include "RL_NRF24.h"

#else

#error Must define radio driver

#endif


typedef enum {
  S_BINARYSENSOR    = 0,
  S_NUMERICSENSOR   = 1,
  S_SWITCH          = 2,
  S_LIGHT           = 3,
  S_COVER           = 4,
  S_FAN             = 5,
  S_HVAC            = 6,
  S_SELECT          = 7,
  S_TRIGGER         = 8,
  S_CUSTOM          = 9,
  S_TAG             = 10
} rl_device_t; // limited to 32

//
typedef enum {
  V_BOOL    = 0,
  V_NUM     = 1,
  V_FLOAT   = 2,
  V_TEXT    = 3,
  V_TAG     = 4,
  V_RAW     = 5
} rl_data_t; // limited to 7

#define MAX_PACKET_DATA_LEN 12
typedef struct __attribute__((packed)) {
  uint8_t destinationID;
  uint8_t senderID;
  uint8_t childID;
  uint8_t sensordataType; // (rl_sensor_t << 3) + rl_data_t
  union {
    struct {
      int32_t value;
      uint16_t divider;
      uint8_t precision;
      char unit[5];
    } num;
    char text[MAX_PACKET_DATA_LEN];
    uint8_t rawByte[MAX_PACKET_DATA_LEN];
    uint16_t rawWord[MAX_PACKET_DATA_LEN / sizeof(uint16_t)];
    struct {
      uint32_t tagH;
      uint32_t tagL;
      uint16_t readerID;
      uint16_t readerType;
    } tag;
  } data;
} rl_packet_t;

#define RL_PACKET_SIZE sizeof(rl_packet_t)

RadioDriver RLDriver;

rl_packet_t currentPacket;

void (*_onRxDone)(rl_packet_t);
void (*_onTxDone)();

class RadioLinkClass
{
  public:
    explicit RadioLinkClass() {
    }
    bool begin(long frequency, void(*callbackR)(rl_packet_t), void(*callbackT)(), int TxLevel)
    {
      _onRxDone = callbackR;
      _onTxDone = callbackT;
      if (RLDriver.begin(frequency))
      {
        RLDriver.onReceive(onRxDone);
        RLDriver.onTxDone(onTxDone);
        RLDriver.setTxPower(TxLevel);
        RLDriver.receive();
        return true;
      }
      return false;
    }

    static void onRxDone(int packetSize)
    {
      if (packetSize == 0) return;
      byte p = 0;
      byte* raw = (byte*)&currentPacket;
      while (RLDriver.available() && p < RL_PACKET_SIZE)
      {
        byte b = RLDriver.read();
        raw[p++] = b;
      }
      if (_onRxDone)
      {
        _onRxDone(currentPacket);
      }
    }

    static void onTxDone()
    {
      RLDriver.receive();
      if (_onTxDone)
      {
        _onTxDone();
      }
    }

    void publishPaquet(rl_packet_t packet)
    {
      RLDriver.idle();
      RLDriver.beginPacket();
      RLDriver.write((uint8_t *)&packet, sizeof(packet));
      RLDriver.endPacket(true);
    }

    void publishNum(byte destinationid, byte senderid, byte childid, rl_device_t sensortype, const long value)
    {
      rl_packet_t packet;
      packet.destinationID = destinationid;
      packet.senderID = senderid;
      packet.childID = childid;
      packet.sensordataType = (sensortype << 3) + V_NUM;
      packet.data.num.value = value;
      publishPaquet(packet);
    }

    void publishFloat(byte destinationid, byte senderid, byte childid, rl_device_t sensortype, const long value, const int divider, const byte precision)
    {
      rl_packet_t packet;
      packet.destinationID = destinationid;
      packet.senderID = senderid;
      packet.childID = childid;
      packet.sensordataType =  (sensortype << 3) + V_FLOAT;
      packet.data.num.value = value;
      packet.data.num.divider = divider;
      packet.data.num.precision = precision;
      publishPaquet(packet);
    }

    void publishText(byte destinationid, byte senderid, byte childid, const char* text)
    {
      rl_packet_t packet;
      packet.destinationID = destinationid;
      packet.senderID = senderid;
      packet.childID = childid;
      packet.sensordataType =  (S_CUSTOM << 3) + V_TEXT;
      byte l = strlen(text);
      if (l > MAX_PACKET_DATA_LEN) {
        l = MAX_PACKET_DATA_LEN;
      }
      strncpy(packet.data.text, text, l);
      if (l < MAX_PACKET_DATA_LEN) {
        packet.data.text[l] = 0;
      }
      publishPaquet(packet);
    }

    void publishTag(byte destinationid, byte senderid, byte childid, const uint32_t tagH, const uint32_t tagL, const byte readerID, const byte readerType)
    {
      rl_packet_t packet;
      packet.destinationID = destinationid;
      packet.senderID = senderid;
      packet.childID = childid;
      packet.sensordataType =  (S_TAG << 3) + V_TAG;
      packet.data.tag.tagH = tagH;
      packet.data.tag.tagL = tagL;
      packet.data.tag.readerID = readerID;
      packet.data.tag.readerType = readerType;
      publishPaquet(packet);
    }

    void publishRaw(byte destinationid, byte senderid, byte childid, const uint8_t* data, const byte len)
    {
      rl_packet_t packet;
      packet.destinationID = destinationid;
      packet.senderID = senderid;
      packet.childID = childid;
      packet.sensordataType =  (S_CUSTOM << 3) + V_RAW;
      for (byte b = 0; b < len && b < MAX_PACKET_DATA_LEN; b++) {
        packet.data.rawByte[b] = data[b];
      }
      publishPaquet(packet);
    }
};

#endif
