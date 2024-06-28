/*
	By Matiere&lumiere
	https://github.com/PM04290/

	Version 1.1 : initial
	Version 2.0 : packet release (config)
	
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
	const long RL_DEFAULT_SPI_FREQUENCY = 8E6; // not used
	const uint8_t RL_DEFAULT_SS_PIN     = 3;
	const uint8_t RL_DEFAULT_RESET_PIN  = 2;
	const uint8_t RL_DEFAULT_DINT_PIN   = 8;
#elif defined(__AVR_ATtiny1616__) || defined(__AVR_ATtiny3216__)
	const uint8_t RL_NEW_MISO           = 0;
	const uint8_t RL_NEW_MOSI           = 0;
	const uint8_t RL_NEW_SCLK           = 0;
	const uint8_t RL_NEW_SS             = 0;
	SPIClass* RL_DEFAULT_SPI            = &SPI;
	const long RL_DEFAULT_SPI_FREQUENCY = 8E6; // not used
	const uint8_t RL_DEFAULT_SS_PIN     = 0;
	const uint8_t RL_DEFAULT_RESET_PIN  = 1;
	const uint8_t RL_DEFAULT_DINT_PIN   = 13;
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
	#elif defined(ARDUINO_ESP32_POE_ISO) || defined(ARDUINO_ESP32_POE)
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
#elif defined(__AVR_ATmega32U4__)
	const uint8_t RL_NEW_MISO           = 0;
	const uint8_t RL_NEW_MOSI           = 0;
	const uint8_t RL_NEW_SCLK           = 0;
	const uint8_t RL_NEW_SS             = 0;
	SPIClass* RL_DEFAULT_SPI            = &SPI;
	const long RL_DEFAULT_SPI_FREQUENCY = 8E6;
	const uint8_t RL_DEFAULT_SS_PIN     = 10;
	const uint8_t RL_DEFAULT_RESET_PIN  = 9;
	const uint8_t RL_DEFAULT_DINT_PIN   = 7;
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
uint8_t _TXdone = 0;

ISR_PREFIX void RLhelper_base::onDintRise()
{
  RLhelper.handleDintRise();
}

bool RadioLinkClass::begin(long frequency, void(*callbackR)(uint8_t,rl_packet_t*), void(*callbackT)(), int TxLevel, uint8_t radioDistance)
{
  _onRxDone = callbackR;
  _onTxDone = callbackT;
  if (RLhelper.begin(frequency))
  {
	RLhelper.onInternalRxDone(onRxDone);
	RLhelper.onInternalTxDone(onTxDone);
	RLhelper.setTxPower(TxLevel);
	setRadioDistance(radioDistance);
	
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
  if ((size_t)packetSize > sizeof(currentPacket)) packetSize = sizeof(currentPacket);
  byte* raw = (byte*)&currentPacket;
  RLhelper.read(raw, packetSize);
  uint8_t crc = CRC_PRELOAD;
  for (uint8_t i = 0; i < packetSize-1; i++)
  {
	  crc += raw[i];
  }
  if (crc != currentPacket.crc)
  {
	  return;
  }
  if (_onRxDone)
  {
	_onRxDone(packetSize, &currentPacket);
  }
}

void RadioLinkClass::onTxDone()
{
  _TXdone = true;
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

void RadioLinkClass::setRadioDistance(uint8_t n) // 0 .. 3
{
	if (n == 3) {
		RLhelper.setSpreadingFactor(11);   // 7 .. 12
		RLhelper.setSignalBandwidth(6);    // 0 .. 9
		RLhelper.setCodingRate4(4);        // 1 .. 4
		return;
	}
	if (n == 2) {
		RLhelper.setSpreadingFactor(10);   // 7 .. 12
		RLhelper.setSignalBandwidth(6);    // 0 .. 9
		RLhelper.setCodingRate4(3);        // 1 .. 4
		return;
	}
	if (n == 1) {
		RLhelper.setSpreadingFactor(9);   // 7 .. 12
		RLhelper.setSignalBandwidth(7);   // 0 .. 9
		RLhelper.setCodingRate4(2);       // 1 .. 4
		return;
	}
	// Default : SF=7, BW = 7, CR=1
	RLhelper.setSpreadingFactor(7);   // 7 .. 12
	RLhelper.setSignalBandwidth(8);   // 0 .. 9
	RLhelper.setCodingRate4(1);       // 1 .. 4
}

void RadioLinkClass::publishPaquet(rl_packets* packet, byte version)
{
  if (version == 0 || version == RL_CURRENT_VERSION)
  {
    packet->current.crc = CRC_PRELOAD;
    for (uint8_t i = 0; i < sizeof(rl_packets)-1; i++)
    {
      packet->current.crc += ((uint8_t *)&packet->current)[i];
    }
	_TXdone = false;
    RLhelper.write((uint8_t *)packet, sizeof(packet->current));
    while (_waitOnTx && !_TXdone) { delay(2); };
	return;
  }
}

void RadioLinkClass::publishBool(byte destinationid, byte senderid, byte childid, const uint8_t value, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType = (E_BINARYSENSOR << 3) + D_BOOL;
		  packet.current.data.num.value = value;
	  break;
  }
  publishPaquet(&packet, version);
}

void RadioLinkClass::publishNum(byte destinationid, byte senderid, byte childid, const long value, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType = (E_NUMERICSENSOR << 3) + D_NUM;
		  packet.current.data.num.value = value;
		  packet.current.data.num.divider = 1;
	  break;
  }
  publishPaquet(&packet, version);
}

void RadioLinkClass::publishFloat(byte destinationid, byte senderid, byte childid, const long value, const int divider, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (E_NUMERICSENSOR << 3) + D_FLOAT;
		  packet.current.data.num.value = value;
		  packet.current.data.num.divider = divider;
	  break;
  }
  publishPaquet(&packet, version);
}

void RadioLinkClass::publishSwitch(byte destinationid, byte senderid, byte childid, const uint8_t value, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType = (E_SWITCH << 3) + D_NUM;
		  packet.current.data.num.value = value;
		  packet.current.data.num.divider = 1;
	  break;
  }
  publishPaquet(&packet, version);
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
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (E_TEXTSENSOR << 3) + D_TEXT;
		  strncpy(packet.current.data.text, text, l);
		  if (l < MAX_PACKET_DATA_LEN) {
			packet.current.data.text[l] = 0;
		  }
	  break;
  }
  publishPaquet(&packet, version);
}

void RadioLinkClass::publishTag(byte destinationid, byte senderid, byte childid, const uint32_t tagH, const uint32_t tagL, const byte readerID, const byte readerType, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (E_TAG << 3) + D_TAG;
		  packet.current.data.tag.tagH = tagH;
		  packet.current.data.tag.tagL = tagL;
		  packet.current.data.tag.readerID = readerID;
		  packet.current.data.tag.readerType = readerType;
	  break;
  }
  publishPaquet(&packet, version);
}

void RadioLinkClass::publishRaw(byte destinationid, byte senderid, byte childid, const uint8_t* data, const byte len, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (E_CUSTOM << 3) + D_RAW;
		  for (byte b = 0; b < len && b < MAX_PACKET_DATA_LEN; b++) {
			packet.current.data.rawByte[b] = data[b];
		  }
	  break;
  }
  publishPaquet(&packet, version);
}

void RadioLinkClass::publishLight(byte destinationid, byte senderid, byte childid, uint8_t state, uint8_t brightness, uint16_t temperature, uint8_t red, uint8_t green, uint8_t blue, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (E_LIGHT << 3) + D_RAW;
		  packet.current.data.light.state = state;
		  packet.current.data.light.brightness = brightness;
		  packet.current.data.light.temperature = temperature;
		  packet.current.data.light.red = red;
		  packet.current.data.light.green = green;
		  packet.current.data.light.blue = blue;
	  break;
  }
  publishPaquet(&packet, version);
}

void RadioLinkClass::publishCover(byte destinationid, byte senderid, byte childid, uint8_t command, uint8_t position, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = childid;
		  packet.current.sensordataType =  (E_COVER << 3) + D_RAW;
		  packet.current.data.cover.state = 0;
		  packet.current.data.cover.command = command;
		  packet.current.data.cover.position = position;
	  break;
  }
  publishPaquet(&packet, version);
}

void RadioLinkClass::publishConfig(byte destinationid, byte senderid, rl_configs_t* cnf, rl_conf_t cnfIdx, byte version)
{
  rl_packets packet;
  switch (version) {
	  case 1:
	  default:
		  packet.current.destinationID = destinationid;
		  packet.current.senderID = senderid;
		  packet.current.childID = RL_ID_CONFIG;
		  packet.current.sensordataType =  (E_CONFIG << 3) + (cnfIdx&0x7);
		  memcpy(&packet.current.data.configs, cnf, sizeof(rl_configs_t));
	  break;
  }
  publishPaquet(&packet, version);
}
