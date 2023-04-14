#ifndef RL_SX1278_H_
#define RL_SX1278_H_

#include <Arduino.h>
#include "../RLhelper.h"

class RLhelper_SX1278 : public RLhelper_base {
  public:
    RLhelper_SX1278();
    int begin(long frequency) override;
    int lqi() override;
    void setTxPower(int level) override;
	int read(byte* buf, uint8_t len) override;
	int write(byte* buf, uint8_t len) override;
	int receiveMode() override;
	void handleDintRise() override;
	void onInternalRxDone(void(*callback)(int)) override;
	void onInternalTxDone(void(*callback)()) override;
	bool isTransmitting() override;
  private:
	//static void onDintRise();
#if !defined(__AVR_ATtiny84__)
    SPISettings _spiSettings;
#endif
    int _ss;
    SPIClass* _spi;
  protected:
    int _reset;
    long _frequency;
    uint8_t readRegister(uint8_t address);
	void writeRegister(uint8_t address, uint8_t value);
    uint8_t singleTransfer(uint8_t address, uint8_t value);
    void idle();
    void sleep();
	void setFrequency(long frequency);
	void setOCP(uint8_t mA);
	int beginPacket();
	int endPacket();
};

#endif
