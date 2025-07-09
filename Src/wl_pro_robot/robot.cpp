
#include "robot.h"
#include <ArduinoJson.h>
#include "wifi.h"
#include "eeprom_util.h"

int uncontrolable = 0;
int BAT_PIN = 35;
esp_adc_cal_characteristics_t adc_chars;
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
// This undocumented internal function must be declared
extern "C" {
uint8_t temprature_sens_read();
}

double RobotProtocol::get_fahrenheit() {
  return temprature_sens_read();
}

double RobotProtocol::get_degree_centigrade() {
  double fahrenheit = get_fahrenheit();
  // Convert to Celsius (unit: Fahrenheit)
  return (fahrenheit - 32) / 1.8;
}

double RobotProtocol::get_battery_voltage() {
  // Voltage reading
  uint32_t sum = 0;
  sum = analogRead(BAT_PIN);
  uint32_t voltage = esp_adc_cal_raw_to_voltage(sum, &adc_chars);
  return (voltage * 3.97) / 1000.0;
}

double RobotProtocol::get_battery_level() {
  double currentVoltage = get_battery_voltage();
  double maxVoltage = 12.57;
  double minVoltage = 7.0;

  // Ensure the voltage is within a reasonable range
  if (currentVoltage >= maxVoltage) return 100.0;
  if (currentVoltage <= minVoltage) return 0.0;

  // Calculate the percentage
  double battery_percentage = (currentVoltage - minVoltage) / (maxVoltage - minVoltage) * 100.0;

  battery_percentage = round(battery_percentage) / 1; // Format to retain two significant digits (rounded to 2 decimal places)

  // Battery logic correction (prevent out-of-bounds and display of 0 data)
  if (battery_percentage > 100) {
    battery_percentage = 100;
  } else if (battery_percentage < 1) {
    battery_percentage = 1;
  }
  return battery_percentage;
}

int RobotProtocol::get_robot_status() {
  // 0 - Normal state, 1 - Primary out-of-control state (tilting), 2 - Recovery waiting state
  int robot_status = -1;
  if (uncontrolable == 0) {
    robot_status = 0;
  } else if (uncontrolable == 1) {
    robot_status = 1;
  } else if (uncontrolable > 1) {
    robot_status = 2;
  }
  return robot_status;
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
    } else if (type == MESSAGE_TYPE.GET_DEVICE_INFO) {
        feedback_util_send_message(FEEDBACK_CHANNEL.ALL);
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
