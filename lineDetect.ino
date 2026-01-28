#include "WiFi.h"
#include "esp_camera.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
#include "video.h"
#include "web.h"
const char* ssid     = "Thegales";
const char* password = "talk2thegales";


const int ledFlash =4;
bool ledFlashState = true;
bool debug = true;

void ledON(){
  digitalWrite(ledFlash, HIGH);
}

void ledOFF(){
  digitalWrite(ledFlash, LOW);
}




void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // if you use this trick
  pinMode(ledFlash, OUTPUT);
  ledON();
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed");
    return;
  }
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  startCamera();
  setupServer();
}

void loop() {
  // nothing, async server handles requests
}