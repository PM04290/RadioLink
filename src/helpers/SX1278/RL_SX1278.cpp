// Fork of LoRa library
// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by M&L https://github.com/PM04290
#include "RL_SX1278.h"
#include "../../RadioLink.h"


// registers
#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_OCP                  0x0b
#define REG_LNA                  0x0c
#define REG_FIFO_ADDR_PTR        0x0d
#define REG_FIFO_TX_BASE_ADDR    0x0e
#define REG_FIFO_RX_BASE_ADDR    0x0f
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_PKT_SNR_VALUE        0x19
#define REG_PKT_RSSI_VALUE       0x1a
#define REG_RSSI_VALUE           0x1b
#define REG_MODEM_CONFIG_1       0x1d
#define REG_MODEM_CONFIG_2       0x1e
#define REG_PREAMBLE_MSB         0x20
#define REG_PREAMBLE_LSB         0x21
#define REG_PAYLOAD_LENGTH       0x22
#define REG_MODEM_CONFIG_3       0x26
#define REG_FREQ_ERROR_MSB       0x28
#define REG_FREQ_ERROR_MID       0x29
#define REG_FREQ_ERROR_LSB       0x2a
#define REG_RSSI_WIDEBAND        0x2c
#define REG_DETECTION_OPTIMIZE   0x31
#define REG_INVERTIQ             0x33
#define REG_DETECTION_THRESHOLD  0x37
#define REG_SYNC_WORD            0x39
#define REG_INVERTIQ2            0x3b
#define REG_DIO_MAPPING_1        0x40
#define REG_VERSION              0x42
#define REG_PA_DAC               0x4d

// modes
#define MODE_LONG_RANGE_MODE     0x80
#define MODE_SLEEP               0x00
#define MODE_STDBY               0x01
#define MODE_TX                  0x03
#define MODE_RX_CONTINUOUS       0x05
#define MODE_RX_SINGLE           0x06

// PA config
#define PA_BOOST                 0x80

// IRQ masks
#define IRQ_TX_DONE_MASK           0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define IRQ_RX_DONE_MASK           0x40

#define RF_MID_BAND_THRESHOLD    525E6
#define RSSI_OFFSET_HF_PORT      157
#define RSSI_OFFSET_LF_PORT      164

#define MAX_PKT_LENGTH           255

RLhelper_SX1278::RLhelper_SX1278()
{
#if !defined(__AVR_ATtiny84__)
  _spiSettings = SPISettings(RL_DEFAULT_SPI_FREQUENCY, MSBFIRST, SPI_MODE0);
#endif
  _ss = RL_DEFAULT_SS_PIN;
  _reset = RL_DEFAULT_RESET_PIN;
  _dint = RL_DEFAULT_DINT_PIN;
  _spi = RL_DEFAULT_SPI;
  _frequency = 0;
}

int RLhelper_SX1278::begin(long frequency)
{
 // setup pins
  pinMode(_ss, OUTPUT);
  // set SS high
  digitalWrite(_ss, HIGH);
  if (_reset != -1)
  {
    pinMode(_reset, OUTPUT);
    // perform reset
    digitalWrite(_reset, LOW);
    delay(10);
    digitalWrite(_reset, HIGH);
    delay(10);
  }

#if defined(__AVR_ATtiny84__)
  _spi->setDataMode(SPI_MODE0);
#endif
  // start SPI
#ifdef ESP32
  if (RL_NEW_SCLK > 0)
  {
    _spi->begin(RL_NEW_SCLK, RL_NEW_MISO, RL_NEW_MOSI, RL_NEW_SS);
  } else
  {
    _spi->begin();
  }
#else
  _spi->begin();
#endif

  // check version
  uint8_t version = readRegister(REG_VERSION);
  if (version != 0x12) {
    return 0;
  }
  // put in sleep mode
  sleep();
  // set frequency
  setFrequency(frequency);
  // set base addresses
  writeRegister(REG_FIFO_TX_BASE_ADDR, 0);
  writeRegister(REG_FIFO_RX_BASE_ADDR, 0);
  // set LNA boost
  writeRegister(REG_LNA, readRegister(REG_LNA) | 0x03);
  // set auto AGC
  writeRegister(REG_MODEM_CONFIG_3, 0x04);
  // set output power to 17 dBm
  setTxPower(17);
  // put in standby mode
  idle();
  return 1;
}

void RLhelper_SX1278::setTxPower(int level)
{
    // PA BOOST
    if (level > 17)
	{
      if (level > 20)
	  {
        level = 20;
      }
      // subtract 3 from level, so 18 - 20 maps to 15 - 17
      level -= 3;
      // High Power +20 dBm Operation (Semtech SX1276/77/78/79 5.4.3.)
      writeRegister(REG_PA_DAC, 0x87);
      setOCP(140);
    } else
	{
      if (level < 2)
	  {
        level = 2;
      }
      //Default value PA_HF/LF or +17dBm
      writeRegister(REG_PA_DAC, 0x84);
      setOCP(100);
    }
    writeRegister(REG_PA_CONFIG, PA_BOOST | (level - 2));
}

int RLhelper_SX1278::read(byte* buf, uint8_t len)
{
  byte p = 0;
  byte available = readRegister(REG_RX_NB_BYTES);
  while (p < available && p < len)
  {
	buf[p++] = readRegister(REG_FIFO);
  }
  return p;
}

int RLhelper_SX1278::write(byte* buf, uint8_t len)
{
  beginPacket();
  // check len
  if (len > MAX_PKT_LENGTH)
  {
    len = MAX_PKT_LENGTH;
  }
  // write data
  for (size_t i = 0; i < len; i++)
  {
    writeRegister(REG_FIFO, buf[i]);
  }
  // update length
  writeRegister(REG_PAYLOAD_LENGTH, len);
  //
  endPacket();
  return len;
}

int RLhelper_SX1278::receiveMode()
{
  writeRegister(REG_DIO_MAPPING_1, 0x00); // DIO0 => RXDONE
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
  return 1;
}

int RLhelper_SX1278::lqi()
{
  int rssi = (readRegister(REG_PKT_RSSI_VALUE) - (_frequency < RF_MID_BAND_THRESHOLD ? RSSI_OFFSET_LF_PORT : RSSI_OFFSET_HF_PORT));
  return map(rssi, -120, -20, 0, 100);
}

void RLhelper_SX1278::handleDintRise()
{
  int irqFlags = readRegister(REG_IRQ_FLAGS);
  // clear IRQ's
  writeRegister(REG_IRQ_FLAGS, irqFlags);
  if ((irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0)
  {
    if ((irqFlags & IRQ_RX_DONE_MASK) != 0)
    {
      // received a packet
      int packetLength = readRegister(REG_RX_NB_BYTES);
      // set FIFO address to current RX address
      writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_CURRENT_ADDR));
      if (_onInternalRxDone)
      {
        _onInternalRxDone(packetLength);
      }
    }
    else if ((irqFlags & IRQ_TX_DONE_MASK) != 0)
    {
      if (_onInternalTxDone)
      {
        _onInternalTxDone();
      }
    }
  }
}

uint8_t RLhelper_SX1278::readRegister(uint8_t address)
{
  return singleTransfer(address & 0x7f, 0x00);
}

void RLhelper_SX1278::writeRegister(uint8_t address, uint8_t value)
{
  singleTransfer(address | 0x80, value);
}

uint8_t RLhelper_SX1278::singleTransfer(uint8_t address, uint8_t value)
{
  uint8_t response;
  digitalWrite(_ss, LOW);
#if !defined(__AVR_ATtiny84__)
  _spi->beginTransaction(_spiSettings);
#endif
  _spi->transfer(address);
  response = _spi->transfer(value);
#if !defined(__AVR_ATtiny84__)
  _spi->endTransaction();
#endif
  digitalWrite(_ss, HIGH);
  return response;
}

void RLhelper_SX1278::idle()
{
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

void RLhelper_SX1278::sleep()
{
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

void RLhelper_SX1278::setFrequency(long frequency)
{
  _frequency = frequency;
  uint64_t frf = ((uint64_t)frequency << 19) / 32000000;
  writeRegister(REG_FRF_MSB, (uint8_t)(frf >> 16));
  writeRegister(REG_FRF_MID, (uint8_t)(frf >> 8));
  writeRegister(REG_FRF_LSB, (uint8_t)(frf >> 0));
}

void RLhelper_SX1278::setOCP(uint8_t mA)
{
  uint8_t ocpTrim = 27;
  if (mA <= 120) {
    ocpTrim = (mA - 45) / 5;
  } else if (mA <=240) {
    ocpTrim = (mA + 30) / 10;
  }
  writeRegister(REG_OCP, 0x20 | (0x1F & ocpTrim));
}

int RLhelper_SX1278::beginPacket()
{
  if (isTransmitting()) {
    return 0;
  }
  // put in standby mode
  idle();
  // reset FIFO address and paload length
  writeRegister(REG_FIFO_ADDR_PTR, 0);
  writeRegister(REG_PAYLOAD_LENGTH, 0);
  return 1;
}

int RLhelper_SX1278::endPacket()
{
  if (_onInternalTxDone)
  {
      writeRegister(REG_DIO_MAPPING_1, 0x40); // DIO0 => TXDONE
  }
  // put in TX mode
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);

  return 1;
}

bool RLhelper_SX1278::isTransmitting()
{
  if ((readRegister(REG_OP_MODE) & MODE_TX) == MODE_TX) {
    return true;
  }
  if (readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) {
    // clear IRQ's
    writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
  }
  return false;
}

void RLhelper_SX1278::onInternalRxDone(void(*callback)(int))
{
  _onInternalRxDone = callback;
  if (callback) {
	pinMode(_dint, INPUT);
	attachInterrupt(digitalPinToInterrupt(_dint), onDintRise, RISING);
  } else {
	detachInterrupt(digitalPinToInterrupt(_dint));
  }
}

void RLhelper_SX1278::onInternalTxDone(void(*callback)())
{
  _onInternalTxDone = callback;
  if (callback) {
	pinMode(_dint, INPUT);
	attachInterrupt(digitalPinToInterrupt(_dint), onDintRise, RISING);
  } else {
	detachInterrupt(digitalPinToInterrupt(_dint));
  }
}
