#include "ota_esp32.h"
#include <Update.h>

/// @file ota_esp32.cpp
/// @brief ESP32 自身 OTA 实现（Arduino Update 库）。

static String s_str_last_err = "";   // 最近一次错误原因
static char   s_ac_head[32] = {0};  // 首块数据前8字节的hex
static bool   s_b_first = true;     // 是否为首个数据块

/// @brief 获取最近一次 OTA 错误原因（供网页/串口显示）
/// @param 无
/// @return String 错误描述
String ota_esp32_last_error(void)
{
    if (s_ac_head[0])
        {
            return s_str_last_err + String(" | head=") + String(s_ac_head);
        }
    return s_str_last_err;
}

/// @brief 开始一次 ESP32 OTA
/// @param u32_size 固件总字节数
/// @return bool true=开始成功；false=失败
bool ota_esp32_begin(uint32_t u32_size)
{
    s_str_last_err = "";
    s_ac_head[0] = 0;
    s_b_first = true;
    Serial.printf("[ESP32-OTA] begin, size=%u\n", u32_size);
    if (u32_size == 0)
    {
        s_str_last_err = "size is 0 (no file selected?)";
        Serial.println("[ESP32-OTA] begin FAILED: size=0");
        return false;
    }
    if (!Update.begin(u32_size))
    {
        s_str_last_err = Update.errorString();
        Serial.printf("[ESP32-OTA] begin FAILED: %s\n", s_str_last_err.c_str());
        return false;
    }
    return true;
}

/// @brief 写入一段固件数据
/// @param p_data 数据指针
/// @param u32_len 数据长度
/// @return bool true=写入成功
bool ota_esp32_write(const uint8_t *p_data, uint32_t u32_len)
{
    //记录第一块数据的前8字节
    if (s_b_first && u32_len >= 4)
    {
        snprintf(s_ac_head, sizeof(s_ac_head), "%02X %02X %02X %02X %02X %02X %02X %02X",
                 p_data[0], p_data[1], p_data[2], p_data[3],
                 p_data[4], p_data[5], p_data[6], p_data[7]);
        Serial.printf("[ESP32-OTA] first chunk len=%u, head: %s\n", (unsigned)u32_len, s_ac_head);
        s_b_first = false;
        
        //首字节必须是0xE9  否则数据已损坏，拒绝写入
        if (p_data[0] != 0xE9)
        {
            s_str_last_err = String("bad magic 0x") + String(p_data[0], HEX) +
                             " (upload data corrupted, not firmware)";
            Serial.printf("[ESP32-OTA] write REJECTED: %s\n", s_str_last_err.c_str());
            return false;
        }
    }
    
    size_t u32_written = Update.write((uint8_t *)p_data, u32_len);
    if (u32_written != u32_len)
    {
        s_str_last_err = Update.errorString();
        Serial.printf("[ESP32-OTA] write FAILED: wrote %u/%u, err=%s\n",
                      (unsigned)u32_written, (unsigned)u32_len, s_str_last_err.c_str());
        return false;
    }
    return true;
}

/// @brief 结束OTA，校验并重启
/// @param 无
/// @return bool true=成功 设备重启，false=失败 不重启
bool ota_esp32_end(void)
{
    if (Update.end(true))
    {
        Serial.println("[ESP32-OTA] SUCCESS, rebooting...");
        return true;   // 紧接着 ESP.restart()
    }
    else
    {
        s_str_last_err = Update.errorString();
        Serial.printf("[ESP32-OTA] end FAILED: %s\n", s_str_last_err.c_str());
        Update.abort();
        return false;
    }
}

/// @brief 中止当前 OTA
/// @param 无
/// @return 无
void ota_esp32_abort(void)
{
    Update.abort();
    Serial.println("[ESP32-OTA] aborted");
}
