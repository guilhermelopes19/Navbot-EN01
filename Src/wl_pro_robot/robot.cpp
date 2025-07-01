
#include "robot.h"
#include <ArduinoJson.h>
#include "wifi.h"
#include "eeprom_util.h"
Wrobot wrobot;
RobotProtocol rp(20);

RobotProtocol::RobotProtocol(uint8_t len)
{
    _len = len;
    _now_buf = new uint8_t[_len];
    _old_buf = new uint8_t[_len];

    for(int i=0;i<_len;i++) {
        _now_buf[i] = 0;
    }

    _now_buf[0] = 0xAA;
    _now_buf[1] = 0x55;
}

RobotProtocol::~RobotProtocol()
{
    delete [] _now_buf;
    delete [] _old_buf;
}

void RobotProtocol::spinOnce(void)
{
    int flag = checkBufRefresh();
    if(flag)
    {
        //UART_WriteBuf(); //This will convert the control information on the web end into a serial port protocol for transmission

        //Serial.println(wrobot.dir);//Test data acquisition
        //Serial.println("date have send\n");
        //Serial.printf("height:%d\n", wrobot.height);
        //Serial.printf("roll:%d\n", wrobot.pitch);
        //Serial.printf("linear:%d\n", wrobot.linear);
//        Serial.printf("\n");
//        Serial.printf("joy_X:%d\n", wrobot.joyx);
//        Serial.printf("joy_Y:%d\n", wrobot.joyy);
    }
}









/**************The following is simultaneously used for serial port output control protocols***************/

void RobotProtocol::UART_WriteBuf(void)
{
    for(int i=0; i<_len; i++) {
        Serial.write(_now_buf[i]);
    }
}

int RobotProtocol::checkBufRefresh(void)
{
    int ret = 0;
    for(int i=0; i<_len; i++)
    {
        if(_now_buf[i] != _old_buf[i]) {
            ret = 1;
            break;
        }else {
            ret = 0;
        }
    }

    for(int i=0; i<_len; i++) {
        _old_buf[i]= _now_buf[i];
    }
    return ret;
}


void RobotProtocol::printDoc(StaticJsonDocument<300> &doc){
    char receive_command[300];
    serializeJson(doc, receive_command);
    Serial.printf("receive_command:%s \r\n",receive_command);
}

void RobotProtocol::isSys(StaticJsonDocument<300> &doc) {
    String type = doc["type"];
    Serial.print("type:");
    Serial.println(type);

    if (type == MESSAGE_TYPE.SYS_WIFI) {
        String ssid = doc["ssid"];
        String password = doc["password"];
        String state = doc["state"];

        // save data
        if (state == WIFI_STATE.SERVER || state == WIFI_STATE.CLIENT || state == WIFI_STATE.CLOSE) {
            eeprom_util.write(&EepromParam.ADDR_WIFI_STATE, state);
        }
        eeprom_util.write(&EepromParam.ADDR_WIFI_SSID, ssid);
        eeprom_util.write(&EepromParam.ADDR_WIFI_PASSWORD, password);

        // read data
        Serial.print("READ SAVE WIFI STATE:");
        Serial.println(eeprom_util.read(&EepromParam.ADDR_WIFI_STATE));
        Serial.print("READ SAVE WIFI SSID:");
        Serial.println(eeprom_util.read(&EepromParam.ADDR_WIFI_SSID));
        Serial.print("READ SAVE WIFI PASSWORD:");
        Serial.println(eeprom_util.read(&EepromParam.ADDR_WIFI_PASSWORD));

    } else if (type == MESSAGE_TYPE.SYS_WEB_SOCKET_SERVER) {
        String host = doc["host"];
        uint16_t port = doc["port"];
        String url = doc["url"];

        // save data
        eeprom_util.write(&EepromParam.ADDR_WEB_SOCKET_HOST, host);
        eeprom_util.writeUint16T(&EepromParam.ADDR_WEB_SOCKET_PORT, port);
        eeprom_util.write(&EepromParam.ADDR_WEB_SOCKET_URL, url);

        // read data
        Serial.print("READ SAVE WEB SOCKET HOST:");
        Serial.println(eeprom_util.read(&EepromParam.ADDR_WEB_SOCKET_HOST));
        Serial.print("READ SAVE WEB SOCKET PORT:");
        Serial.println(eeprom_util.readToUint16T(&EepromParam.ADDR_WEB_SOCKET_PORT));
        Serial.print("READ SAVE WEB SOCKET URL:");
        Serial.println(eeprom_util.read(&EepromParam.ADDR_WEB_SOCKET_URL));
    } else if (type == MESSAGE_TYPE.SYS_RESTART) {
        ESP.restart();
    }
}

void RobotProtocol::parseBasic(StaticJsonDocument<300> &doc)
{

    printDoc(doc);
    isSys(doc);
    _now_buf[2] = BASIC;

    String dir = doc["dir"];
    if(dir == "stop") {
        _now_buf[3] = STOP;
        wrobot.dir = STOP;  
    } else {
        if(dir == "jump") {
            _now_buf[3] = JUMP;
            wrobot.dir = JUMP;
        }else if(dir == "forward") {
            _now_buf[3] = FORWARD;
            wrobot.dir = FORWARD;
        }else if(dir == "back") {
            _now_buf[3] = BACK;
            wrobot.dir = BACK;
        }else if(dir == "left") {
            _now_buf[3] = LEFT;
            wrobot.dir = LEFT;
        }else if(dir == "right") {
            _now_buf[3] = RIGHT;
            wrobot.dir = RIGHT;
        }else{
            _now_buf[3] = STOP;
            wrobot.dir = STOP;
        }
    }

    int height = doc["height"];
    _now_buf[4] = height;
    wrobot.height = height;

    int roll = doc["roll"];
    wrobot.roll = roll;
    if(roll >= 0) {
        _now_buf[5] = 0;
    }else {
        _now_buf[5] = 1;
    }
    _now_buf[6] = abs(roll);    
    
    int linear = doc["linear"];
    wrobot.linear = linear;
    if(linear >= 0) {
        _now_buf[7] = 0;
    }else {
        _now_buf[7] = 1;
    }
    _now_buf[8] = abs(linear); 
    
    int angular = doc["angular"];
    wrobot.angular = angular;
    if(angular >= 0) {
        _now_buf[9] = 0;
    }else {
        _now_buf[9] = 1;
    }
    _now_buf[10] = abs(angular); 
   

    int stable = doc["stable"];
    wrobot.go = stable;
    if(stable) {
        _now_buf[11] = 1;
    }else {
        _now_buf[11] = 0;
    }
    
    int joy_x = doc["joy_x"];
    wrobot.joyx = joy_x;
    if(joy_x >= 0) {
        _now_buf[12] = 0;
    }else {
        _now_buf[12] = 1;
    }
    _now_buf[13] = abs(joy_x); 


    int joy_y = doc["joy_y"];
    wrobot.joyy = joy_y;
    if(joy_y >= 0) {
        _now_buf[14] = 0;
    }else {
        _now_buf[14] = 1;
    }
    _now_buf[15] = abs(joy_y); 
}
