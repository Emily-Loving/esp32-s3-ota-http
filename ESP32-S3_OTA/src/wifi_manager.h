#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <IPAddress.h>

//WiFi 连接管理 先尝试 STA 连路由器 失败则回退到AP热点模式。

//自定义命名 避免与 ESP-IDF 自带的 wifi_mode_t/WIFI_MODE_STA 冲突
typedef enum
{
    OWM_NONE = 0,   // 未连接
    OWM_STA,        // 已连上路由器  STA
    OWM_AP          // 自身开热点  AP
} ota_wifi_mode_t;

void wifi_init(void);
ota_wifi_mode_t wifi_get_mode(void);
IPAddress wifi_get_ip(void);
String wifi_get_mac_string(void);
String wifi_get_status_string(void);

#endif 
