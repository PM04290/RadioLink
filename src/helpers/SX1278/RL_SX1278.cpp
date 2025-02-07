// Fork of LoRa library
// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by M&L https://github.com/PM04290
/*
	The SX1278 has many different settings to allow the user to fully customize the range,
	data rate, and power consumption, but the most important are the following three:
	
    Bandwidth. The SX1278 allows to set bandwidth from 7.8 kHz to 500 kHz. The higher the
	bandwidth value, the faster the data transfer. However, this comes at a cost of lower
	total sensitivity, and therefore lower maximum range.
	
    Spreading Factor. In LoRa modulation, each bit of information is represented by multiple
	chirps. Spreading factor is a measure of how many chirps are there per bit of data.
	The SX1278 supports 7 different settings, the higher the spreading factor,
	the slower the data transmission and higher the range.
	
    Coding Rate. To improve the stability of transmission, SX1278 can perform additional
	error checking. The measure of this error checking is called coding rate and there are
	four possible values. At the lowest 4/5 coding rate, the transmission is a bit less stable,
	and a bit faster. At the highest coding rate 4/8, the link is much more reliable, but at
	the expense of slower data transmission rate.
*/


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
	pinMode(_ss, OUTPUT);
	// set SS high
	digitalWrite(_ss, HIGH);
	if (_reset != 0xFF)
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
	//setTxPower(17);
	// put in standby mode
	idle();
	return 1;
}

void RLhelper_SX1278::end()
{
	_spi->end();
	#if defined(__AVR_ATtiny84__)
		pinMode(4, INPUT);
		pinMode(5, INPUT);
		pinMode(6, INPUT);
	#else
		if (RL_NEW_SCLK > 0)
		{
			pinMode(RL_NEW_SCLK, INPUT);
			pinMode(RL_NEW_MISO, INPUT);
			pinMode(RL_NEW_MOSI, INPUT);
		} else
		{
			pinMode(SCK, INPUT);
			pinMode(MISO, INPUT);
			pinMode(MOSI, INPUT);
		}
	#endif
	// setup pins
	pinMode(_ss, INPUT);
	if (_reset != 0xFF)
	{
		pinMode(_reset, INPUT);
	}
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
	float rawSNR = (int8_t)readRegister(REG_PKT_SNR_VALUE) / 4;
	if(rawSNR < 0.0) {
		rssi += rawSNR;
	}
	return map(rssi, -128, -10, 0, 100);
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

void RLhelper_SX1278::setSpreadingFactor(int sf)
{
	if (sf < 6) {
		sf = 6;
		} else if (sf > 12) {
		sf = 12;
	}
	
	if (sf == 6) {
		writeRegister(REG_DETECTION_OPTIMIZE, 0xc5);
		writeRegister(REG_DETECTION_THRESHOLD, 0x0c);
		} else {
		writeRegister(REG_DETECTION_OPTIMIZE, 0xc3);
		writeRegister(REG_DETECTION_THRESHOLD, 0x0a);
	}
	
	writeRegister(REG_MODEM_CONFIG_2, (readRegister(REG_MODEM_CONFIG_2) & 0x0f) | ((sf << 4) & 0xf0));
	setLdoFlag();
}

void RLhelper_SX1278::setSignalBandwidth(int bw)
{
	if (bw < 0) {
		bw = 0;
	}
	if (bw > 9) {
		bw = 9;
	}
	writeRegister(REG_MODEM_CONFIG_1, (readRegister(REG_MODEM_CONFIG_1) & 0x0f) | (bw << 4));
	setLdoFlag();
}

void RLhelper_SX1278::setLdoFlag()
{
	// Section 4.1.1.5
	long symbolDuration = 1000 / ( getSignalBandwidth() / (1L << getSpreadingFactor()) ) ;
	
	// Section 4.1.1.6
	boolean ldoOn = symbolDuration > 16;
	
	uint8_t config3 = readRegister(REG_MODEM_CONFIG_3);
	bitWrite(config3, 3, ldoOn);
	writeRegister(REG_MODEM_CONFIG_3, config3);
}

int RLhelper_SX1278::getSpreadingFactor()
{
	return readRegister(REG_MODEM_CONFIG_2) >> 4;
}

long RLhelper_SX1278::getSignalBandwidth()
{
	byte bw = (readRegister(REG_MODEM_CONFIG_1) >> 4);
	
	switch (bw) {
		case 0: return 7.8E3;
		case 1: return 10.4E3;
		case 2: return 15.6E3;
		case 3: return 20.8E3;
		case 4: return 31.25E3;
		case 5: return 41.7E3;
		case 6: return 62.5E3;
		case 7: return 125E3;
		case 8: return 250E3;
		case 9: return 500E3;
	}
	
	return -1;
}

void RLhelper_SX1278::setCodingRate4(int denominator)
{
	if (denominator < 5) {
		denominator = 5;
		} else if (denominator > 8) {
		denominator = 8;
	}
	
	int cr = denominator - 4;
	
	writeRegister(REG_MODEM_CONFIG_1, (readRegister(REG_MODEM_CONFIG_1) & 0xf1) | (cr << 1));
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
	uint8_t reg = readRegister(REG_OP_MODE);
	if ((reg & MODE_TX) == MODE_TX) {
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

void RLhelper_SX1278::setDint(uint8_t pin)
{
	_dint = pin;
}
