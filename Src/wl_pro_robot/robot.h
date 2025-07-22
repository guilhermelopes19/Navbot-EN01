
#pragma once

#include <WiFi.h>
#include <ArduinoJson.h>
#include "esp_adc_cal.h"
#include "feedback_util.h"

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
    String SYS_WIFI = "sys_wifi";
    String SYS_WEB_SOCKET_SERVER = "sys_web_socket_server";
    String SYS_RESTART = "sys_restart";
    String GET_DEVICE_INFO = "get_device_info";
    String OFF_SERVO = "off_servo";
    String ON_SERVO = "on_servo";
    String CALIBRATION_SERVO = "calibrate_servo";
} MESSAGE_TYPE;


	
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



		RobotProtocol(uint8_t len);
		~RobotProtocol();
    double get_pcb_version();
		void spinOnce(void);
		double get_fahrenheit(void);
		double get_degree_centigrade(void);
		double get_battery_voltage(void);
		double get_battery_level(void);
		int get_robot_status(void);
		void printDoc(StaticJsonDocument<300> &doc);
		void isSys(StaticJsonDocument<300> &doc);
		void parseBasic(StaticJsonDocument<300> &doc);
    void parseJson(StaticJsonDocument<300> &doc);
  private:
    uint8_t *_now_buf;
    uint8_t *_old_buf;
    uint8_t _len;
    void UART_WriteBuf(void);
    int checkBufRefresh(void);
};


extern int uncontrolable;          //Too much tilt and loss of control
extern int BAT_PIN;    // select the input pin for the ADC
extern esp_adc_cal_characteristics_t adc_chars;

extern Wrobot wrobot;
extern RobotProtocol rp;
