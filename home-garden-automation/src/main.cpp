#include <Arduino.h>
#include "main.h"
#include "secrets.h"

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <BlynkSimpleEsp8266_SSL.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#define BLYNK_PRINT Serial
#define ALLOW_MANUAL_CONTROL false

#define PIN_LED1            0
#define PIN_LED2            2
#define PIN_LED3            14

#define PIN_SW1             15
#define PIN_IN1             5
#define PIN_CONTROL_IN      12
#define PIN_CONTROL_OUT     13

#define VPIN_MANUAL_SWITCH  V0
#define VPIN_STATUS_REPORT  V1
#define VPIN_RSSI_REPORT    V2
#define VPIN_TIME           V10


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.google.com", 32400, 3600000);

BlynkTimer timer;
WidgetRTC rtc;


bool status_external_LED_control = 0;  // current plant LED status
// bool previous_timer_control_status = 0;  // used to switch back to auto mode
bool flag_overwrite_timer_LED_control = 0;  // 1 if user modifies the timer

uint32_t time_current = 0;  // in seconds since 00:00
uint32_t time_start = 0;  // in seconds since 00:00
uint32_t time_stop = 0;  // in seconds since 00:00

const int time_manual_start = 8;
const int time_manual_stop = 20;


void setup()
{
  delay(100);

  Serial.begin(115200);

  init_pins();

  Serial.println();
  Serial.println();
  Serial.println("ESP8266 initializing Blynk Service...");


  // Blynk.connect(auth, ssid, pass);

  Blynk.connectWiFi(ssid, pass);
  Serial.println("ESP8266 connected to WiFi");

  Blynk.config(auth);
  Serial.println("BLynk config done");

  // bool res_conn = false;
  // while(res_conn == false)
  // {
  //   res_conn = Blynk.connect();
  //   if(res_conn == false)
  //   {
  //     Serial.println("Blynk server connection failed! Retrying...");
  //   }
  // }
  // Serial.println("Blynk server connection established!");
  // Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  timeClient.begin();

  timer.setInterval(1000L, report_RSSI);
  timer.setInterval(500L, report_status);
  timer.setInterval(1000L, process_plant_LED);

  Serial.println("ESP8266 initialized");

  animate_LED();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void loop()
{
  timeClient.update();

  timer.run();

  LED_process();

  // Blynk.run();

  // delay(500);
}


void init_pins()
{
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);

  digitalWrite(PIN_LED1, HIGH);  // all LEDs off
  digitalWrite(PIN_LED2, HIGH);
  digitalWrite(PIN_LED3, HIGH);

  pinMode(PIN_SW1, INPUT);
  pinMode(PIN_IN1, INPUT);
  pinMode(PIN_CONTROL_IN, INPUT);
  pinMode(PIN_CONTROL_OUT, OUTPUT);

  digitalWrite(PIN_CONTROL_OUT, LOW);  // default state to low
  digitalWrite(PIN_LED2, LOW);  // Blynk connection check LED to on
}


void animate_LED()
{
  digitalWrite(PIN_LED1, HIGH);
  digitalWrite(PIN_LED2, HIGH);
  digitalWrite(PIN_LED3, HIGH);

  digitalWrite(PIN_LED1, LOW);
  delay(80);
  digitalWrite(PIN_LED2, LOW);
  delay(80);
  digitalWrite(PIN_LED3, LOW);
  delay(80);

  digitalWrite(PIN_LED1, HIGH);
  delay(80);
  digitalWrite(PIN_LED2, HIGH);
  delay(80);
  digitalWrite(PIN_LED3, HIGH);
  delay(80);

  digitalWrite(PIN_LED1, LOW);
  delay(80);
  digitalWrite(PIN_LED2, LOW);
  delay(80);
  digitalWrite(PIN_LED3, LOW);
  delay(80);

  digitalWrite(PIN_LED1, HIGH);
  delay(80);
  digitalWrite(PIN_LED2, HIGH);
  delay(80);
  digitalWrite(PIN_LED3, HIGH);
}

void LED_process()
{
  if(millis()/1000%2)  // heatbeat
  {
    digitalWrite(PIN_LED1, HIGH);
  }else
  {
    digitalWrite(PIN_LED1, LOW);
  }

  if(Blynk.CONNECTED == true)  // Blynk connection
  {
    digitalWrite(PIN_LED2, HIGH);
  }else
  {
    digitalWrite(PIN_LED2, LOW);
  }
  
}


void report_RSSI()
{
  Blynk.virtualWrite(VPIN_RSSI_REPORT, WiFi.RSSI());
}

void report_status()
{
  Blynk.virtualWrite(VPIN_STATUS_REPORT, status_external_LED_control);
}


void process_plant_LED()
{
  if(timeClient.getHours()>=time_manual_start && timeClient.getHours()<time_manual_stop)
  {
    control_plant_LED(1);
  }else
  {
    control_plant_LED(0);
  }
  
  // time_current = hour()*60*60 + minute()*60 + second();

  // if(flag_overwrite_timer_LED_control == 0)  // auto timer control; just check time
  // {
  //   if(time_current>=time_start && time_current<=time_stop)
  //   {
  //     control_plant_LED(1);
  //   }else
  //   {
  //     control_plant_LED(0);
  //   }
  // }else  // manual control set; check to switch back to auto mode
  // // TODO: this part has errors
  // // check "ALLOW_MANUAL_CONTROL" flag before debugging
  // {
  //   bool is_time_set = time_current>=time_start&&time_current<=time_stop;
  //   if(is_time_set == status_external_LED_control)
  //   {
  //     ;
  //   }else
  //   {
  //     flag_overwrite_timer_LED_control = 0;
  //     process_plant_LED();
  //   }
  // }
}

void control_plant_LED(bool parameter_control)
{
  if(parameter_control == 1)
  {
    status_external_LED_control = 1;
    digitalWrite(PIN_LED3, LOW);
    digitalWrite(PIN_CONTROL_OUT, HIGH);
  }else
  {
    status_external_LED_control = 0;
    digitalWrite(PIN_LED3, HIGH);
    digitalWrite(PIN_CONTROL_OUT, LOW);
  }
}


BLYNK_CONNECTED()
{
  rtc.begin();
}

BLYNK_WRITE(VPIN_MANUAL_SWITCH)
{
  flag_overwrite_timer_LED_control = ALLOW_MANUAL_CONTROL;

  if(param.asInt() == true)
  {
    control_plant_LED(1);
  }else
  {
    control_plant_LED(0);
  }

  // process_plant_LED();
}

BLYNK_WRITE(VPIN_TIME)
{
  TimeInputParam t(param);

  flag_overwrite_timer_LED_control = 0;

  if(t.hasStartTime())
  {
    time_start = t.getStartHour()*60*60 + t.getStartMinute()*60 + t.getStartSecond();
  }

  if(t.hasStopTime())
  {
    time_stop = t.getStopHour()*60*60 + t.getStopMinute()*60 + t.getStopSecond();
  }

  time_current = hour()*60*60 + minute()*60 + second();

  Serial.print("time_curr:  ");  Serial.println(time_current);
  Serial.print("time_start: ");  Serial.println(time_start);
  Serial.print("time_stop:  ");  Serial.println(time_stop);

  process_plant_LED();
}

