#include "wifi.h"
#include "robot.h"
#include "EEPROM.h"
#include "define.h"

char wifi_ssid[50]={0};
char wifi_password[30]={0};
char eeprom_once=0;
// ESP-01S access the WiFi network in AP mode
void set_sta_wifi()
{
  if(eeprom_once == 0)
  {
    eeprom_once = 1;
    if(!EEPROM.begin(100))
    {
      Serial.printf("error! eeprom init false \r\n");
      return;
    }
  }
  
  //Read the wifi information
  EEPROM.readString(ADDR_WIFI_SSID,wifi_ssid,50);  
  EEPROM.readString(ADDR_WIFI_PASSWORD,wifi_password,30);  

  Serial.printf("wifi:%s \r\n",wifi_ssid);
  Serial.printf("pswd:%s \r\n",wifi_password);

  WiFi.setHostname("navbot-en01-123456");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid,wifi_password);
  Serial.printf("connect %s...\r\n",wifi_ssid);

}
void wifi_init(void)
{
  set_sta_wifi();
}
void wifi_loop(void)
{
  static char wifi_status = 0;
  static char connect_count_down = 10;

//If the wifi is not connected, it will actively connect every 10 seconds
  if(WiFi.status()!=WL_CONNECTED)  
  {
    connect_count_down --;
    if(connect_count_down == 0)
    {
      set_sta_wifi();
      connect_count_down = 10;
    }
    wifi_status = 0;
    
  }
  else //Print the IP information once when connecting via wifi
  {
    connect_count_down = 1;
    if(wifi_status == 0)
    {
      // Print the IP address of ESP-01S
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    }
    wifi_status = 1;
  }
  
}

