/*
 * =============================================================
 *  TRINETRA - ESP32-CAM Surveillance System
 *  HTTP Server & Handlers (app_httpd.cpp)
 * =============================================================
 *  Supports multiple simultaneous viewers in STA+AP mode
 *  
 *  Routes:
 *    /          -> Serve HTML UI
 *    /stream    -> MJPEG live stream (port 81)
 *    /capture   -> Capture single JPEG frame
 *    /save-photo-> Capture & save JPEG to SD card
 *    /control   -> Set camera params (framesize, quality, etc.)
 *    /status    -> JSON status of camera sensor
 *    /led       -> Flash LED on/off
 *    /system-stats -> Real-time system monitoring data
 *    /wifi-scan    -> Scan available WiFi networks
 *    /wifi-connect -> Connect to selected network
 *    /wifi-status  -> Get WiFi connection status
 *    /wifi-reset   -> Clear saved WiFi credentials
 * =============================================================
 */

#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "esp32-hal-ledc.h"
#include "sdkconfig.h"
#include "camera_index.h"
#include "board_config.h"

// SD card headers
#include "FS.h"
#include "SD_MMC.h"

// WiFi for RSSI reading
#include <WiFi.h>

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#endif

// =======================
// LED Flash Configuration
// =======================
#if defined(LED_GPIO_NUM)
#define CONFIG_LED_MAX_INTENSITY 255
int led_duty = 0;
bool isStreaming = false;
#endif

// =======================
// SD Card State
// =======================
extern bool sdCardAvailable;  // Defined in trenetra.ino

// =======================
// System Stats (from trenetra.ino)
// =======================
extern unsigned long systemStartTime;
extern uint32_t currentFPS;
extern unsigned long totalFrames;
extern unsigned long lastFrameTime;

// =======================
// WiFi Manager Functions (from trenetra.ino)
// =======================
extern String scanNetworks();
extern void saveWiFiCredentials(const char* ssid, const char* password);
extern void clearWiFiCredentials();
extern String getWiFiStatus();
extern bool connectToWiFi(const char* ssid, const char* password, int timeoutSeconds);
extern bool wifiConnected;

// =======================
// MJPEG Stream Boundary
// =======================
#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

// =======================
// Rolling Average Filter (for FPS logging)
// =======================
typedef struct {
  size_t size;
  size_t index;
  size_t count;
  int sum;
  int *values;
} ra_filter_t;

static ra_filter_t ra_filter;

static ra_filter_t *ra_filter_init(ra_filter_t *filter, size_t sample_size) {
  memset(filter, 0, sizeof(ra_filter_t));
  filter->values = (int *)malloc(sample_size * sizeof(int));
  if (!filter->values) return NULL;
  memset(filter->values, 0, sample_size * sizeof(int));
  filter->size = sample_size;
  return filter;
}

#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
static int ra_filter_run(ra_filter_t *filter, int value) {
  if (!filter->values) return value;
  filter->sum -= filter->values[filter->index];
  filter->values[filter->index] = value;
  filter->sum += filter->values[filter->index];
  filter->index++;
  filter->index = filter->index % filter->size;
  if (filter->count < filter->size) filter->count++;
  return filter->sum / filter->count;
}
#endif

// =======================
// LED Enable/Disable
// =======================
#if defined(LED_GPIO_NUM)
void enable_led(bool en) {
  int duty = en ? led_duty : 0;
  if (en && isStreaming && (led_duty > CONFIG_LED_MAX_INTENSITY)) {
    duty = CONFIG_LED_MAX_INTENSITY;
  }
  ledcWrite(LED_GPIO_NUM, duty);
  log_i("Set LED intensity to %d", duty);
}
#endif

// =======================
// Photo Counter for SD filenames
// =======================
static uint32_t photoCounter = 0;

// ==================================================================
//  HANDLER: Serve the HTML UI
// ==================================================================
static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, index_html, strlen(index_html));
}

// ==================================================================
//  HANDLER: Capture single JPEG (download to browser)
// ==================================================================
static esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;

#if defined(LED_GPIO_NUM)
  enable_led(true);
  vTaskDelay(150 / portTICK_PERIOD_MS);
  fb = esp_camera_fb_get();
  enable_led(false);
#else
  fb = esp_camera_fb_get();
#endif

  if (!fb) {
    log_e("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  if (fb->format == PIXFORMAT_JPEG) {
    res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  } else {
    // Convert to JPEG if not already
    uint8_t *jpg_buf = NULL;
    size_t jpg_len = 0;
    bool converted = frame2jpg(fb, 80, &jpg_buf, &jpg_len);
    if (converted) {
      res = httpd_resp_send(req, (const char *)jpg_buf, jpg_len);
      free(jpg_buf);
    } else {
      log_e("JPEG conversion failed");
      res = ESP_FAIL;
      httpd_resp_send_500(req);
    }
  }

  esp_camera_fb_return(fb);
  return res;
}

// ==================================================================
//  HANDLER: Save photo to SD card
// ==================================================================
static esp_err_t save_photo_handler(httpd_req_t *req) {
  char json_response[256];
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  // Check if SD card is available
  if (!sdCardAvailable) {
    snprintf(json_response, sizeof(json_response),
             "{\"success\":false,\"error\":\"SD card not available\"}");
    return httpd_resp_send(req, json_response, strlen(json_response));
  }

  // Capture a frame
  camera_fb_t *fb = NULL;

#if defined(LED_GPIO_NUM)
  enable_led(true);
  vTaskDelay(150 / portTICK_PERIOD_MS);
  fb = esp_camera_fb_get();
  enable_led(false);
#else
  fb = esp_camera_fb_get();
#endif

  if (!fb) {
    snprintf(json_response, sizeof(json_response),
             "{\"success\":false,\"error\":\"Camera capture failed\"}");
    return httpd_resp_send(req, json_response, strlen(json_response));
  }

  // Build filename: /trinetra_XXXXX.jpg
  char filename[64];
  photoCounter++;
  snprintf(filename, sizeof(filename), "/trinetra_%05lu.jpg", (unsigned long)photoCounter);

  // Write to SD card
  File file = SD_MMC.open(filename, FILE_WRITE);
  if (!file) {
    esp_camera_fb_return(fb);
    snprintf(json_response, sizeof(json_response),
             "{\"success\":false,\"error\":\"Failed to open file on SD\"}");
    return httpd_resp_send(req, json_response, strlen(json_response));
  }

  size_t written = 0;
  if (fb->format == PIXFORMAT_JPEG) {
    written = file.write(fb->buf, fb->len);
  } else {
    // Convert to JPEG first
    uint8_t *jpg_buf = NULL;
    size_t jpg_len = 0;
    if (frame2jpg(fb, 80, &jpg_buf, &jpg_len)) {
      written = file.write(jpg_buf, jpg_len);
      free(jpg_buf);
    }
  }
  file.close();
  esp_camera_fb_return(fb);

  if (written > 0) {
    log_i("Photo saved: %s (%u bytes)", filename, written);
    snprintf(json_response, sizeof(json_response),
             "{\"success\":true,\"filename\":\"%s\",\"size\":%u}",
             filename, (unsigned)written);
  } else {
    snprintf(json_response, sizeof(json_response),
             "{\"success\":false,\"error\":\"Write failed\"}");
  }

  return httpd_resp_send(req, json_response, strlen(json_response));
}

// ==================================================================
//  HANDLER: MJPEG Live Stream
// ==================================================================
static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  struct timeval _timestamp;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;
  char *part_buf[128];

  static int64_t last_frame = 0;
  if (!last_frame) last_frame = esp_timer_get_time();

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) return res;

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  httpd_resp_set_hdr(req, "X-Framerate", "60");

#if defined(LED_GPIO_NUM)
  isStreaming = true;
#endif

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      log_e("Camera capture failed");
      res = ESP_FAIL;
    } else {
      _timestamp.tv_sec = fb->timestamp.tv_sec;
      _timestamp.tv_usec = fb->timestamp.tv_usec;

      if (fb->format != PIXFORMAT_JPEG) {
        bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
        esp_camera_fb_return(fb);
        fb = NULL;
        if (!jpeg_converted) {
          log_e("JPEG compression failed");
          res = ESP_FAIL;
        }
      } else {
        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;
      }
    }

    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if (res == ESP_OK) {
      size_t hlen = snprintf((char *)part_buf, 128, _STREAM_PART,
                             _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }

    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if (_jpg_buf) {
      free(_jpg_buf);
      _jpg_buf = NULL;
    }

    if (res != ESP_OK) {
      log_e("Send frame failed");
      break;
    }

    int64_t fr_end = esp_timer_get_time();
    int64_t frame_time = fr_end - last_frame;
    last_frame = fr_end;
    frame_time /= 1000;

    // Update global FPS stats
    totalFrames++;
    if (frame_time > 0) {
      currentFPS = 1000 / frame_time;
    }
    lastFrameTime = millis();

#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
    uint32_t avg_frame_time = ra_filter_run(&ra_filter, frame_time);
    log_i("MJPG: %uB %ums (%.1ffps), AVG: %ums (%.1ffps)",
          (uint32_t)(_jpg_buf_len), (uint32_t)frame_time,
          1000.0 / (uint32_t)frame_time, avg_frame_time,
          1000.0 / avg_frame_time);
#endif
  }

#if defined(LED_GPIO_NUM)
  isStreaming = false;
#endif

  return res;
}

// ==================================================================
//  HANDLER: Camera control (framesize, quality, etc.)
// ==================================================================
static esp_err_t parse_get(httpd_req_t *req, char **obuf) {
  char *buf = NULL;
  size_t buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char *)malloc(buf_len);
    if (!buf) {
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      *obuf = buf;
      return ESP_OK;
    }
    free(buf);
  }
  httpd_resp_send_404(req);
  return ESP_FAIL;
}

static esp_err_t cmd_handler(httpd_req_t *req) {
  char *buf = NULL;
  char variable[32];
  char value[32];

  if (parse_get(req, &buf) != ESP_OK) return ESP_FAIL;

  if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) != ESP_OK ||
      httpd_query_key_value(buf, "val", value, sizeof(value)) != ESP_OK) {
    free(buf);
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }
  free(buf);

  int val = atoi(value);
  log_i("%s = %d", variable, val);
  sensor_t *s = esp_camera_sensor_get();
  int res = 0;

  // Handle all supported camera parameters
  if (!strcmp(variable, "framesize")) {
    if (s->pixformat == PIXFORMAT_JPEG) {
      res = s->set_framesize(s, (framesize_t)val);
    }
  } else if (!strcmp(variable, "quality")) {
    res = s->set_quality(s, val);
  } else if (!strcmp(variable, "contrast")) {
    res = s->set_contrast(s, val);
  } else if (!strcmp(variable, "brightness")) {
    res = s->set_brightness(s, val);
  } else if (!strcmp(variable, "saturation")) {
    res = s->set_saturation(s, val);
  } else if (!strcmp(variable, "gainceiling")) {
    res = s->set_gainceiling(s, (gainceiling_t)val);
  } else if (!strcmp(variable, "colorbar")) {
    res = s->set_colorbar(s, val);
  } else if (!strcmp(variable, "awb")) {
    res = s->set_whitebal(s, val);
  } else if (!strcmp(variable, "agc")) {
    res = s->set_gain_ctrl(s, val);
  } else if (!strcmp(variable, "aec")) {
    res = s->set_exposure_ctrl(s, val);
  } else if (!strcmp(variable, "hmirror")) {
    res = s->set_hmirror(s, val);
  } else if (!strcmp(variable, "vflip")) {
    res = s->set_vflip(s, val);
  } else if (!strcmp(variable, "awb_gain")) {
    res = s->set_awb_gain(s, val);
  } else if (!strcmp(variable, "agc_gain")) {
    res = s->set_agc_gain(s, val);
  } else if (!strcmp(variable, "aec_value")) {
    res = s->set_aec_value(s, val);
  } else if (!strcmp(variable, "aec2")) {
    res = s->set_aec2(s, val);
  } else if (!strcmp(variable, "dcw")) {
    res = s->set_dcw(s, val);
  } else if (!strcmp(variable, "bpc")) {
    res = s->set_bpc(s, val);
  } else if (!strcmp(variable, "wpc")) {
    res = s->set_wpc(s, val);
  } else if (!strcmp(variable, "raw_gma")) {
    res = s->set_raw_gma(s, val);
  } else if (!strcmp(variable, "lenc")) {
    res = s->set_lenc(s, val);
  } else if (!strcmp(variable, "special_effect")) {
    res = s->set_special_effect(s, val);
  } else if (!strcmp(variable, "wb_mode")) {
    res = s->set_wb_mode(s, val);
  } else if (!strcmp(variable, "ae_level")) {
    res = s->set_ae_level(s, val);
  }
#if defined(LED_GPIO_NUM)
  else if (!strcmp(variable, "led_intensity")) {
    led_duty = val;
    if (isStreaming) enable_led(true);
  }
#endif
  else {
    log_i("Unknown command: %s", variable);
    res = -1;
  }

  if (res < 0) return httpd_resp_send_500(req);

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}

// ==================================================================
//  HANDLER: Camera status JSON
// ==================================================================
static esp_err_t status_handler(httpd_req_t *req) {
  static char json_response[1024];

  sensor_t *s = esp_camera_sensor_get();
  char *p = json_response;
  *p++ = '{';

  p += sprintf(p, "\"framesize\":%u,", s->status.framesize);
  p += sprintf(p, "\"quality\":%u,", s->status.quality);
  p += sprintf(p, "\"brightness\":%d,", s->status.brightness);
  p += sprintf(p, "\"contrast\":%d,", s->status.contrast);
  p += sprintf(p, "\"saturation\":%d,", s->status.saturation);
  p += sprintf(p, "\"sharpness\":%d,", s->status.sharpness);
  p += sprintf(p, "\"special_effect\":%u,", s->status.special_effect);
  p += sprintf(p, "\"wb_mode\":%u,", s->status.wb_mode);
  p += sprintf(p, "\"awb\":%u,", s->status.awb);
  p += sprintf(p, "\"awb_gain\":%u,", s->status.awb_gain);
  p += sprintf(p, "\"aec\":%u,", s->status.aec);
  p += sprintf(p, "\"aec2\":%u,", s->status.aec2);
  p += sprintf(p, "\"ae_level\":%d,", s->status.ae_level);
  p += sprintf(p, "\"aec_value\":%u,", s->status.aec_value);
  p += sprintf(p, "\"agc\":%u,", s->status.agc);
  p += sprintf(p, "\"agc_gain\":%u,", s->status.agc_gain);
  p += sprintf(p, "\"gainceiling\":%u,", s->status.gainceiling);
  p += sprintf(p, "\"bpc\":%u,", s->status.bpc);
  p += sprintf(p, "\"wpc\":%u,", s->status.wpc);
  p += sprintf(p, "\"raw_gma\":%u,", s->status.raw_gma);
  p += sprintf(p, "\"lenc\":%u,", s->status.lenc);
  p += sprintf(p, "\"hmirror\":%u,", s->status.hmirror);
  p += sprintf(p, "\"vflip\":%u,", s->status.vflip);
  p += sprintf(p, "\"dcw\":%u,", s->status.dcw);
  p += sprintf(p, "\"colorbar\":%u,", s->status.colorbar);
  p += sprintf(p, "\"sd_card\":%s", sdCardAvailable ? "true" : "false");

#if defined(LED_GPIO_NUM)
  p += sprintf(p, ",\"led_intensity\":%u", led_duty);
#else
  p += sprintf(p, ",\"led_intensity\":%d", -1);
#endif

  *p++ = '}';
  *p++ = 0;

  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json_response, strlen(json_response));
}

// ==================================================================
//  HANDLER: LED on/off control
// ==================================================================
static esp_err_t led_handler(httpd_req_t *req) {
  char *buf = NULL;
  char state_str[8];

  if (parse_get(req, &buf) != ESP_OK) return ESP_FAIL;

  if (httpd_query_key_value(buf, "state", state_str, sizeof(state_str)) != ESP_OK) {
    free(buf);
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }
  free(buf);

  int state = atoi(state_str);

#if defined(LED_GPIO_NUM)
  led_duty = state ? CONFIG_LED_MAX_INTENSITY : 0;
  enable_led(state ? true : false);
  log_i("LED set to %s", state ? "ON" : "OFF");
#endif

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}

// ==================================================================
//  HANDLER: System Stats (uptime, temp, FPS, WiFi, memory)
// ==================================================================
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

static esp_err_t system_stats_handler(httpd_req_t *req) {
  static char json_response[512];
  
  // Calculate uptime
  unsigned long uptime_ms = millis() - systemStartTime;
  unsigned long uptime_sec = uptime_ms / 1000;
  unsigned long uptime_min = uptime_sec / 60;
  unsigned long uptime_hrs = uptime_min / 60;
  unsigned long uptime_days = uptime_hrs / 24;
  
  // Get ESP32 temperature (Fahrenheit)
  float temp_f = (temprature_sens_read() - 32) / 1.8;
  float temp_c = temp_f;  // Already in Celsius
  
  // Get WiFi stats
  int rssi = WiFi.RSSI();  // Signal strength
  int clients = WiFi.softAPgetStationNum();  // Connected clients
  
  // Get memory stats
  uint32_t free_heap = ESP.getFreeHeap();
  uint32_t total_heap = ESP.getHeapSize();
  uint32_t free_psram = ESP.getFreePsram();
  uint32_t total_psram = ESP.getPsramSize();
  
  char *p = json_response;
  *p++ = '{';
  
  // Uptime
  p += sprintf(p, "\"uptime_days\":%lu,", uptime_days);
  p += sprintf(p, "\"uptime_hours\":%lu,", uptime_hrs % 24);
  p += sprintf(p, "\"uptime_minutes\":%lu,", uptime_min % 60);
  p += sprintf(p, "\"uptime_seconds\":%lu,", uptime_sec % 60);
  p += sprintf(p, "\"uptime_total_sec\":%lu,", uptime_sec);
  
  // Temperature
  p += sprintf(p, "\"temp_celsius\":%.1f,", temp_c);
  p += sprintf(p, "\"temp_fahrenheit\":%.1f,", (temp_c * 1.8) + 32);
  
  // Video stats
  p += sprintf(p, "\"fps\":%u,", currentFPS);
  p += sprintf(p, "\"total_frames\":%lu,", totalFrames);
  p += sprintf(p, "\"streaming\":%s,", isStreaming ? "true" : "false");
  
  // WiFi stats
  p += sprintf(p, "\"wifi_rssi\":%d,", rssi);
  p += sprintf(p, "\"wifi_clients\":%d,", clients);
  
  // Memory stats
  p += sprintf(p, "\"free_heap\":%u,", free_heap);
  p += sprintf(p, "\"total_heap\":%u,", total_heap);
  p += sprintf(p, "\"heap_usage\":%.1f,", 100.0 * (total_heap - free_heap) / total_heap);
  p += sprintf(p, "\"free_psram\":%u,", free_psram);
  p += sprintf(p, "\"total_psram\":%u,", total_psram);
  
  if (total_psram > 0) {
    p += sprintf(p, "\"psram_usage\":%.1f", 100.0 * (total_psram - free_psram) / total_psram);
  } else {
    p += sprintf(p, "\"psram_usage\":0.0");
  }
  
  *p++ = '}';
  *p++ = 0;
  
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json_response, strlen(json_response));
}

// ==================================================================
//  WiFi Manager Handlers
// ==================================================================

// Scan available WiFi networks
static esp_err_t wifi_scan_handler(httpd_req_t *req) {
  String networks = scanNetworks();
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, networks.c_str(), networks.length());
}

// Get WiFi status
static esp_err_t wifi_status_handler(httpd_req_t *req) {
  String status = getWiFiStatus();
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, status.c_str(), status.length());
}

// Connect to WiFi network
static esp_err_t wifi_connect_handler(httpd_req_t *req) {
  char buf[256];
  int ret, remaining = req->content_len;
  
  // Handle GET request with query params
  if (req->method == HTTP_GET) {
    size_t query_len = httpd_req_get_url_query_len(req) + 1;
    if (query_len > 1) {
      char query[256];
      if (httpd_req_get_url_query_str(req, query, query_len) == ESP_OK) {
        char ssid[64] = {0};
        char password[64] = {0};
        
        httpd_query_key_value(query, "ssid", ssid, sizeof(ssid));
        httpd_query_key_value(query, "password", password, sizeof(password));
        
        // URL decode the SSID and password
        // (simple decode - handle %20 and + for space)
        for (int i = 0; ssid[i]; i++) {
          if (ssid[i] == '+') ssid[i] = ' ';
        }
        for (int i = 0; password[i]; i++) {
          if (password[i] == '+') password[i] = ' ';
        }
        
        if (strlen(ssid) > 0) {
          // Try to connect
          bool success = connectToWiFi(ssid, password, 15);
          
          String json = "{";
          json += "\"success\":" + String(success ? "true" : "false") + ",";
          json += "\"ssid\":\"" + String(ssid) + "\",";
          if (success) {
            // Save credentials if connection successful
            saveWiFiCredentials(ssid, password);
            json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
            json += "\"message\":\"Connected and saved!\"";
          } else {
            json += "\"ip\":\"\",";
            json += "\"message\":\"Connection failed\"";
          }
          json += "}";
          
          httpd_resp_set_type(req, "application/json");
          httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
          return httpd_resp_send(req, json.c_str(), json.length());
        }
      }
    }
  }
  
  // Default error response
  const char* error = "{\"success\":false,\"message\":\"Missing SSID parameter\"}";
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, error, strlen(error));
}

// Reset/clear saved WiFi credentials
static esp_err_t wifi_reset_handler(httpd_req_t *req) {
  clearWiFiCredentials();
  WiFi.disconnect();
  wifiConnected = false;
  
  const char* response = "{\"success\":true,\"message\":\"WiFi credentials cleared. Device will use AP mode only until reconfigured.\"}";
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, response, strlen(response));
}

// ==================================================================
//  Start Camera Web Server
// ==================================================================
void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_uri_handlers = 16;

  // ---- URI definitions for the main HTTP server (port 80) ----
  httpd_uri_t index_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = index_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t status_uri = {
    .uri = "/status",
    .method = HTTP_GET,
    .handler = status_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t cmd_uri = {
    .uri = "/control",
    .method = HTTP_GET,
    .handler = cmd_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t capture_uri = {
    .uri = "/capture",
    .method = HTTP_GET,
    .handler = capture_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t save_photo_uri = {
    .uri = "/save-photo",
    .method = HTTP_GET,
    .handler = save_photo_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t led_uri = {
    .uri = "/led",
    .method = HTTP_GET,
    .handler = led_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t system_stats_uri = {
    .uri = "/system-stats",
    .method = HTTP_GET,
    .handler = system_stats_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  // ---- WiFi Manager URIs ----
  httpd_uri_t wifi_scan_uri = {
    .uri = "/wifi-scan",
    .method = HTTP_GET,
    .handler = wifi_scan_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t wifi_connect_uri = {
    .uri = "/wifi-connect",
    .method = HTTP_GET,
    .handler = wifi_connect_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t wifi_status_uri = {
    .uri = "/wifi-status",
    .method = HTTP_GET,
    .handler = wifi_status_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t wifi_reset_uri = {
    .uri = "/wifi-reset",
    .method = HTTP_GET,
    .handler = wifi_reset_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  // ---- URI for the stream server (port 81) ----
  httpd_uri_t stream_uri = {
    .uri = "/stream",
    .method = HTTP_GET,
    .handler = stream_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    , .is_websocket = true, .handle_ws_control_frames = false, .supported_subprotocol = NULL
#endif
  };

  // Initialize rolling-average filter for FPS logging
  ra_filter_init(&ra_filter, 20);

  // Start main HTTP server on port 80
  log_i("Starting web server on port: '%d'", config.server_port);
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &cmd_uri);
    httpd_register_uri_handler(camera_httpd, &status_uri);
    httpd_register_uri_handler(camera_httpd, &capture_uri);
    httpd_register_uri_handler(camera_httpd, &save_photo_uri);
    httpd_register_uri_handler(camera_httpd, &led_uri);
    httpd_register_uri_handler(camera_httpd, &system_stats_uri);
    // WiFi Manager
    httpd_register_uri_handler(camera_httpd, &wifi_scan_uri);
    httpd_register_uri_handler(camera_httpd, &wifi_connect_uri);
    httpd_register_uri_handler(camera_httpd, &wifi_status_uri);
    httpd_register_uri_handler(camera_httpd, &wifi_reset_uri);
  }

  // Start stream HTTP server on port 81
  config.server_port += 1;
  config.ctrl_port += 1;
  log_i("Starting stream server on port: '%d'", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

// ==================================================================
//  Setup LED Flash PWM
// ==================================================================
void setupLedFlash() {
#if defined(LED_GPIO_NUM)
  ledcAttach(LED_GPIO_NUM, 5000, 8);
#else
  log_i("LED flash is disabled -> LED_GPIO_NUM undefined");
#endif
}
