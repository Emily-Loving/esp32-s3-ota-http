#ifndef CONFIG_H
#define CONFIG_H



//        修改这里即可适配你的环境哦


/* ===================== 固件版本 ===================== */
#define FW_VERSION_STRING   "1.0.0"   // 固件版本号，网页与串口会显示

/* ===================== WiFi 配置 ===================== */
//填入路由器 SSID 和密码  留空则跳过 STA 直接进 AP
#define WIFI_STA_SSID       ""
#define WIFI_STA_PASSWORD   ""
#define WIFI_STA_TIMEOUT_MS 15000     // STA 连接超时 单位ms，超时后切 AP

// AP 模式配置
#define WIFI_AP_SSID_PREFIX "ESP32-OTA-"   // AP 热点名前缀，后面会拼 MAC 后 4 位 hex
#define WIFI_AP_PASSWORD    "12345678"     // AP 密码  至少8位，设为空字符串则开放

/* ===================== Web 服务器 ===================== */
#define WEB_PORT            80        // Web 管理页端口

#endif 
