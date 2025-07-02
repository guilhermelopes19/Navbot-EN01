
#pragma once

#include <WiFi.h>
#include <ArduinoJson.h>

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
} MESSAGE_TYPE;


	
// Robot mode enumeration type
typedef enum {
  BASIC = 0,
} Robot_Mode_t; 

class RobotProtocol
{
	public:
		RobotProtocol(uint8_t len);
		~RobotProtocol();
		void spinOnce(void);
		void printDoc(StaticJsonDocument<300> &doc);
		void isSys(StaticJsonDocument<300> &doc);
		void parseBasic(StaticJsonDocument<300> &doc);
;
  private:
    uint8_t *_now_buf;
    uint8_t *_old_buf;
    uint8_t _len;
    void UART_WriteBuf(void);
    int checkBufRefresh(void);
};

extern Wrobot wrobot;
extern RobotProtocol rp;
