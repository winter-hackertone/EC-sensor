#include "EC_module.h"

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  for (int i = 0; i < SIZE; i++) {
    readings[i] = 0;
  }
  Serial.println("System Start. Waiting for 5 non-zero data points...");
}

void loop() {
    int current_adc = read_sensor_adc();
    
    update_moving_average(current_adc);
    
    check_and_alert(current_adc);
    
    display_data(current_adc);
    
    delay(1000);
}lay(1000);
}