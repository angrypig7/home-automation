#include<Arduino.h>

void init_pins();

void animate_LED();
void LED_process();

void report_RSSI();
void report_status();

void control_plant_LED(bool parameter_control);
void process_plant_LED();
