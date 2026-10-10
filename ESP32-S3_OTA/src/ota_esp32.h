#ifndef OTA_ESP32_H
#define OTA_ESP32_H

#include <Arduino.h>


// ESP32 自身 OTA：使用 Arduino Update 库把上传的 bin 写入 OTA 分区

String ota_esp32_last_error(void);
bool ota_esp32_begin(uint32_t u32_size);
bool ota_esp32_write(const uint8_t *p_data, uint32_t u32_len);
bool ota_esp32_end(void);
void ota_esp32_abort(void);

#endif 
