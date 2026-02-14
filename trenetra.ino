/*
 * =============================================================
 *  TRINETRA - ESP32-CAM Surveillance System
 *  Main Sketch (trenetra.ino)
 * =============================================================
 *  Board:  AI-Thinker ESP32-CAM (with PSRAM)
 *  Mode:   Wi-Fi Access Point (AP)
 *  AP:     Trinetra_AP / 12345678
 *  URL:    http://192.168.4.1
 *  Stream: http://192.168.4.1:81/stream
 * =============================================================
 *  Features:
 *   - MJPEG live streaming (dual frame buffer)
 *   - Photo capture & download
 *   - Save photos to microSD card
 *   - Flash LED on/off control
 *   - Dynamic resolution & quality selection
 *   - Dark-themed responsive mobile UI
 * =============================================================
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include "FS.h"
#include "SD_MMC.h"
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif

// Board configuration (selects AI-Thinker + pin definitions)
#include "board_config.h"

// =======================
// Wi-Fi AP Configuration
// =======================
const char *ap_ssid     = "Trinetra";
const char *ap_password = "88888888";   // Set to "" for open network
// Simple IP Address: 1.2.3.4
IPAddress local_ip(1, 2, 3, 4);
IPAddress gateway(1, 2, 3, 4);
IPAddress subnet(255, 255, 255, 0);

// DNS Server for Captive Portal (auto-redirect)
DNSServer dnsServer;
const byte DNS_PORT = 53;

// =======================
// System Stats (exported for app_httpd.cpp)
// =======================
unsigned long systemStartTime = 0;
uint32_t currentFPS = 0;
unsigned long totalFrames = 0;
unsigned long lastFrameTime = 0;

// =======================
// SD Card availability flag (used by app_httpd.cpp)
// =======================
bool sdCardAvailable = false;

// =======================
// Function prototypes (defined in app_httpd.cpp)
// =======================
void startCameraServer();
void setupLedFlash();

// =======================
// Initialize SD Card (1-bit mode for AI-Thinker compatibility)
// =======================
void initSDCard() {
  Serial.println("[SD] Initializing SD card...");

  // AI-Thinker ESP32-CAM uses GPIO 4 for both flash LED and SD_MMC data line.
  // We init SD in 1-bit mode to free GPIO 4 for LED after SD init.
  if (!SD_MMC.begin("/sdcard", true)) {  // true = 1-bit mode
    Serial.println("[SD] Card mount failed or not present.");
    sdCardAvailable = false;
    return;
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("[SD] No SD card detected.");
    sdCardAvailable = false;
    return;
  }

  sdCardAvailable = true;
  Serial.printf("[SD] Card ready. Type: ");
  switch (cardType) {
    case CARD_MMC:  Serial.println("MMC");   break;
    case CARD_SD:   Serial.println("SD");    break;
    case CARD_SDHC: Serial.println("SDHC");  break;
    default:        Serial.println("Unknown"); break;
  }
  Serial.printf("[SD] Total: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
  Serial.printf("[SD] Used:  %lluMB\n", SD_MMC.usedBytes()  / (1024 * 1024));
}

// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println("=================================");
  Serial.println("   TRINETRA Surveillance System  ");
  Serial.println("   ESP32-CAM | AI-Thinker        ");
  Serial.println("=================================");

  // ----- Camera Configuration -----
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;            // 20 MHz XCLK
  config.pixel_format = PIXFORMAT_JPEG;       // JPEG for streaming
  config.frame_size   = FRAMESIZE_QVGA;       // Default: 320x240
  config.jpeg_quality = 12;                   // 0-63, lower = better quality
  config.fb_count     = 1;
  config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location  = CAMERA_FB_IN_PSRAM;

  // Optimize if PSRAM is available
  if (psramFound()) {
    Serial.println("[CAM] PSRAM found - using optimized settings");
    config.jpeg_quality = 10;                 // Better quality with PSRAM
    config.fb_count     = 2;                  // Dual frame buffer for smooth stream
    config.grab_mode    = CAMERA_GRAB_LATEST; // Always grab the latest frame
    config.frame_size   = FRAMESIZE_QVGA;     // Start at QVGA for fast streaming
  } else {
    Serial.println("[CAM] No PSRAM - using conservative settings");
    config.frame_size   = FRAMESIZE_QVGA;
    config.fb_location  = CAMERA_FB_IN_DRAM;
    config.fb_count     = 1;
  }

  // ----- Initialize Camera -----
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[CAM] Init FAILED with error 0x%x\n", err);
    Serial.println("[CAM] Rebooting in 3 seconds...");
    delay(3000);
    ESP.restart();
    return;
  }
  Serial.println("[CAM] Camera initialized successfully");

  // ----- Sensor Adjustments -----
  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }
  // Set initial frame size to QVGA for fast streaming
  s->set_framesize(s, FRAMESIZE_QVGA);

  // ----- Setup LED Flash -----
#if defined(LED_GPIO_NUM)
  setupLedFlash();
  Serial.println("[LED] Flash LED initialized");
#endif

  // ----- Initialize SD Card -----
  initSDCard();

  // ----- Start Wi-Fi Access Point -----
  Serial.println("[WiFi] Starting Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(ap_ssid, ap_password);
  delay(100);  // Brief delay for AP to stabilize

  // Start DNS Server for Captive Portal (redirects all DNS requests to our IP)
  dnsServer.start(DNS_PORT, "*", local_ip);
  Serial.println("[DNS] Captive portal DNS started");

  // ----- Setup mDNS (hostname) -----
  if (MDNS.begin("trinetra")) {
    Serial.println("[mDNS] Hostname: trinetra.local");
    MDNS.addService("http", "tcp", 80);
  } else {
    Serial.println("[mDNS] Failed to start");
  }

  Serial.printf("[WiFi] AP SSID:     %s\n", ap_ssid);
  Serial.printf("[WiFi] AP Password: %s\n", ap_password);
  Serial.printf("[WiFi] AP IP:       %s\n", local_ip.toString().c_str());

  // ----- Start Web Server -----
  startCameraServer();

  // ----- Setup OTA (Over-The-Air Updates) -----
  ArduinoOTA.setHostname("trinetra");
  ArduinoOTA.setPassword("trinetra123");  // Change this password!
  
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("[OTA] Start updating " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\n[OTA] Update complete!");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();
  Serial.println("[OTA] Ready for updates");

  // ----- Record system start time -----
  systemStartTime = millis();

  Serial.println("=================================");
  Serial.println("   TRINETRA is READY!            ");
  Serial.printf("   Web UI:  http://%s\n", local_ip.toString().c_str());
  Serial.printf("   Or use:  http://trinetra.local\n");
  Serial.printf("   Stream:  http://%s:81/stream\n", local_ip.toString().c_str());
  Serial.println("   Connect to WiFi - Auto Opens! ");
  Serial.println("   OTA Password: trinetra123     ");
  Serial.println("=================================");
}

// =======================
// LOOP - Process DNS, OTA, and stats tracking
// =======================
void loop() {
  dnsServer.processNextRequest();  // Handle DNS for captive portal redirection
  ArduinoOTA.handle();              // Handle OTA updates
  delay(10);
}
