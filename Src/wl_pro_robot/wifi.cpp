#include "wifi.h"
#include "robot.h"
#include "eeprom_util.h"

char wifi_ssid[50] = { 0 };
char wifi_password[30] = { 0 };
char eeprom_once = 0;

WIFI_STATE_TypeDef WIFI_STATE;


// Configure the parameters related to the AP (hotspot) mode
// const char *ssid = "navbot-en01-xxxxxx";
const char *ap_password = "12345678";

IPAddress AP_IP(192, 168, 1, 11);
IPAddress AP_GATEWAY(192, 168, 1, 11);
IPAddress AP_SUBNET(255, 255, 255, 0);


char wifi_mode;

// ESP-01S access the WiFi network in AP mode
void wifi_set_sta() {
  //Read the wifi information
  eeprom_util.read(&EepromParam.ADDR_WIFI_SSID, wifi_ssid);
  eeprom_util.read(&EepromParam.ADDR_WIFI_PASSWORD, wifi_password);

  String dev_name = rp.config_json[CONFIG_KEY.NAME];
  const char* sta_name = dev_name.c_str();

  WiFi.setHostname(sta_name);
  wifi_mode = WIFI_STA;
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  String wifi_info = String("ssid:") + wifi_ssid + ",password:" + wifi_password;
  Serial.println("Connecting to Wifi: " + wifi_info);
  Serial.printf("connect %s...\r\n", wifi_ssid);
}

void wifi_set_ap(void) {
  char ap_name[20] = { 0 };
  rp.build_dev_name(ap_name);
  wifi_mode = WIFI_AP;
  WiFi.mode(WIFI_AP);
  Serial.printf("Reset Reason: %d%%\n", ESP.getFreeHeap()*100 / ESP.getHeapSize());
  WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_SUBNET);
  Serial.printf("Reset Reason: %d%%\n", ESP.getFreeHeap()*100 / ESP.getHeapSize());
  WiFi.softAP(ap_name, ap_password);
  // Serial.println();
  // Serial.print("AP IP address = ");
  // Serial.println(WiFi.softAPIP());
}
bool is_wifi_state(String str)
{
  if(str == WIFI_STATE.CLIENT){
    return true;
  }else if(str == WIFI_STATE.SERVER){
    return true;
  }else if(str == WIFI_STATE.CLOSE){
    return true;
  }
  return false;
}
String get_wifi_state(void) {
  char state_arr[EepromParam.ADDR_WIFI_STATE.size];
  eeprom_util.read(&EepromParam.ADDR_WIFI_STATE, state_arr);
  String state = state_arr;
  if (is_wifi_state(state) == false) {
    state = WIFI_STATE.SERVER;
    eeprom_util.write(&EepromParam.ADDR_WIFI_STATE, state);
  }
  return state;
}

void wifi_init(void) {
  String wifi_state = get_wifi_state();
  Serial.printf("wifi_state: %s\n", wifi_state.c_str());
  if (wifi_state == WIFI_STATE.CLIENT) {
    Serial.println("start wifi client...");
    wifi_set_sta();  // wifi client
    rp.wifi_state = WIFI_CLIENT;
  } else if (wifi_state == WIFI_STATE.SERVER) {
    Serial.println("start wifi server...");
    wifi_set_ap();  // wifi server
    rp.wifi_state = WIFI_SERVOR;
  } else {

  }
}
void wifi_loop(void) {
  static char wifi_status = 0;
  static char connect_count_down = 10;

  if (wifi_mode != WIFI_STA) return;

  //If the wifi is not connected, it will actively connect every 10 seconds
  if (WiFi.status() != WL_CONNECTED) {
    connect_count_down--;
    if (connect_count_down == 0) {
      wifi_set_sta();
      connect_count_down = 10;
    }
    wifi_status = 0;

  } else  //Print the IP information once when connecting via wifi
  {
    connect_count_down = 1;
    if (wifi_status == 0) {
      // Print the IP address of ESP-01S
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    }
    wifi_status = 1;
  }
}
