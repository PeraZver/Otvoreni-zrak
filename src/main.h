#include <Arduino.h>
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ESP8266HTTPClient.h> 
#include <SoftwareSerial.h>
#include <PMserial.h>             // Arduino library for PMS7003 sensors with serial interface
#include <ArduinoOTA.h>
#include <ZgZrak_JSON.h>
#include <ZgZrak_EEPROM.h>
#include <NTPClient.h>            // NTP Client for time syncing
#include <WiFiUdp.h>              // UDP layer for the NTP Client
#include <secret.h>

/* PMS7003 defines */
#define PMS_RX D5
#define PMS_TX D6
#define SENSOR_UPDATE_TIME 1*60*1000
#define WIFI_CHECK_TIME    5*60*1000
#define LED LED_BUILTIN


/* Global variables */
String token;
String ZG_Zrak_post_url; 
bool posted = true;
int num_measurements = 0;
float pm01_avg, pm25_avg, pm10_avg, pm0p3_avg, pm0p5_avg, pm1p0_avg, pm2p5_avg, pm5p0_avg, pm10p0_avg = 0.0f;
signed int last_hour = 0;
unsigned long last_millis, last_measurement = 0;

/* Function prototypes */
String postJSON(const char * url, String json);
bool getToken();
bool read_sensor_data();
#ifdef DEBUG
ICACHE_RAM_ATTR void resetToFactoryDefaults();
#endif

#ifndef RELEASE
void startOTA();
#endif