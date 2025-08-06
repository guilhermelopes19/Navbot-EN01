
#pragma once

#include <WiFi.h>
#include <ArduinoJson.h>
#include "esp_adc_cal.h"
#include "feedback_util.h"


#define CONFIG_JSON_SIZE 300

typedef struct {
  int height = 38;
  int roll;
  int linear;
  int angular;
  int dir;
  int dir_last;
  int joyy;
  int joyy_last;
  int joyx;
  int joyx_last;
  bool go;
  int uncontrolable = 0;          //Too much tilt and loss of control
}MemotyTabTypeDef;


typedef struct {
  int height = 38;
  int roll;
  int linear;
  int angular;
  int dir;
  int dir_last;
  int joyy;
  int joyy_last;
  int joyx;
  int joyx_last;
  bool go;
  int uncontrolable = 0;          //Too much tilt and loss of control
}Wrobot;


// Enumeration of robot motion states
typedef enum {
	FORWARD = 0, 	
	BACK,        	       		
	RIGHT,       	
	LEFT,        	
	STOP,
  JUMP,         		
} QR_State_t;


struct {
  const String SYS_WIFI = "sys_wifi";
  const String SYS_WEB_SOCKET_SERVER = "sys_web_socket_server";
  const String SYS_RESTART = "sys_restart";
  const String GET_DEVICE_INFO = "get_device_info";
  const String OFF_SERVO = "off_servo";
  const String ON_SERVO = "on_servo";
  const String CALIBRATION_SERVO = "calibrate_servo";
  const String SET_NAME = "set_name";
} MESSAGE_TYPE;

struct {
  const String NAME  = "name";
  const String WIFI_SSID = "wifi_ssid";
  const String WIFI_PASSWORD = "wifi_password";
  const String WIFI_STATE = "wifi_state";

} CONFIG_KEY;



// Robot mode enumeration type
typedef enum {
  BASIC = 0,
} Robot_Mode_t; 

class RobotProtocol
{
	public:
    double battery_voltage;
    double pcb_version;
    double fahrenheit;
    double centigrade;
    double battery_level;
    int    status;
    int16_t offset_roll = 0;
    
    StaticJsonDocument<CONFIG_JSON_SIZE> config_json;    


		RobotProtocol(uint8_t len);
		~RobotProtocol();

    double get_pcb_version();
		void spinOnce(void);
		double get_fahrenheit(void);
		double get_degree_centigrade(void);
		double get_battery_voltage(void);
		double get_battery_level(void);
		int get_robot_status(void);
    void get_dev_name(char dev_name[]);
    void config_json_init(void);
		void printDoc(StaticJsonDocument<300> &doc);
		void isSys(StaticJsonDocument<300> &doc);
		void parseBasic(StaticJsonDocument<300> &doc);
    void parseJson(StaticJsonDocument<300> &doc);
    void json_test(char* json_arr);
  private:
    uint8_t *_now_buf;
    uint8_t *_old_buf;
    uint8_t _len;
    void UART_WriteBuf(void);
    int checkBufRefresh(void);
    void save_config_json(void);
};


extern int uncontrolable;          //Too much tilt and loss of control
extern int BAT_PIN;    // select the input pin for the ADC
extern esp_adc_cal_characteristics_t adc_chars;

extern Wrobot wrobot;
extern RobotProtocol rp;
