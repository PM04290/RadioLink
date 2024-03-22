#ifndef RADIOLINK_H_
#define RADIOLINK_H_

#include <Arduino.h>
#include <RLpacket.h>

#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
#include <tinySPI.h>
#define SPIClass tinySPI
#define digitalPinToInterrupt(p)  ((p) == 8 ? 0 : NOT_AN_INTERRUPT)
#else
#include <SPI.h>
#endif

#define CRC_PRELOAD 0x5A

extern const uint8_t RL_NEW_MISO;
extern const uint8_t RL_NEW_MOSI;
extern const uint8_t RL_NEW_SCLK;
extern const uint8_t RL_NEW_SS;

extern SPIClass* RL_DEFAULT_SPI;
extern const long RL_DEFAULT_SPI_FREQUENCY;
extern const uint8_t RL_DEFAULT_SS_PIN;
extern const uint8_t RL_DEFAULT_RESET_PIN;
extern const uint8_t RL_DEFAULT_DINT_PIN;

class RadioLinkClass
{
  public:
    RadioLinkClass() { _waitOnTx = false; };
    bool begin(long frequency, void(*callbackR)(uint8_t,rl_packet_t*), void(*callbackT)(), int TxLevel);
	void end();
    static void onRxDone(int packetSize);
    static void onTxDone();
	int  lqi();
	void sleep();
	void idle();
	void setWaitOnTx(bool state);
    void publishPaquet(rl_packets* packet, byte version = 0);
    void publishBool(byte destinationid, byte senderid, byte childid, const uint8_t value, byte version = 0);
    void publishNum(byte destinationid, byte senderid, byte childid, const long value, byte version = 0);
    void publishFloat(byte destinationid, byte senderid, byte childid, const long value, const int divider, const byte precision, byte version = 0);
    void publishSwitch(byte destinationid, byte senderid, byte childid, const uint8_t value, byte version = 0);
    void publishText(byte destinationid, byte senderid, byte childid, const char* text, byte version = 0);
    void publishTag(byte destinationid, byte senderid, byte childid, const uint32_t tagH, const uint32_t tagL, const byte readerID, const byte readerType, byte version = 0);
    void publishRaw(byte destinationid, byte senderid, byte childid, const uint8_t* data, const byte len, byte version = 0);
    void publishLight(byte destinationid, byte senderid, byte childid, uint8_t state, uint8_t brightness, uint16_t temperature, uint8_t red, uint8_t green, uint8_t blue, byte version = 0);
    void publishCover(byte destinationid, byte senderid, byte childid, uint8_t command, uint8_t position, byte version = 0);
    void publishConfig(byte destinationid, byte senderid, byte childid, rl_config_t cnf, byte version = 0);
  private:
    uint8_t _waitOnTx;
};

#endif
