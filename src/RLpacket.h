#ifndef RLPACKET_H_
#define RLPACKET_H_

#include <Arduino.h>

#define RL_CURRENT_VERSION 3

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

#define MAX_PACKET_DATA_LEN_V3 16

#define MAX_PACKET_DATA_LEN MAX_PACKET_DATA_LEN_V3

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
    char text[MAX_PACKET_DATA_LEN];
    uint8_t rawByte[MAX_PACKET_DATA_LEN];
    uint16_t rawWord[MAX_PACKET_DATA_LEN / sizeof(uint16_t)];
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
  } data;
} rl_packet_t;
#define RL_PACKET_SIZE sizeof(rl_packet_t)

// Old version for legacy compatotility

typedef enum {
  SV1_BINARYSENSOR    = 0,
  SV1_SWITCH          = 1,
  SV1_LIGHT           = 2,
  SV1_SENSOR          = 3,
  SV1_TRIGGER         = 4,
  SV1_COVER           = 5,
  SV1_SELECT          = 6,
  SV1_INPUTNUMBER     = 7,
  SV1_INPUTTEXT       = 8,
  SV1_CUSTOM          = 9,
  SV1_TAG             = 10
} rl_deviceV1_t; // limited to 32

#define MAX_PACKET_DATA_LEN_V1 8
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
    } num;
    char text[MAX_PACKET_DATA_LEN_V1];
    uint8_t rawByte[MAX_PACKET_DATA_LEN_V1];
    uint16_t rawWord[MAX_PACKET_DATA_LEN_V1 / sizeof(uint16_t)];
    struct {
      uint32_t tagH;
      uint32_t tagL;
    } tag;
  } data_V1;
} rl_packetV1_t;
#define RL_PACKETV1_SIZE sizeof(rl_packetV1_t)

#define MAX_PACKET_DATA_LEN_V2 12
typedef enum {
  SV2_BINARYSENSOR    = 0,
  SV2_NUMERICSENSOR   = 1,
  SV2_SWITCH          = 2,
  SV2_LIGHT           = 3,
  SV2_COVER           = 4,
  SV2_FAN             = 5,
  SV2_HVAC            = 6,
  SV2_SELECT          = 7,
  SV2_TRIGGER         = 8,
  SV2_CUSTOM          = 9,
  SV2_TAG             = 10,
  SV2_TEXTSENSOR      = 11,
} rl_deviceV2_t; // limited to 31

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
    char text[MAX_PACKET_DATA_LEN_V2];
    uint8_t rawByte[MAX_PACKET_DATA_LEN_V2];
    uint16_t rawWord[MAX_PACKET_DATA_LEN_V2 / sizeof(uint16_t)];
    struct {
      uint32_t tagH;
      uint32_t tagL;
      uint16_t readerID;
      uint16_t readerType;
    } tag;
  } data;
} rl_packetV2_t;
#define RL_PACKETV2_SIZE sizeof(rl_packetV2_t)

#endif
