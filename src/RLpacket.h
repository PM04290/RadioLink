#ifndef RLPACKET_H_
#define RLPACKET_H_

#include <Arduino.h>

#define RL_CURRENT_VERSION 1

typedef enum {
  S_BINARYSENSOR    = 0,
  S_NUMERICSENSOR   = 1,
  S_SWITCH          = 2,
  S_LIGHT           = 3,
  S_COVER           = 4,
  S_FAN             = 5,
  S_HVAC            = 6,
  S_SELECT          = 7,
  S_TRIGGER         = 8,
  S_CUSTOM          = 9,
  S_TAG             = 10,
  S_TEXTSENSOR      = 11,
  S_INPUTNUMBER     = 12,
  //\ add new before this line
  S_CONFIG          = 31, // for internal use only
} rl_device_t; // limited to 31

//
typedef enum {
  V_BOOL    = 0,
  V_NUM     = 1,
  V_FLOAT   = 2,
  V_TEXT    = 3,
  V_TAG     = 4,
  V_RAW     = 5
} rl_data_t; // limited to 7


// V1 (current version)
#define MAX_PACKET_DATA_LEN_V1 16
typedef struct {
  uint8_t childID;
  uint8_t deviceType;
  uint8_t dataType;
  uint8_t reserved[MAX_PACKET_DATA_LEN_V1-3-6];
  char unit[6];
} rl_config_t;

typedef struct __attribute__((packed)) {
  uint8_t destinationID;
  uint8_t senderID;
  uint8_t childID;
  uint8_t sensordataType; // (rl_sensor_t << 3) + rl_data_t
  union {
    struct {
      int32_t value;
      uint16_t divider;
      uint8_t precision;
      char unit[5];
    } num;
    char text[MAX_PACKET_DATA_LEN_V1];
    uint8_t rawByte[MAX_PACKET_DATA_LEN_V1];
    uint16_t rawWord[MAX_PACKET_DATA_LEN_V1 / sizeof(uint16_t)];
    struct {
      uint32_t tagH;
      uint32_t tagL;
      uint16_t readerID;
      uint16_t readerType;
    } tag;
	struct {
	  uint8_t state;
	  uint8_t brightness;
	  uint16_t temperature;
	  uint8_t red;
	  uint8_t green;
	  uint8_t blue;
	} light;
	struct {
	  uint8_t state;
	  uint8_t command;
	  uint8_t position;
	} cover;
    rl_config_t config;
  } data;
  uint8_t crc; // must be the last : for calculation with sizeof()-1
} rl_packetV1_t;
#define RL_PACKETV1_SIZE sizeof(rl_packetV1_t)
#define MAX_PACKET_DATA_LEN MAX_PACKET_DATA_LEN_V1

// current packet type & size
#define rl_packet_t rl_packetV1_t
#define RL_PACKET_SIZE RL_PACKETV1_SIZE

typedef union {
  rl_packet_t current;
  rl_packetV1_t v1;
} rl_packets;

#endif
