#pragma once

#include <Arduino.h>

#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
#include <tinySPI.h>
#define SPIClass tinySPI
#define digitalPinToInterrupt(p)  ((p) == 8 ? 0 : NOT_AN_INTERRUPT)
#else
#include <SPI.h>
#endif

class RLhelper_base {
  public:
    RLhelper_base() {}
    virtual int begin(long frequency) { return 0; }
	virtual void end();
    virtual int lqi() { return 0; }
	virtual void sleep();
	virtual void idle();
    virtual void setTxPower(int level) {}
	virtual int read(byte* buf, uint8_t len);
	virtual int write(byte* buf, uint8_t len) { return 0; }
	virtual int receiveMode() { return 0; };
    virtual void handleDintRise() {};
	virtual void onInternalRxDone(void(*callback)(int)) {};
	virtual void onInternalTxDone(void(*callback)()) {};
	virtual bool isTransmitting() { return false; } ;

  protected:
  	static void onDintRise();
    void (*_onInternalRxDone)(int);
    void (*_onInternalTxDone)();
    uint8_t _dint;
};
