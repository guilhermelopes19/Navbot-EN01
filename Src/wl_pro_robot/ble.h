#pragma once

typedef struct BlePackTypDef{
  uint8_t header[2];
  uint8_t idle1[3];
  int8_t roll;
  int8_t height;
  int16_t linear;
  uint8_t linear_H;
  uint8_t linear_L;
  int8_t angular;
  int8_t stable;
  int8_t mode;
  int8_t dir;
  int8_t joy_y;
  int8_t joy_x;
  int8_t idle2[4];
  uint8_t checkSum;
}BlePackTypDef;

typedef struct BleDataTypDef{
  BlePackTypDef data;
  int16_t len;
  int16_t index;
  uint8_t state;
  uint8_t timeout;
}BleDataTypDef;


void ble_init(char* botName) ;
void ble_send_data(uint8_t* data, uint8_t len);

extern BleDataTypDef ble_rx;







