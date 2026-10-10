#include "wifi_manager.h"
#include "config.h"
#include <WiFi.h>

/// @file wifi_manager.cpp
/// @brief WiFi 连接管理实现。

static ota_wifi_mode_t s_e_wifi_mode = OWM_NONE;   // 当前 WiFi 模式
static IPAddress   s_st_ip;                          // 当前 IP 地址

/// @brief 生成 AP 模式的 SSID：前缀 + MAC 后 4 位 hex（保证不撞名）
/// @param 无
/// @return String 
static String wifi_make_ap_ssid(void)
{
    String c_str_mac = WiFi.macAddress();
    //MAC
    c_str_mac.replace(":", "");
    String c_str_tail = c_str_mac.substring(c_str_mac.length() - 4);
    c_str_tail.toUpperCase();
    return String(WIFI_AP_SSID_PREFIX) + c_str_tail;
}

/// @brief 启动 WiFi  STA超时失败则切AP
/// @param 无
/// @return 无
void wifi_init(void)
{
    WiFi.mode(WIFI_STA);
    String c_str_ssid = String(WIFI_STA_SSID);

    if (c_str_ssid.length() > 0 && c_str_ssid != "YourWiFiSSID")
    {
        Serial.printf("[WiFi] STA connecting to '%s' ...\n", WIFI_STA_SSID);
        WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASSWORD);

        uint32_t u32_start = millis();
        while (WiFi.status() != WL_CONNECTED)
        {
            if (millis() - u32_start > WIFI_STA_TIMEOUT_MS)
            {
                Serial.println("[WiFi] STA timeout, switch to AP mode.");
                break;
            }
            delay(500);
            Serial.print(".");
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED)
        {
            s_e_wifi_mode = OWM_STA;
            s_st_ip = WiFi.localIP();
            Serial.printf("[WiFi] STA connected, IP: %s\n", s_st_ip.toString().c_str());
            return;
        }
    }
    else
    {
        Serial.println("[WiFi] STA SSID not configured, skip to AP mode.");
    }

    //STA 失败 回退到AP模式  先彻底断开STA 清理状态再切AP
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(100);
    WiFi.mode(WIFI_AP);
    String c_str_ap_ssid = wifi_make_ap_ssid();

    bool b_ok;
    String c_str_ap_pass = String(WIFI_AP_PASSWORD);
    if (c_str_ap_pass.length() > 0)
    {
        b_ok = WiFi.softAP(c_str_ap_ssid.c_str(), c_str_ap_pass.c_str());
    }
        
    else
    {
        b_ok = WiFi.softAP(c_str_ap_ssid.c_str());
    }

    if (b_ok)
    {
        s_e_wifi_mode = OWM_AP;
        s_st_ip = WiFi.softAPIP();
        Serial.printf("[WiFi] AP mode, SSID: '%s', IP: %s\n",
                      c_str_ap_ssid.c_str(), s_st_ip.toString().c_str());
    }
    else
    {
        s_e_wifi_mode = OWM_NONE;
        Serial.println("[WiFi] AP start FAILED!");
    }
}

/// @brief 获取当前 WiFi 模式
/// @param 无
/// @return ota_wifi_mode_t 当前模式
ota_wifi_mode_t wifi_get_mode(void)
{
    return s_e_wifi_mode;
}

/// @brief 获取设备自身 IP（STA 下是路由器分配的 IP，AP 下是 192.168.4.1）
/// @param 无
/// @return IPAddress
IPAddress wifi_get_ip(void)
{
    return s_st_ip;
}

/// @brief 获取设备 MAC 地址字符串   格式 XX:XX:XX:XX:XX:XX
/// @param 无
/// @return String MAC 
String wifi_get_mac_string(void)
{
    return WiFi.macAddress();
}

/// @brief 获取人类可读的连接状态字符串（用于网页/串口显示）
/// @param 无
/// @return  STA或AP
String wifi_get_status_string(void)
{
    switch (s_e_wifi_mode)
    {
    case OWM_STA: return String("STA ") + s_st_ip.toString();
    case OWM_AP:  return String("AP ") + s_st_ip.toString();
    default:            return String("Disconnected");
    }
}
