#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

//Web 管理服务器：提供管理页面、固件上传、状态查询

void web_server_init(void);
void web_server_loop(void);

#endif 
