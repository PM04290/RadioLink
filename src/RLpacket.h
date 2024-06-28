#ifndef RLPACKET_H_
#define RLPACKET_H_
/* Revision 2.0
 *
 */
#include <Arduino.h>

#define RL_CURRENT_VERSION 1


typedef enum {
  E_BINARYSENSOR    = 0,
  E_NUMERICSENSOR   = 1,
  E_SWITCH          = 2,
  E_LIGHT           = 3,
  E_COVER           = 4,
  E_FAN             = 5,
  E_HVAC            = 6,
  E_SELECT          = 7,
  E_TRIGGER         = 8,
  E_EVENT           = 9,
  E_TAG             = 10,
  E_TEXTSENSOR      = 11,
  E_INPUTNUMBER     = 12,
  E_CUSTOM          = 13,
  E_DATE            = 14,
  E_TIME            = 15,
  E_DATETIME        = 16,
  //\ add new before this line
  E_CONFIG          = 31, // for internal use only
} rl_element_t; // limited to 31

//
typedef enum {
  D_BOOL    = 0,
  D_NUM     = 1,
  D_FLOAT   = 2,
  D_TEXT    = 3,
  D_TAG     = 4,
  D_RAW     = 5
} rl_data_t; // limited to 7


typedef enum {
  C_BASE    = 0,
  C_UNIT    = 1,
  C_OPTS    = 2,
  C_NUMS    = 3,
  //\ add new before this line
  C_END     = 7
} rl_conf_t; // limited to 7


// V1 (current version)
#define MAX_PACKET_DATA_LEN_V1 16
typedef struct __attribute__((packed)) {
  uint8_t childID;
  uint8_t deviceType;
  uint8_t dataType;
  char name[MAX_PACKET_DATA_LEN_V1-3];
} rl_configBase_t;

typedef struct __attribute__((packed)) {
  uint8_t childID;
  char text[MAX_PACKET_DATA_LEN_V1-1];
} rl_configText_t;

typedef struct __attribute__((packed)) {
  uint8_t childID;
  int32_t mini;
  int32_t maxi;
  uint16_t divider;
  uint16_t step;
  char text[MAX_PACKET_DATA_LEN_V1-13];
} rl_configNums_t;

typedef union  __attribute__((packed)) {
	rl_configBase_t base;
	rl_configText_t text;
	rl_configNums_t nums;
} rl_configs_t;

typedef struct __attribute__((packed)) {
  uint8_t destinationID;
  uint8_t senderID;
  uint8_t childID;
  uint8_t sensordataType; // (rl_sensor_t << 3) + rl_data_t
  union {
    struct {
      int32_t value;
      uint16_t divider;
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
	rl_configs_t configs;
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

#define RL_ID_BROADCAST 0xFF
#define RL_ID_CONFIG    0xFE
#define RL_ID_PING      0xFD
#define RL_ID_DATETIME  0xFC

#endif
