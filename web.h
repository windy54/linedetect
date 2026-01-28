#include <ESPAsyncWebServer.h>

extern void ledOFF();
extern void ledON();

bool rawMode = true;   // when true raw is displayed on button and processed video displayed
int lineCentre= 0;

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head><meta charset="utf-8"><title>ESP32-CAM</title></head>
<body>
  <h2>Processed grayscale</h2>
  <img id="cam" src="/capture">
  <button id="rawBtn">raw</button>
  <script>
    setInterval(function() {
      var img = document.getElementById('cam');
      img.src = '/capture?ts=' + Date.now();
    }, 1000); // 1 fps

  document.getElementById('rawBtn').addEventListener('click', () => {
    // tell the ESP that "raw" was pressed
    fetch('/rawPressed')
      .then(r => r.text())
      .then(newLabel => {
        // ESP replies with new text, update button
        document.getElementById('rawBtn').textContent = newLabel;
      })
      .catch(err => console.error(err));
  });  
  </script>
</body>
</html>
)rawliteral";

void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });


  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request){
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    request->send(500, "text/plain", "Capture failed");
    return;
  }


  // Run edge detection in-place
  if (rawMode){
    lineCentre = detectLineEdges(fb);
    Serial.print("centre=");
    Serial.println(lineCentre);
  }

  uint8_t *jpg_buf = nullptr;
  size_t   jpg_len = 0;
  bool ok = frame2jpg(fb, 80, &jpg_buf, &jpg_len);
  esp_camera_fb_return(fb);

  if (!ok || !jpg_buf) {
    request->send(500, "text/plain", "JPEG conversion failed");
    return;
  }

  auto *response =
      request->beginResponse_P(200, "image/jpeg", jpg_buf, jpg_len);
  response->addHeader("Cache-Control", "no-store");
  request->send(response);
  free(jpg_buf);
});

  server.on("/rawPressed", HTTP_GET, [](AsyncWebServerRequest *request){
    String newLabel="Processed";
    // flip state, or whatever logic you want
    rawMode = !rawMode;
    //String newLabel = rawMode ? "raw" : "processed";  // or any text you like
    if(rawMode){
      ledON();
      newLabel = "Raw";
    }
    else{
      ledOFF();
      newLabel = "Processed";
    }
    request->send(200, "text/plain", newLabel);
  });
  
  server.begin();
}