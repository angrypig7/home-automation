#include <Arduino.h>
#include "main.h"
#include "secrets.h"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266_SSL.h>

#define BLYNK_PRINT Serial

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
#define VPIN_TIME_START     V3
#define VPIN_TIME_END       V5


WidgetLED led1(V1);

BlynkTimer timer;

uint8_t control_status = 0;  // current plant LED status

void setup()
{
  delay(100);

  Serial.begin(115200);

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

  Serial.println();
  Serial.println();
  Serial.println("ESP8266 initializing Blynk Service...");

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  timer.setInterval(1000L, signalStrengthReport);
  timer.setInterval(500L, statusReport);

  Serial.println("ESP8266 initialized");

  animateLED();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void loop()
{
  Blynk.run();

  timer.run();

  LED_process();

  // delay(120);
}


void animateLED()
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


void signalStrengthReport()
{
  Blynk.virtualWrite(VPIN_RSSI_REPORT, WiFi.RSSI());
}

void statusReport()
{
  Blynk.virtualWrite(VPIN_STATUS_REPORT, control_status);
}

BLYNK_WRITE(V1)
{
  // Serial.print("Blynk-V1: ");
  // Serial.println(param.asStr());

  if(param.asInt() == true)
  {
    digitalWrite(PIN_LED3, LOW);
    digitalWrite(PIN_CONTROL_OUT, HIGH);
  }else
  {
    digitalWrite(PIN_LED3, HIGH);
    digitalWrite(PIN_CONTROL_OUT, LOW);
  }
}

