#include "web_socket_client_util.h"
#include <WebSocketsClient.h>
#include "robot.h"

MyWebSocketsClientData my_wss_data;

WebSocketsClient webSocketClient = WebSocketsClient();

void eventCallback(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      {
        printf("WEB SOCKET CLIENT:DISCONNECTED !!\n");
        rp.socket_connected = true;
        break;
      }


    case WStype_CONNECTED:
      {
        printf("WEB SOCKET CLIENT:CONNECTED !!\n");
        rp.socket_connected = false;
      }

      break;
  }



  if (type == WStype_TEXT) {

    my_wss_data.length = length;

    my_wss_data.buffer = (char*)malloc(length+1);
    memcpy(my_wss_data.buffer,payload,length);
    *(my_wss_data.buffer+length+1) = 0;

    my_wss_data.state = WSS_STATE_WAITING_PROCSSING;


    return;

  }
}

void web_sockets_client_init(void) {

  if (get_wifi_state() == WIFI_STATE.CLIENT) {
    String web_socket_client_host = eeprom_util.read(&EepromParam.ADDR_WEB_SOCKET_HOST);
    uint16_t web_socket_client_port = eeprom_util.readToUint16T(&EepromParam.ADDR_WEB_SOCKET_PORT);
    String web_socket_client_path = eeprom_util.read(&EepromParam.ADDR_WEB_SOCKET_PATH);

    if (web_socket_client_host.length() > 0 && web_socket_client_port > 0 && web_socket_client_path.length() > 0) {
      String cloud_token = rp.config_json[CONFIG_KEY.CLOUD_TOKEN];
      String connectionString = "wss://hub.navbot.com:443/ws/" + cloud_token;

      Serial.println("WebSocket Connecting Address: " + connectionString);

      if (443 == web_socket_client_port) {
        webSocketClient.beginSSL(web_socket_client_host, web_socket_client_port, web_socket_client_path);
      } else {
        webSocketClient.begin(web_socket_client_host, web_socket_client_port, web_socket_client_path);
      }

      // Set event callback handler function
      webSocketClient.onEvent(eventCallback);

      // Optional: Set the reconnection interval (in milliseconds)
      webSocketClient.setReconnectInterval(10000);

      // Ping every 15 seconds, with a 3-second timeout and 2 failed attempts resulting in disconnection.
      webSocketClient.enableHeartbeat(15000, 3000, 2);
    } else {
      String param = "{web_socket_client_host:" + web_socket_client_host + ",web_socket_client_port:" + web_socket_client_port + ",web_socket_client_path:" + web_socket_client_path + "}";
      Serial.println("Connecting to WebSocket: ERROR->Parameter verification failed->" + param);
    }
  }
}
void web_sockets_client_processing(void)
{
  if(my_wss_data.state == WSS_STATE_WAITING_PROCSSING)
  {
    my_wss_data.state = WSS_STATE_IDLE;
    delay(0);
    String payload_str = String((char *)my_wss_data.buffer);
    Serial.print("Web Socket Client Receive A Message:");
    Serial.println(payload_str);

    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, payload_str);

    printf("eFuse Two Point: Supported\n");
    String mode_str = doc["mode"];
    rp.parseJson(doc);
  }
}


void web_sockets_client_loop(void) {
  web_sockets_client_processing();
  webSocketClient.loop();
}


void web_sockets_client_send_message(String value) {
  if (rp.socket_connected == true) {
    webSocketClient.sendTXT(value);
  }
}
