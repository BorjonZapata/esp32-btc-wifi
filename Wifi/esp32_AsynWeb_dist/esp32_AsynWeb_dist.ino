#include <WiFi.h>

#include "ESPAsyncWebServer.h"


// Your WiFi credentials
//#define WIFI_SSID "shakiwifi"
//#define WIFI_PASSWORD ""


#define WIFI_SSID "Totalplay-D9A3_2.4G"
#define WIFI_PASSWORD "D9A34C52jmf7Fr49"


#define TRIG_PIN 22
#define ECHO_PIN 13

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Variables to hold sensor readings
float distance = 0;

// Stores the elapsed time from device start up
unsigned long elapsedMillis = 0;
// The frequency of sensor updates to firebase, set to 10seconds
unsigned long update_interval = 1000;

void Wifi_Init() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

String processor(const String &var) {
  //Serial.println(var);
  if (var == "DISTANCE") {
    return String(distance);
  }
  return String();
}


void updateSensorReadings() {
  Serial.println("------------------------------------");
  Serial.println("Reading Sensor data ...");
  digitalWrite(ECHO_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long time = pulseIn(ECHO_PIN, HIGH);
  distance = time * 0.000001 * 34300.0 / 2.0;
  Serial.printf("Disntance reading: %.2f \n", distance);
  delay(500);
}

void uploadSensorData() {
  if (millis() - elapsedMillis > update_interval) {
    elapsedMillis = millis();
    updateSensorReadings();
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 Distance Server</h2>
  <p>
    <i class="fa fa-arrows-h" style="color:#059e8a;"></i> 
    <span class="dht-labels">Distancia</span> 
    <span id="distance">%DISTANCE%</span>
    <sup class="units"> cm</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("distance").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/distance", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";


void server_Init() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/distance", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(distance).c_str());
  });
  // Start server
  server.begin();
}

void setup() {
  Serial.begin(115200);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  Wifi_Init();
  server_Init();
}

void loop() {
  uploadSensorData();
}
