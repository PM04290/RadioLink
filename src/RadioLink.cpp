/*
	By Matiere&lumiere
	https://github.com/PM04290/

	Version 0.1 : initial
	Version 0.2 : add NRF24l01
	
	Supported hardware radio:
	- SX1278 (eg. Ra-01)
	
	pour détection des différents hardware
	https://github.com/houtbrion/detectArduinoHardware/blob/1bed091c85e1a4398ab476a34d14334f22452c06/src/detectArduinoHardware.h
*/
#include <RadioLink.h>


#define RL_SX1278 1
//#define RL_CC1101 1
//#define RL_NRF24 1

#if defined(__AVR_ATtiny84__)
	const uint8_t RL_NEW_MISO           = 0;
	const uint8_t RL_NEW_MOSI           = 0;
	const uint8_t RL_NEW_SCLK           = 0;
	const uint8_t RL_NEW_SS             = 0;
	SPIClass* RL_DEFAULT_SPI            = &SPI;
	const long RL_DEFAULT_SPI_FREQUENCY = 8E6;
	const uint8_t RL_DEFAULT_SS_PIN     = 3;
	const uint8_t RL_DEFAULT_RESET_PIN  = 2;
	const uint8_t RL_DEFAULT_DINT_PIN   = 8;
#elif defined(ESP32)
	#if defined(ARDUINO_ESP32_POE_ISO)
		// defining NEW spi pin
		//#define RL_DEFAULT_SPI         not defined for specific SPI
		const uint8_t RL_NEW_MISO           = 15;
		const uint8_t RL_NEW_MOSI           = 2;
		const uint8_t RL_NEW_SCLK           = 14;
		const uint8_t RL_NEW_SS             = 5;
		SPIClass* RL_DEFAULT_SPI            = new SPIClass(HSPI);
		const long RL_DEFAULT_SPI_FREQUENCY = 8E6;
		const uint8_t RL_DEFAULT_SS_PIN     =  5;
		const uint8_t RL_DEFAULT_RESET_PIN  =  4;
		const uint8_t RL_DEFAULT_DINT_PIN   =  36;
	#elif defined(ARDUINO_WT32_ETH01)
		// defining NEW spi pin
		//#define RL_DEFAULT_SPI         not defined for specific SPI
		const uint8_t RL_NEW_MISO           = 15;
		const uint8_t RL_NEW_MOSI           = 2;
		const uint8_t RL_NEW_SCLK           = 14;
		const uint8_t RL_NEW_SS             = 5;
		SPIClass* RL_DEFAULT_SPI            = new SPIClass(HSPI);
		const long RL_DEFAULT_SPI_FREQUENCY = 8E6;
		const uint8_t RL_DEFAULT_SS_PIN     =  5;
		const uint8_t RL_DEFAULT_RESET_PIN  =  4;
		const uint8_t RL_DEFAULT_DINT_PIN   =  36;
	#else
		const uint8_t RL_NEW_MISO           = 0;
		const uint8_t RL_NEW_MOSI           = 0;
		const uint8_t RL_NEW_SCLK           = 0;
		const uint8_t RL_NEW_SS             = 0;
		SPIClass* RL_DEFAULT_SPI            = &SPI;
		const long RL_DEFAULT_SPI_FREQUENCY = 8E6;
		const uint8_t RL_DEFAULT_SS_PIN     =  5;
		const uint8_t RL_DEFAULT_RESET_PIN  =  4;
		const uint8_t RL_DEFAULT_DINT_PIN   =  2;
	#endif
#else
	const uint8_t RL_NEW_MISO           = 0;
	const uint8_t RL_NEW_MOSI           = 0;
	const uint8_t RL_NEW_SCLK           = 0;
	const uint8_t RL_NEW_SS             = 0;
	SPIClass* RL_DEFAULT_SPI            = &SPI;
	const long RL_DEFAULT_SPI_FREQUENCY = 8E6;
	const uint8_t RL_DEFAULT_SS_PIN     = 10;
	const uint8_t RL_DEFAULT_RESET_PIN  = 9;
	const uint8_t RL_DEFAULT_DINT_PIN   = 2;
#endif

#if defined(RL_SX1278)
	#include "helpers/SX1278/RL_SX1278.h"
	#define RADIOHELPER RLhelper_SX1278
#else
	#error Must define radio driver
#endif

#if (ESP8266 || ESP32)
    #define ISR_PREFIX ICACHE_RAM_ATTR
#else
    #define ISR_PREFIX
#endif

RADIOHELPER RLhelper;

static rl_packet_t currentPacket;

void (*_onRxDone)(uint8_t,rl_packet_t*);
void (*_onTxDone)();

ISR_PREFIX void RLhelper_base::onDintRise()
{
  RLhelper.handleDintRise();
}

bool RadioLinkClass::begin(long frequency, void(*callbackR)(uint8_t,rl_packet_t*), void(*callbackT)(), int TxLevel)
{
  _onRxDone = callbackR;
  _onTxDone = callbackT;
  if (RLhelper.begin(frequency))
  {
	RLhelper.onInternalRxDone(onRxDone);
	RLhelper.onInternalTxDone(onTxDone);
	RLhelper.setTxPower(TxLevel);
	RLhelper.receiveMode();
	return true;
  }
  return false;
}

void RadioLinkClass::onRxDone(int packetSize)
{
  if (packetSize == 0) return;
  if (packetSize > RL_PACKET_SIZE) packetSize = RL_PACKET_SIZE;
  byte* raw = (byte*)&currentPacket;
  RLhelper.read(raw, packetSize);
  if (_onRxDone)
  {
	_onRxDone(packetSize, &currentPacket);
  }
}

void RadioLinkClass::onTxDone()
{
  RLhelper.receiveMode();
  if (_onTxDone)
  {
	_onTxDone();
  }
}

int  RadioLinkClass::lqi() {
  return RLhelper.lqi();
}

void RadioLinkClass::setWaitOnTx(bool state)
{
	_waitOnTx = state;
}

void RadioLinkClass::publishPaquet(rl_packet_t packet)
{
  RLhelper.write((uint8_t *)&packet, sizeof(packet));
  while (_waitOnTx && RLhelper.isTransmitting());
}

void RadioLinkClass::publishPaquetV1(rl_packetV1_t packet)
{
  RLhelper.write((uint8_t *)&packet, sizeof(packet));
}

void RadioLinkClass::publishPaquetV2(rl_packetV2_t packet)
{
  RLhelper.write((uint8_t *)&packet, sizeof(packet));
}

void RadioLinkClass::publishNum(byte destinationid, byte senderid, byte childid, rl_device_t sensortype, const long value)
{
  rl_packet_t packet;
  packet.destinationID = destinationid;
  packet.senderID = senderid;
  packet.childID = childid;
  packet.sensordataType = (sensortype << 3) + V_NUM;
  packet.data.num.value = value;
  publishPaquet(packet);
}

void RadioLinkClass::publishFloat(byte destinationid, byte senderid, byte childid, rl_device_t sensortype, const long value, const int divider, const byte precision)
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

void RadioLinkClass::publishText(byte destinationid, byte senderid, byte childid, const char* text)
{
  rl_packet_t packet;
  packet.destinationID = destinationid;
  packet.senderID = senderid;
  packet.childID = childid;
  packet.sensordataType =  (S_TEXTSENSOR << 3) + V_TEXT;
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

void RadioLinkClass::publishTag(byte destinationid, byte senderid, byte childid, const uint32_t tagH, const uint32_t tagL, const byte readerID, const byte readerType)
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

void RadioLinkClass::publishRaw(byte destinationid, byte senderid, byte childid, const uint8_t* data, const byte len)
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

void RadioLinkClass::publishLight(byte destinationid, byte senderid, byte childid, uint8_t state, uint8_t brightness, uint16_t temperature, uint8_t red, uint8_t green, uint8_t blue)
{
  rl_packet_t packet;
  packet.destinationID = destinationid;
  packet.senderID = senderid;
  packet.childID = childid;
  packet.sensordataType =  (S_LIGHT << 3) + V_RAW;
  packet.data.light.state = state;
  packet.data.light.brightness = brightness;
  packet.data.light.temperature = temperature;
  packet.data.light.red = red;
  packet.data.light.green = green;
  packet.data.light.blue = blue;
  publishPaquet(packet);
}

void RadioLinkClass::publishCover(byte destinationid, byte senderid, byte childid, uint8_t command, uint8_t position)
{
  rl_packet_t packet;
  packet.destinationID = destinationid;
  packet.senderID = senderid;
  packet.childID = childid;
  packet.sensordataType =  (S_COVER << 3) + V_RAW;
  packet.data.cover.state = 0;
  packet.data.cover.command = command;
  packet.data.cover.position = position;
  publishPaquet(packet);
}

void RadioLinkClass::publishConfig(byte destinationid, byte senderid, byte childid, const uint8_t* data, const byte len)
{
  rl_packet_t packet;
  packet.destinationID = destinationid;
  packet.senderID = senderid;
  packet.childID = childid;
  packet.sensordataType =  (S_CONFIG << 3) + V_RAW;
  for (byte b = 0; b < len && b < MAX_PACKET_DATA_LEN; b++) {
	packet.data.rawByte[b] = data[b];
  }
  publishPaquet(packet);
}
