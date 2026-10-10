#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"
#include "web_server.h"

/// @file main.cpp
/// @brief 入口：初始化串口 -> WiFi -> Web 服务器 -> 主循环。

void setup()
{
    Serial.begin(115200);
    delay(200);
    Serial.println();
    Serial.println("===================================");
    Serial.printf(" ESP32 OTA Toolbox v%s\n", FW_VERSION_STRING);
    Serial.println("===================================");

    wifi_init();            //先STA  失败回退 AP
    web_server_init();      //启动 Web 管理页

    Serial.printf("[MAIN] open http://%s in browser\n", wifi_get_ip().toString().c_str());
}

void loop()
{
    web_server_loop();      // 处理HTTP请求
}
