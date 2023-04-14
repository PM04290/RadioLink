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
    static void onRxDone(int packetSize);
    static void onTxDone();
	int  lqi();
	void setWaitOnTx(bool state);
    void publishPaquet(rl_packet_t packet);
    void publishPaquetV1(rl_packetV1_t packet); // For legacy, there is only send packet
    void publishPaquetV2(rl_packetV2_t packet); // For legacy, there is only send packet
    void publishNum(byte destinationid, byte senderid, byte childid, rl_device_t sensortype, const long value);
    void publishFloat(byte destinationid, byte senderid, byte childid, rl_device_t sensortype, const long value, const int divider, const byte precision);
    void publishText(byte destinationid, byte senderid, byte childid, const char* text);
    void publishTag(byte destinationid, byte senderid, byte childid, const uint32_t tagH, const uint32_t tagL, const byte readerID, const byte readerType);
    void publishRaw(byte destinationid, byte senderid, byte childid, const uint8_t* data, const byte len);
    void publishLight(byte destinationid, byte senderid, byte childid, uint8_t state, uint8_t brightness, uint16_t temperature, uint8_t red, uint8_t green, uint8_t blue);
    void publishCover(byte destinationid, byte senderid, byte childid, uint8_t command, uint8_t position);
    void publishConfig(byte destinationid, byte senderid, byte childid, const uint8_t* data, const byte len);
  private:
    uint8_t _waitOnTx;
};

#endif
