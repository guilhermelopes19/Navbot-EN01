#pragma once

#include <WiFi.h>


void wifi_set_sta(void); //Access the WiFi network in STA mode
void wifi_init(void);
void wifi_loop(void);




extern char wifi_ssid[50];
extern char wifi_password[30];


