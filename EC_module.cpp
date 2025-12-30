#include "EC_module.h" 
#include <HardwareSerial.h>

int readings[SIZE];
int readIndex = 0;
long total = 0;
float averageADC = 0;
unsigned long measurementCount = 0;
int measurementState = 1;

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
    Serial.println(voltage, 2);
}

void set_measurement_state(int new_state) {
    if (new_state == 0) {
        measurementState = 0;
        Serial.println(">>> [명령] 측정 중지됨 (STOPPED). 시리얼 모니터에 1 입력 시 재시작.");
    } else if (new_state == 1) {
        measurementState = 1;
        Serial.println(">>> [명령] 측정 시작됨 (STARTED). 시리얼 모니터에 0 입력 시 중지.");
    } else {
        Serial.println(">>> [명령 오류] 유효하지 않은 입력입니다. 1(시작) 또는 0(중지)을 사용하세요.");
    }
}