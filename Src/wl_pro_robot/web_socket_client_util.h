#pragma once

#include "eeprom_util.h"
#include "wifi.h"
#include "robot.h"


void web_sockets_client_init();

void web_sockets_client_loop();

void web_sockets_client_send_message(String value);