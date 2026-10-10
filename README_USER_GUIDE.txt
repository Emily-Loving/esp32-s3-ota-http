
演示视频：


【功能】
  1. 网页上传固件，刷写 ESP32 自身
  2. ESP32 当无线桥，通过 UART2 给 STM32 升级
  3. WiFi 双模式：先连路由器(STA)，失败自动开热点(AP)

  步骤 1：烧录固件到 ESP32
  -------------------------
  用 USB 线把 ESP32-S3 连电脑。
  在 VSCode 里打开本工程，底部状态栏点 "→"(Upload) 按钮，


  步骤 2：打开串口监视器看 IP
  -------------------------
  VSCode 底部点 "插头" 图标（Serial Monitor），波特率 115200。
  会看到类似：
      [WiFi] STA SSID not configured, skip to AP mode.
      [WiFi] AP mode, SSID: 'ESP32-OTA-xxxx', IP: 192.168.4.1
      [WEB] server started on port 80
  
  热点IP 固定 192.168.4.1。

  步骤 3：手机/电脑连 ESP32 的热点
  -------------------------
  WiFi 列表里找 "ESP32-OTA-xxxx"，连接。
  密码: 12345678   （可在 src/config.h 里改 WIFI_AP_PASSWORD）

  步骤 4：浏览器打开管理页
  -------------------------
  浏览器输入:  http://192.168.4.1  （如果用的AP）
  能看到页面（版本号、MAC、IP、芯片型号、升级目标下拉框、选文件按钮）





///English


video：


ESP32 OTA Toolbox - Usage Guide

[Features]
  1. Upload firmware via web page to flash the ESP32 itself
  2. ESP32 acts as a wireless bridge, upgrading an STM32 over UART2
  3. WiFi dual mode: try router (STA) first, auto fall back to hotspot (AP) on failure

  Step 1: Flash firmware to the ESP32
  -------------------------
  Connect the ESP32-S3 to your PC with a USB cable.
  Open this project in VSCode, then click the "->" (Upload) button on the bottom status bar.

  Step 2: Open the serial monitor to see the IP
  -------------------------
  Click the "plug" icon at the bottom of VSCode (Serial Monitor), baud rate 115200.
  You will see something like:
      [WiFi] STA SSID not configured, skip to AP mode.
      [WiFi] AP mode, SSID: 'ESP32-OTA-xxxx', IP: 192.168.4.1
      [WEB] server started on port 80

  The hotspot IP is fixed at 192.168.4.1.

  Step 3: Connect your phone/PC to the ESP32 hotspot
  -------------------------
  Find "ESP32-OTA-xxxx" in the WiFi list and connect.
  Password: 12345678   (change WIFI_AP_PASSWORD in src/config.h)

  Step 4: Open the management page in a browser
  -------------------------
  Enter in the browser:  http://192.168.4.1  (if using the AP)
  You will see the page (version, MAC, IP, chip model, update-target dropdown, file-select button).
