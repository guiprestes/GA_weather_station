#define BLYNK_TEMPLATE_ID "XXX"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "XXX"

#define LED_WIFI 5

#include <SPI.h>
#include <Adafruit_BMP280.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <Nokia_LCD.h>

Adafruit_BMP280 bmp;
Nokia_LCD display(18,23,4,15,2);

void taskTemperature(void *pvParameters);
void taskWebServer(void *pvParameters);

const char* ssid = "wifi_name";
const char* password = "password";
const char* auth_token = "blynk-token";

void blinkLed() {
  while(1) {
    digitalWrite(LED_WIFI, HIGH);
    delay(100);
    digitalWrite(LED_WIFI, LOW);
    delay(100);
  }
}

void setup() {
  Serial.begin(9600);
  bmp.begin(0x76);
  SPI.begin();
  display.begin();
  display.setContrast(60);
  display.clear();
  WiFi.begin(ssid, password);
  Blynk.begin(auth_token ,ssid, password);

  pinMode(LED_WIFI, OUTPUT);

  while(WiFi.status() != WL_CONNECTED) {
    delay(2000);
    blinkLed();
    Serial.println("Connecting...");
  }
  Serial.println("Connected");
  digitalWrite(LED_WIFI, HIGH);

  xTaskCreate(
          taskTemperature
          ,  "taskTemperature"
          ,  4096
          ,  NULL
          ,  1
          ,  NULL );
  xTaskCreate(
          taskWebServer
          ,  "taskWebServer"
          ,  2048
          ,  NULL
          ,  2
          ,  NULL );
  // Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop() {
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void taskTemperature( void *pvParameters __attribute__((unused))) {
  for (;;) {

    double getTemperature = bmp.readTemperature();
    int getAltitude = bmp.readAltitude(1013.25);

    Serial.print("Temperatura: ");
    Serial.println(getTemperature);
    display.setCursor(0,1);
    display.println(getTemperature);
    display.setCursor(27,1);
    display.print(" C");
    Blynk.virtualWrite(V5,getTemperature);

    Serial.print("Altitude: ");
    Serial.println(getAltitude);
    display.setCursor(50,1);
    display.println(getAltitude);
    display.setCursor(60,1);
    display.println(" m");
    Blynk.virtualWrite(V4,getAltitude);

    delay(5000);
    vTaskDelay(5);
  }
}

void taskWebServer( void *pvParameters __attribute__((unused))) {
  for (;;) {
    Blynk.run();
    display.setCursor(0,4);
    display.println("Blynk Connect...");
    // display.setCursor(65,4);
    //timer.run();
    delay(300);
    vTaskDelay(5);
  }
}