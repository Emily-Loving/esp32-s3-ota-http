#include "web_server.h"
#include "config.h"
#include "wifi_manager.h"
#include "ota_esp32.h"
#include "banner_img.h"
#include <ESPAsyncWebServer.h>
#include <Update.h>

/// @brief Web 管理服务器实现（异步 AsyncWebServer，大文件上传不阻塞）。

static AsyncWebServer s_server(WEB_PORT);

/// @brief 上传过程的全局状态（供 /status 查询）
static volatile bool     s_b_busy = false;
static volatile uint32_t s_u32_done = 0;
static volatile uint32_t s_u32_total = 0;
static String            s_str_stage = "idle";
static String            s_str_result = "";

/// @brief 单次上传的上下文（异步回调间共享）
static String  s_str_target = "";   // 升级目标

/* ---------- HTML 管理页面 ---------- */
static const char PC_INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 OTA 工具箱</title>
<style>
  body{font-family:Arial,sans-serif;max-width:560px;margin:30px auto;padding:0 16px;color:#222}
  h1{font-size:22px}
  .card{border:1px solid #ddd;border-radius:8px;padding:14px 16px;margin-bottom:14px;background:#fafafa}
  .info{font-size:13px;color:#555;line-height:1.7}
  label{display:block;margin:12px 0 4px;font-weight:bold}
  select,input[type=file]{width:100%;padding:8px;font-size:14px;box-sizing:border-box}
  button{margin-top:14px;padding:10px 20px;font-size:15px;cursor:pointer;background:#2563eb;color:#fff;border:none;border-radius:6px}
  button:disabled{background:#9ca3af;cursor:not-allowed}
  #log{margin-top:14px;padding:10px;background:#111;color:#0f0;font-family:monospace;font-size:12px;
       height:160px;overflow-y:auto;border-radius:6px;white-space:pre-wrap}
  .bar{width:100%;height:14px;background:#eee;border-radius:7px;overflow:hidden;margin-top:6px}
  .bar>div{height:100%;width:0;background:#2563eb;transition:width .2s}
</style>
</head>
<body>
<h1>ESP32 远程 OTA 工具箱</h1>
<div style="text-align:center;margin:6px 0 14px">
  <img src="/banner.jpg" alt="banner" style="max-width:100%;width:360px;border-radius:10px">
</div>
<div class="card info">
  版本: <span id="ver">-</span><br>
  MAC: <span id="mac">-</span><br>
  网络: <span id="net">-</span><br>
  芯片: <span id="chip">-</span>
</div>
<div class="card">
  <label>选择固件 (.bin)</label>
  <input type="file" id="file" accept=".bin">
  <button id="btn" onclick="doUpload()">开始升级</button>
  <div class="bar"><div id="prog"></div></div>
</div>
<div id="log">等待操作...</div>
<script>
const $=id=>document.getElementById(id);
function log(t){const l=$('log');l.textContent+=t+'\n';l.scrollTop=l.scrollHeight;}
fetch('/info').then(r=>r.json()).then(j=>{
  $('ver').textContent=j.version;$('mac').textContent=j.mac;
  $('net').textContent=j.net;$('chip').textContent=j.chip;
}).catch(()=>log('获取设备信息失败'));
function doUpload(){
  const f=$('file').files[0];if(!f){log('请先选择文件');return;}
  const btn=$('btn');btn.disabled=true;$('prog').style.width='0%';
  log('开始上传 '+f.name+' ('+f.size+' bytes)');
  const fd=new FormData();fd.append('file',f);
  const xhr=new XMLHttpRequest();
  xhr.open('POST','/update');
  xhr.upload.onprogress=e=>{if(e.lengthComputable){const p=Math.round(e.loaded/e.total*100);
    $('prog').style.width=p+'%';log('上传进度 '+p+'%');}};
  xhr.onload=()=>{log('服务器响应: '+xhr.responseText);
    if(xhr.status==200 && xhr.responseText.includes('OK')){
      log('=== 升级成功 ===');log('设备即将重启...');
    }else{log('=== 升级失败 ===');}
    btn.disabled=false;};
  xhr.onerror=()=>{log('网络错误');btn.disabled=false;};
  xhr.send(fd);
}
</script>
</body>
</html>
)HTML";

/// @brief 处理根路径：返回管理页
static void handle_root(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *resp =
        request->beginResponse_P(200, "text/html", PC_INDEX_HTML);
    resp->addHeader("Cache-Control", "no-cache");
    request->send(resp);
}

/// @brief 处理 /banner.jpg
static void handle_banner(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *resp =
        request->beginResponse_P(200, "image/jpeg", (const uint8_t *)banner_img, BANNER_IMG_LEN);
    resp->addHeader("Cache-Control", "max-age=86400");
    request->send(resp);
}

/// @brief 处理 /info：返回设备信息 JSON
static void handle_info(AsyncWebServerRequest *request)
{
    String c_json = "{";
    c_json += "\"version\":\"" + String(FW_VERSION_STRING) + "\",";
    c_json += "\"mac\":\"" + wifi_get_mac_string() + "\",";
    c_json += "\"net\":\"" + wifi_get_status_string() + "\",";
    c_json += "\"chip\":\"ESP32-S3\"";
    c_json += "}";
    request->send(200, "application/json", c_json);
}

/// @brief 处理 /status：返回上传进度 JSON
static void handle_status(AsyncWebServerRequest *request)
{
    String c_json = "{";
    c_json += "\"busy\":" + String(s_b_busy ? "true" : "false") + ",";
    c_json += "\"done\":" + String((uint32_t)s_u32_done) + ",";
    c_json += "\"total\":" + String((uint32_t)s_u32_total) + ",";
    c_json += "\"stage\":\"" + s_str_stage + "\",";
    c_json += "\"result\":\"" + s_str_result + "\"";
    c_json += "}";
    request->send(200, "application/json", c_json);
}

/// @brief 上传 body 分块到达回调（AsyncWebServer 异步调用，不阻塞）
static void handle_upload_body(AsyncWebServerRequest *request, const String &filename,
                               size_t index, uint8_t *data, size_t len, bool final)
{
    if (s_str_target == "esp32")
    {
        if (index == 0)
        {
            s_u32_total = request->contentLength();
            if (!ota_esp32_begin(s_u32_total))
            {
                s_str_result = "begin failed: " + ota_esp32_last_error();
                request->send(500, "text/plain", "ERROR: " + s_str_result);
                return;
            }
            s_str_stage = "flashing";
        }
        if (s_str_result.length() == 0 || s_str_result == "OK")
        {
            if (!ota_esp32_write(data, (uint32_t)len))
            {
                s_str_result = "write failed: " + ota_esp32_last_error();
            }
        }
        s_u32_done = index + len;
        if (final)
        {
            if (ota_esp32_end())
            {
                s_str_result = "OK";
                request->send(200, "text/plain", "OK: esp32 updated, rebooting");
                delay(200);
                ESP.restart();
            }
            else
            {
                s_str_result = "end failed: " + ota_esp32_last_error();
                request->send(500, "text/plain", "ERROR: " + s_str_result);
            }
        }
    }
}

/// @brief 处理 /update POST：初始化上传上下文
static void handle_update(AsyncWebServerRequest *request)
{
    s_b_busy = true;
    s_str_result = "";
    s_str_stage = "uploading";
    s_u32_done = 0;
    s_str_target = "esp32";
    if (request->contentLength() == 0)
    {
        s_b_busy = false;
        request->send(400, "text/plain", "ERROR: no file");
    }
}

/// @brief 启动 Web 服务器（注册路由，开始监听 80 端口）
/// @param 无
/// @return 无
void web_server_init(void)
{
    s_server.on("/", HTTP_GET, handle_root);
    s_server.on("/info", HTTP_GET, handle_info);
    s_server.on("/status", HTTP_GET, handle_status);
    s_server.on("/banner.jpg", HTTP_GET, handle_banner);
    s_server.on("/update", HTTP_POST, handle_update, handle_upload_body);
    s_server.begin();
    Serial.printf("[WEB] server started on port %d\n", WEB_PORT);
}

/// @brief 主循环中调用，处理客户端请求
/// @param 无
/// @return 无
void web_server_loop(void)
{
    ;
}
