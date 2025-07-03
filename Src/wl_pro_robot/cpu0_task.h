
#pragma once

#include "oled_16064.h"
#include "SPIFFS.h"
#include "robot.h"



void maneuver_to_expression(void);
void delayed_sleep_mode(void);

#define SLEEP_COUNT_DOWN 5
uint16_t sleep_time;


File file;
int frames_time = 500;
uint8_t last_frames_width;
void cpu0_task(void *ptParam) {

  uint8_t *file_header = (uint8_t *)malloc(20);
  uint8_t *frames_data = (uint8_t *)malloc(5128);
  int8_t i, j;
  uint8_t width;
  uint8_t height;
  uint8_t *code;
  uint16_t frames_size;
  uint8_t frames_num;
  uint8_t x_coord;

  SPIFFS.begin(true);
  OLED_Init();
  while (1) {
    maneuver_to_expression();

    file.read(file_header, 8);
    frames_num = *(uint32_t *)(file_header);
    width = *(uint16_t *)(file_header + 4);
    height = *(uint16_t *)(file_header + 6);
    frames_size = width * height / 2;
    x_coord = (160 - width) / 2;

    if (last_frames_width != width) {
      OLED_Fill(0, 0, OLED_W, OLED_H, 0x00);
    }

    for (i = 0; i < frames_num; i++) {
      file.read(frames_data, frames_size);
      OLED_DrawBMP(x_coord, 0, width, height, frames_data, 0);
      delay(frames_time);
    }
    last_frames_width = width;
    file.close();
    //delay(2000);
  }
}




enum {
  ADVANCE = 0x0001,
  RETREAT = 0x0002,
  TERN_LEFT = 0x0004,
  TERN_RIGHT = 0x0008,
  RISE = 0x0010,
  DECLINE = 0x0020,
  GO = 0x0040,

  DANCE1 = 0x0100,
  DANCE2 = 0x0200,
  DANCE3 = 0x0400,
  DANCE4 = 0x0800,
} maneuver_state;

void maneuver_to_expression(void) {
  int maneuver_state = 0;
  frames_time = 50;

  if (wrobot.go > 0) {
    sleep_time = SLEEP_COUNT_DOWN;
    maneuver_state |= GO;
  }

  if (wrobot.joyy > 30)
    maneuver_state |= ADVANCE;
  else if (wrobot.joyy < -30)
    maneuver_state |= RETREAT;

  if (wrobot.joyx > 30)
    maneuver_state |= TERN_LEFT;
  else if (wrobot.joyx < -30)
    maneuver_state |= TERN_RIGHT;

  if (wrobot.height > 70)
    maneuver_state |= RISE;
  else if (wrobot.height < 38)
    maneuver_state |= DECLINE;



  if (!(maneuver_state & GO)) {
    delayed_sleep_mode();
  } else if (wrobot.uncontrolable == 1) {
    file = SPIFFS.open("/10.bin", "r");
  } else if (maneuver_state & RISE) {
    file = SPIFFS.open("/2.bin", "r");
  } else if (maneuver_state & ADVANCE) {
    file = SPIFFS.open("/1.bin", "r");
  } else if (maneuver_state & RETREAT) {
    file = SPIFFS.open("/2.bin", "r");
  } else if (maneuver_state & TERN_LEFT) {
    file = SPIFFS.open("/3.bin", "r");
  } else if (maneuver_state & TERN_RIGHT) {
    file = SPIFFS.open("/4.bin", "r");
  } else if (maneuver_state & DANCE1) {
    file = SPIFFS.open("/10.bin", "r");
  } else if (maneuver_state & DANCE2) {
    file = SPIFFS.open("/11.bin", "r");
  } else if (maneuver_state & DANCE3) {
    file = SPIFFS.open("/12.bin", "r");
  } else if (maneuver_state & DANCE4) {
    file = SPIFFS.open("/13.bin", "r");
  } else {
    if (maneuver_state & DECLINE) {
      file = SPIFFS.open("/6.bin", "r");
    }else{
      file = SPIFFS.open("/11.bin", "r");
    }
    
  }
}

void delayed_sleep_mode() {
  if (sleep_time > SLEEP_COUNT_DOWN / 2) {
    file = SPIFFS.open("/10.bin", "r");
    sleep_time--;
  } else if (sleep_time > 0) {
    file = SPIFFS.open("/13.bin", "r");
    sleep_time--;
  } else {
    file = SPIFFS.open("/z.bin", "r");
    frames_time = 500;
  }
}
