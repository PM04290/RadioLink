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
	#if defined(ARDUINO_LOLIN_S2_MINI)
		// defining NEW spi pin
		//#define RL_DEFAULT_SPI         not defined for specific SPI
		const uint8_t RL_NEW_MISO           = 9;
		const uint8_t RL_NEW_MOSI           = 11;
		const uint8_t RL_NEW_SCLK           = 7;
		const uint8_t RL_NEW_SS             = 5;
		SPIClass* RL_DEFAULT_SPI            = new SPIClass(HSPI);
		const long RL_DEFAULT_SPI_FREQUENCY = 8E6;
		const uint8_t RL_DEFAULT_SS_PIN     =  5;
		const uint8_t RL_DEFAULT_RESET_PIN  = 12;
		const uint8_t RL_DEFAULT_DINT_PIN   =  3;
	#elif defined(ARDUINO_ESP32_POE_ISO)
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
		const uint8_t RL_NEW_SS             = 12;
		SPIClass* RL_DEFAULT_SPI            = new SPIClass(HSPI);
		const long RL_DEFAULT_SPI_FREQUENCY = 8E6;
		const uint8_t RL_DEFAULT_SS_PIN     =  12;
		const uint8_t RL_DEFAULT_RESET_PIN  =  4;
		const uint8_t RL_DEFAULT_DINT_PIN   =  35;
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
#elif defined(ESP32S2)
	const uint8_t RL_NEW_MISO           = 0;
	const uint8_t RL_NEW_MOSI           = 0;
	const uint8_t RL_NEW_SCLK           = 0;
	const uint8_t RL_NEW_SS             = 0;
	SPIClass* RL_DEFAULT_SPI            = &SPI;
	const long RL_DEFAULT_SPI_FREQUENCY = 8E6;
	const uint8_t RL_DEFAULT_SS_PIN     = 12;
	const uint8_t RL_DEFAULT_RESET_PIN  =  4;
	const uint8_t RL_DEFAULT_DINT_PIN   = 35;
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
	//RLhelper.setSpreadingFactor(9);
	//RLhelper.setSignalBandwidth(6);
	//RLhelper.setCodingRate4(7);
	RLhelper.receiveMode();
	return true;
  }
  return false;
}

void RadioLinkClass::end()
{
	RLhelper.end();
}

void RadioLinkClass::onRxDone(int packetSize)
{
  if (packetSize == 0) return;
  if (packetSize > sizeof(currentPacket)) packetSize = sizeof(currentPacket);
  byte* raw = (byte*)&currentPacket;
  RLhelper.read(raw, packetSize);
  // CRC check since V4
  if (packetSize == RL_PACKETV4_SIZE) {
	  uint8_t crc = CRC_PRELOAD;
	  for (uint8_t i = 0; i < packetSize-1; i++)
	  {
		  crc += raw[i];
	  }
	  if (crc != currentPacket.crc)
	  {
		  return;
	  }
  }
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

int RadioLinkClass::lqi() {
  return RLhelper.lqi();
}

void RadioLinkClass::sleep() {
  return RLhelper.sleep();
}

void RadioLinkClass::idle() {
  return RLhelper.idle();
}

void RadioLinkClass::setWaitOnTx(bool state)
{
	_waitOnTx = state;
}


void RadioLinkClass::publishPaquetV1(rl_packetV1_t packet)
{
  RLhelper.write((uint8_t *)&packet, sizeof(packet));
}

void RadioLinkClass::publishPaquetV2(rl_packetV2_t packet)
{
  RLhelper.write((uint8_t *)&packet, sizeof(packet));
}

void RadioLinkClass::publishPaquetV3(rl_packetV3_t packet)
{
  RLhelper.write((uint8_t *)&packet, sizeof(packet));
}

void RadioLinkClass::publishPaquet(rl_packets packet, byte version)
{
  if (version == 0 || version == RL_CURRENT_VERSION)
  {
    packet.current.crc = CRC_PRELOAD;
    for (uint8_t i = 0; i < sizeof(packet)-1; i++)
    {
      packet.current.crc += ((uint8_t *)&packet.current)[i];
    }
    RLhelper.write((uint8_t *)&packet, sizeof(packet.current));
    while (_waitOnTx && RLhelper.isTransmitting()) { delay(2); };
	return;
  }
  if (version == 1)
  {
    publishPaquetV1(packet.v1);
    return;
  }
  if (version == 2)
  {
    publishPaquetV2(packet.v2);
    return;
  }
  if (version == 3)
  {
    publishPaquetV3(packet.v3);
    return;
  }
}

void RadioLinkClass::publishNum(byte destinationid, byte senderid, byte childid, rl_device_t sensortype, const long value, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
		  packet.v1.destinationID = destinationid;
		  packet.v1.senderID = senderid;
		  packet.v1.childID = childid;
		  packet.v1.sensordataType = (sensortype << 3) + V_NUM;
		  packet.v1.data.num.value = value;
	  break;
	  case 2:
		  packet.v2.destinationID = destinationid;
		  packet.v2.senderID = senderid;
		  packet.v2.childID = childid;
		  packet.v2.sensordataType = (sensortype << 3) + V_NUM;
		  packet.v2.data.num.value = value;
	  break;
	  case 3:
		  packet.v3.destinationID = destinationid;
		  packet.v3.senderID = senderid;
		  packet.v3.childID = childid;
		  packet.v3.sensordataType = (sensortype << 3) + V_NUM;
		  packet.v3.data.num.value = value;
	  break;
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType = (sensortype << 3) + V_NUM;
		  packet.current.data.num.value = value;
	  break;
  }
  publishPaquet(packet, version);
}

void RadioLinkClass::publishFloat(byte destinationid, byte senderid, byte childid, rl_device_t sensortype, const long value, const int divider, const byte precision, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
		  packet.v1.destinationID = destinationid;
		  packet.v1.senderID = senderid;
		  packet.v1.childID = childid;
		  packet.v1.sensordataType =  (sensortype << 3) + V_FLOAT;
		  packet.v1.data.num.value = value;
		  packet.v1.data.num.divider = divider;
		  packet.v1.data.num.precision = precision;
	  break;
	  case 2:
		  packet.v2.destinationID = destinationid;
		  packet.v2.senderID = senderid;
		  packet.v2.childID = childid;
		  packet.v2.sensordataType =  (sensortype << 3) + V_FLOAT;
		  packet.v2.data.num.value = value;
		  packet.v2.data.num.divider = divider;
		  packet.v2.data.num.precision = precision;
	  break;
	  case 3:
		  packet.v3.destinationID = destinationid;
		  packet.v3.senderID = senderid;
		  packet.v3.childID = childid;
		  packet.v3.sensordataType =  (sensortype << 3) + V_FLOAT;
		  packet.v3.data.num.value = value;
		  packet.v3.data.num.divider = divider;
		  packet.v3.data.num.precision = precision;
	  break;
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (sensortype << 3) + V_FLOAT;
		  packet.current.data.num.value = value;
		  packet.current.data.num.divider = divider;
		  packet.current.data.num.precision = precision;
	  break;
  }
  publishPaquet(packet, version);
}

void RadioLinkClass::publishText(byte destinationid, byte senderid, byte childid, const char* text, byte version)
{
  rl_packets packet;
  byte l = strlen(text);
  if (l > MAX_PACKET_DATA_LEN) {
	l = MAX_PACKET_DATA_LEN;
  }
  switch (version) {
	  case 1:
		  packet.v1.destinationID = destinationid;
		  packet.v1.senderID = senderid;
		  packet.v1.childID = childid;
		  packet.v1.sensordataType =  (SV1_INPUTTEXT << 3) + V_TEXT;
		  strncpy(packet.v1.data.text, text, l);
		  if (l < MAX_PACKET_DATA_LEN) {
			packet.v1.data.text[l] = 0;
		  }
	  break;
	  case 2:
		  packet.v2.destinationID = destinationid;
		  packet.v2.senderID = senderid;
		  packet.v2.childID = childid;
		  packet.v2.sensordataType =  (SV2_TEXTSENSOR << 3) + V_TEXT;
		  strncpy(packet.v2.data.text, text, l);
		  if (l < MAX_PACKET_DATA_LEN) {
			packet.v2.data.text[l] = 0;
		  }
	  break;
	  case 3:
		  packet.v3.destinationID = destinationid;
		  packet.v3.senderID = senderid;
		  packet.v3.childID = childid;
		  packet.v3.sensordataType =  (S_TEXTSENSOR << 3) + V_TEXT;
		  strncpy(packet.v3.data.text, text, l);
		  if (l < MAX_PACKET_DATA_LEN) {
			packet.v3.data.text[l] = 0;
		  }
	  break;
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (S_TEXTSENSOR << 3) + V_TEXT;
		  strncpy(packet.current.data.text, text, l);
		  if (l < MAX_PACKET_DATA_LEN) {
			packet.current.data.text[l] = 0;
		  }
	  break;
  }
  publishPaquet(packet, version);
}

void RadioLinkClass::publishTag(byte destinationid, byte senderid, byte childid, const uint32_t tagH, const uint32_t tagL, const byte readerID, const byte readerType, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
		  packet.v1.destinationID = destinationid;
		  packet.v1.senderID = senderid;
		  packet.v1.childID = childid;
		  packet.v1.sensordataType =  (SV1_TAG << 3) + V_TAG;
		  packet.v1.data.tag.tagH = tagH;
		  packet.v1.data.tag.tagL = tagL;
	  break;
	  case 2:
		  packet.v2.destinationID = destinationid;
		  packet.v2.senderID = senderid;
		  packet.v2.childID = childid;
		  packet.v2.sensordataType =  (SV2_TAG << 3) + V_TAG;
		  packet.v2.data.tag.tagH = tagH;
		  packet.v2.data.tag.tagL = tagL;
		  packet.v2.data.tag.readerID = readerID;
		  packet.v2.data.tag.readerType = readerType;
	  break;
	  case 3:
		  packet.v3.destinationID = destinationid;
		  packet.v3.senderID = senderid;
		  packet.v3.childID = childid;
		  packet.v3.sensordataType =  (S_TAG << 3) + V_TAG;
		  packet.v3.data.tag.tagH = tagH;
		  packet.v3.data.tag.tagL = tagL;
		  packet.v3.data.tag.readerID = readerID;
		  packet.v3.data.tag.readerType = readerType;
	  break;
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (S_TAG << 3) + V_TAG;
		  packet.current.data.tag.tagH = tagH;
		  packet.current.data.tag.tagL = tagL;
		  packet.current.data.tag.readerID = readerID;
		  packet.current.data.tag.readerType = readerType;
	  break;
  }
  publishPaquet(packet, version);
}

void RadioLinkClass::publishRaw(byte destinationid, byte senderid, byte childid, const uint8_t* data, const byte len, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
		  packet.v1.destinationID = destinationid;
		  packet.v1.senderID = senderid;
		  packet.v1.childID = childid;
		  packet.v1.sensordataType =  (SV1_CUSTOM << 3) + V_RAW;
		  for (byte b = 0; b < len && b < MAX_PACKET_DATA_LEN_V1; b++) {
			packet.v1.data.rawByte[b] = data[b];
		  }
	  break;
	  case 2:
		  packet.v2.destinationID = destinationid;
		  packet.v2.senderID = senderid;
		  packet.v2.childID = childid;
		  packet.v2.sensordataType =  (SV2_CUSTOM << 3) + V_RAW;
		  for (byte b = 0; b < len && b < MAX_PACKET_DATA_LEN_V2; b++) {
			packet.v2.data.rawByte[b] = data[b];
		  }
	  break;
	  case 3:
		  packet.v3.destinationID = destinationid;
		  packet.v3.senderID = senderid;
		  packet.v3.childID = childid;
		  packet.v3.sensordataType =  (S_CUSTOM << 3) + V_RAW;
		  for (byte b = 0; b < len && b < MAX_PACKET_DATA_LEN_V3; b++) {
			packet.v3.data.rawByte[b] = data[b];
		  }
	  break;
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (S_CUSTOM << 3) + V_RAW;
		  for (byte b = 0; b < len && b < MAX_PACKET_DATA_LEN; b++) {
			packet.current.data.rawByte[b] = data[b];
		  }
	  break;
  }
  publishPaquet(packet, version);
}

void RadioLinkClass::publishLight(byte destinationid, byte senderid, byte childid, uint8_t state, uint8_t brightness, uint16_t temperature, uint8_t red, uint8_t green, uint8_t blue, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	    return;
	  break;
	  case 2:
	    return;
	  break;
	  case 3:
		  packet.v3.destinationID = destinationid;
		  packet.v3.senderID = senderid;
		  packet.v3.childID = childid;
		  packet.v3.sensordataType =  (S_LIGHT << 3) + V_RAW;
		  packet.v3.data.light.state = state;
		  packet.v3.data.light.brightness = brightness;
		  packet.v3.data.light.temperature = temperature;
		  packet.v3.data.light.red = red;
		  packet.v3.data.light.green = green;
		  packet.v3.data.light.blue = blue;
	  break;
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (S_LIGHT << 3) + V_RAW;
		  packet.current.data.light.state = state;
		  packet.current.data.light.brightness = brightness;
		  packet.current.data.light.temperature = temperature;
		  packet.current.data.light.red = red;
		  packet.current.data.light.green = green;
		  packet.current.data.light.blue = blue;
	  break;
  }
  publishPaquet(packet, version);
}

void RadioLinkClass::publishCover(byte destinationid, byte senderid, byte childid, uint8_t command, uint8_t position, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	    return;
	  break;
	  case 2:
	    return;
	  break;
	  case 3:
		  packet.v3.destinationID = destinationid;
		  packet.v3.senderID = senderid;
		  packet.v3.childID = childid;
		  packet.v3.sensordataType =  (S_COVER << 3) + V_RAW;
		  packet.v3.data.cover.state = 0;
		  packet.v3.data.cover.command = command;
		  packet.v3.data.cover.position = position;
	  break;
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (S_COVER << 3) + V_RAW;
		  packet.current.data.cover.state = 0;
		  packet.current.data.cover.command = command;
		  packet.current.data.cover.position = position;
	  break;
  }
  publishPaquet(packet, version);
}

void RadioLinkClass::publishConfig(byte destinationid, byte senderid, byte childid, const uint8_t* data, const byte len, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	    return;
	  break;
	  case 2:
	    return;
	  break;
	  case 3:
		  packet.v3.destinationID = destinationid;
		  packet.v3.senderID = senderid;
		  packet.v3.childID = childid;
		  packet.v3.sensordataType =  (S_CONFIG << 3) + V_RAW;
		  for (byte b = 0; b < len && b < MAX_PACKET_DATA_LEN_V3; b++) {
			packet.v3.data.rawByte[b] = data[b];
		  }
	  break;
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (S_CONFIG << 3) + V_RAW;
		  for (byte b = 0; b < len && b < MAX_PACKET_DATA_LEN; b++) {
			packet.current.data.rawByte[b] = data[b];
		  }
	  break;
  }
  publishPaquet(packet, version);
}
