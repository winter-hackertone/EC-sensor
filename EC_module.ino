#include "EC_module.h" 

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "bssm_free";
const char* password = "bssm_free";

WebServer server(80);
const size_t capacity = JSON_OBJECT_SIZE(2);

void handleCommand() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String jsonCommand = server.arg("plain"); 

  StaticJsonDocument<capacity> doc;
  DeserializationError error = deserializeJson(doc, jsonCommand);

  if (error) {
    Serial.print(F("JSON 파싱 실패: "));
    Serial.println(error.f_str());
    server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"Invalid JSON\"}");
    return;
  }

  const char* stateValue = doc["state"];
  
  if (stateValue) {
    String stateStr = String(stateValue);
    
    if (stateStr.equalsIgnoreCase("activate")) {
      set_measurement_state(1);
    } else if (stateStr.equalsIgnoreCase("deactivate")) {
      set_measurement_state(0);
    } else {
      Serial.println(">>> [명령 오류] 알 수 없는 state 값: " + stateStr);
    }
    
    String response = "{\"status\":\"success\", \"current_state\":\"";
    response += (measurementState == 1 ? "activate" : "deactivate");
    response += "\"}";
    server.send(200, "application/json", response);
    
  } else {
    server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"'state' key missing\"}");
  }
}

void setupWebServer() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/command", HTTP_POST, handleCommand);
  
  server.begin();
  Serial.println("HTTP Server started.");
}


void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  for (int i = 0; i < SIZE; i++) {
    readings[i] = 0;
  }
  setupWebServer();
  Serial.println("System Ready."); 
}

void loop() {
    server.handleClient();

    if (measurementState == 1) {
        int current_adc = read_sensor_adc();
        
        update_moving_average(current_adc);
        
        check_and_alert(current_adc);
        
        display_data(current_adc);
        
        delay(1000); 
    } else {
        delay(100);
    }
}