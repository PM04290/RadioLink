#ifndef RL_SX1278_H_
#define RL_SX1278_H_

#include <Arduino.h>
#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
#include <tinySPI.h>
#define SPIClass tinySPI
#define digitalPinToInterrupt(p)  ((p) == 8 ? 0 : NOT_AN_INTERRUPT)
#else
#include <SPI.h>
#endif

#if defined(__AVR_ATtiny84__)
#define LORA_DEFAULT_SPI           SPI
#define LORA_DEFAULT_SPI_FREQUENCY 8E6
#define LORA_DEFAULT_SS_PIN        3
#define LORA_DEFAULT_RESET_PIN     2
#define LORA_DEFAULT_DIO0_PIN      8
#elif defined(ESP32)
#define LORA_DEFAULT_SPI           SPI
#define LORA_DEFAULT_SPI_FREQUENCY 8E6
#define LORA_DEFAULT_SS_PIN        5
#define LORA_DEFAULT_RESET_PIN     4
#define LORA_DEFAULT_DIO0_PIN      2
#else
#define LORA_DEFAULT_SPI           SPI
#define LORA_DEFAULT_SPI_FREQUENCY 8E6
#define LORA_DEFAULT_SS_PIN        10
#define LORA_DEFAULT_RESET_PIN     9
#define LORA_DEFAULT_DIO0_PIN      2
#endif

#define PA_OUTPUT_RFO_PIN          0
#define PA_OUTPUT_PA_BOOST_PIN     1

class RadioDriver : public Stream {
  public:
    RadioDriver();

    int begin(long frequency);
    void end();
    int rssi();
    virtual int available();
    void setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
    void setFrequency(long frequency);
    void setSpreadingFactor(int sf);
    void setSignalBandwidth(long sbw);
    void setCodingRate4(int denominator);
    void setPreambleLength(long length);
    void setSyncWord(int sw);
    void setGain(uint8_t gain); // Set LNA gain
    void enableCrc();
    void disableCrc();
    void enableInvertIQ();
    void disableInvertIQ();
    void onReceive(void(*callback)(int));
    void onTxDone(void(*callback)());
    int beginPacket(int implicitHeader = false);
    int endPacket(bool async = false);
    int parsePacket(int size = 0);
    int packetRssi();
    float packetSnr();
    long packetFrequencyError();

    // from Print
    virtual size_t write(uint8_t byte);
    virtual size_t write(const uint8_t *buffer, size_t size);

    // from Stream
    virtual int read();
    virtual int peek();
    virtual void flush();

    void receive(int size = 0);
    void idle();
    void sleep();
    void setOCP(uint8_t mA); // Over Current Protection control
    byte random();
    void setPins(int ss = LORA_DEFAULT_SS_PIN, int reset = LORA_DEFAULT_RESET_PIN, int dio0 = LORA_DEFAULT_DIO0_PIN);
    void setSPI(SPIClass& spi);
    void setSPIFrequency(uint32_t frequency);
    void dumpRegisters(Stream& out);
    void explicitHeaderMode();
    void implicitHeaderMode();

  private:

    void handleDio0Rise();
    bool isTransmitting();
    int getSpreadingFactor();
    long getSignalBandwidth();
    void setLdoFlag();
    uint8_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint8_t value);
    uint8_t singleTransfer(uint8_t address, uint8_t value);
    static void onDio0Rise();
  private:
#ifndef TINYSPI_H_INCLUDED
    SPISettings _spiSettings;
#endif
    int _ss;
    SPIClass* _spi;
    int _reset;
    int _dio0;
    long _frequency;
    int _packetIndex;
    int _implicitHeaderMode;
    void (*_onReceive)(int);
    void (*_onTxDone)();
};

#endif
