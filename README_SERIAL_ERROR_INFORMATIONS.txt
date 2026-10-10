ESP32 OTA 工程 — 串口错误信息一览
（出现以下任意一行即代表对应环节出错）

【WiFi 相关】
[WiFi] STA timeout, switch to AP mode.     	        → 连路由器超时，已切回 AP 热点
[WiFi] STA SSID not configured, skip to AP mode.  	→ 未配置路由器，直接开热点
[WiFi] AP start FAILED!                       				→ 热点启动失败

【ESP32 自身升级 (OTA) 相关】
[ESP32-OTA] begin FAILED: size=0            		  	→ 固件大小为 0，上传的是空文件
[ESP32-OTA] begin FAILED: <原因>               		→ 升级初始化失败（见原因）
[ESP32-OTA] write REJECTED: <原因>             		→ 首字节校验未通过，拒绝写入
[ESP32-OTA] write FAILED: wrote x/y, err=<原因> 	→ 写入中途失败
[ESP32-OTA] end FAILED: <原因>                 		→ 收尾校验失败，固件无效
[ESP32-OTA] aborted                            			→ 升级被中止

【正常成功（非错误，供对照）】
[ESP32-OTA] SUCCESS, rebooting...              		→ 升级成功，正在重启
[WiFi] STA connected, IP: xxx                  			→ 已连上路由器
[WiFi] AP mode, SSID: 'xxx', IP: 192.168.4.1   		→ 热点已开启
[WEB] server started on port 80                			→ 网页服务已启动
[MAIN] open http://xxx in browser              		→ 全部就绪


/////English


ESP32 OTA Project - Serial Error Messages
(Any of the following lines indicates an error in that stage)

[WiFi related]
[WiFi] STA timeout, switch to AP mode.              		-> Router connection timed out, switched to AP hotspot
[WiFi] STA SSID not configured, skip to AP mode.    	-> No router configured, starting hotspot directly
[WiFi] AP start FAILED!                             				-> Hotspot failed to start

[ESP32 self-update (OTA) related]
[ESP32-OTA] begin FAILED: size=0                    			-> Firmware size is 0, uploaded an empty file
[ESP32-OTA] begin FAILED: <reason>                 	 	-> Update init failed (see reason)
[ESP32-OTA] write REJECTED: <reason>                		-> First-byte check failed, write rejected
[ESP32-OTA] write FAILED: wrote x/y, err=<reason>   	-> Write failed mid-way
[ESP32-OTA] end FAILED: <reason>                    		-> Final verification failed, firmware invalid
[ESP32-OTA] aborted                                 				-> Update aborted

[Successful / normal (for reference, not errors)]
[ESP32-OTA] SUCCESS, rebooting...                   			-> Update succeeded, rebooting
[WiFi] STA connected, IP: xxx                       			-> Connected to router
[WiFi] AP mode, SSID: 'xxx', IP: 192.168.4.1        		-> Hotspot started
[WEB] server started on port 80                     			-> Web server started
[MAIN] open http://xxx in browser                   			-> All ready

