#include "EC_module.h"

int readings[SIZE];
int readIndex = 0;
long total = 0;
float averageADC = 0;
unsigned long measurementCount = 0;

int read_sensor_adc() {
    return analogRead(TDS_PIN);
}

void update_moving_average(int current_adc) {
    if (current_adc == 0) {
        return; 
    }

    total = total - readings[readIndex];
    
    readings[readIndex] = current_adc;
    
    total = total + current_adc;
    
    readIndex = (readIndex + 1) % SIZE; 

    if (measurementCount < SIZE) {
        measurementCount++; 
    }
}

void check_and_alert(int current_adc) {
    int valid_divisor = (measurementCount < SIZE) ? measurementCount : SIZE;
    
    if (valid_divisor > 0) {
        averageADC = (float)total / valid_divisor;
    } else {
        averageADC = 0;
    }
    
    if (measurementCount >= SIZE) {
        if (current_adc > (averageADC * WARNING_MULTIPLIER) && averageADC > MIN_AVG_THRESHOLD) { 
            digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
            digitalWrite(BUZZER_PIN, LOW);

            Serial.println("\n!!! 경고: 현재 값이 평균의 1.5배를 초과함 !!!\n");
        }
    } else {
        Serial.print("(안정화 중: 0이 아닌 ");
        Serial.print(measurementCount);
        Serial.print("/");
        Serial.print(SIZE);
        Serial.println(" 측정 완료)");
    }
}

void display_data(int current_adc) {
    float voltage = current_adc * (3.3 / 4095.0);
    
    Serial.print("Current ADC: ");
    Serial.print(current_adc);
    Serial.print(" | Avg ADC (W=");
    Serial.print(SIZE);
    Serial.print("): ");
    Serial.print(averageADC);
    Serial.print(" | Voltage: ");
    Serial.println(voltage);
}