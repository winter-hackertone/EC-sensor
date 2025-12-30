#ifndef TDS_MONITOR_H
#define TDS_MONITOR_H

#include <Arduino.h>

#define SIZE 5
#define MIN_AVG_THRESHOLD 50
#define WARNING_MULTIPLIER 1.5
#define TDS_PIN 32
#define BUZZER_PIN 4

extern int readings[SIZE];
extern long total;
extern float averageADC;
extern unsigned long measurementCount;
extern int readIndex;

int read_sensor_adc();
void update_moving_average(int current_adc);
void check_and_alert(int current_adc);
void display_data(int current_adc);

#endif