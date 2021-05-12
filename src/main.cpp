/************************************************************************************
 * ZG-Zrak
 * Mjerenje kvalitete zraka
 * 
 * Pero, Prosinac 2020 
 ************************************************************************************/
#include "main.h"

/* Class constructors */
WiFiManager wifiManager;
SerialPM pms(PMS7003, PMS_RX, PMS_TX); 
WiFiClient client;
HTTPClient http;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#ifdef DEBUG
#define BUTTON 0             // "Factory" reset button
#endif

void setup() {

  #ifdef DEBUG
  /* Pin setup (put pin 0 in interrupt mode) */
  pinMode(BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON), resetToFactoryDefaults, FALLING);
  #endif
  /* LED is ON when not connected*/
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  /* Inital output */
  Serial.begin(115200);    
  delay(10);
  Serial.print(F("\n\rChip ID:\t"));
  Serial.println(ESP.getChipId());
  Serial.print(F("Core v.:\t"));
  Serial.println(ESP.getCoreVersion());
  Serial.print(F("Last reset reason:\t"));
  Serial.println(ESP.getResetReason());
  Serial.println(F("\n*********************************"));
  Serial.println(F("\n    ZG ZRAK - Kvaliteta Zraka    "));
  Serial.println(F("\n*********************************"));

  /* Configuring WiFi */
  Serial.println(F("[1a] Connecting to the WiFi ..."));
  wifiManager.autoConnect("Otvoreni Zrak");
  Serial.print(F("[1b] Connected to:\t"));
  Serial.println(WiFi.SSID());
  Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());

  /* Read the token from EEPROM */
  token = readStringFromEEPROM(0);
  Serial.print(F("\n\tToken stored in EEPROM:\t"));
  Serial.println(token);
  
  if (token.length() >= 20){
    /* Use the token with ZG Zrak server*/
    /* Here I'm requesting the token, regardless if it has already been stored in eeprom or not. 
    During the test, I had to clean tokens very often, which messed with my wifi config, so I decided to keep it asking every time 
    the program starts. It should be abandoned in the release version, with each new devices.  
    EDIT: it should remain, since upon the initial run the garbage is read from EEPROM and considered as a token.*/
    getToken();    
  }
  else {
    /* If the token is bad, get the new token and reset */
    Serial.print(F("\n\tToken not available."));
    if (getToken()){
      Serial.println(F("\tToken succesfully retreived."));
    }
    else{
      Serial.println(F("\tToken fail. Rebooting ..."));
      WiFi.disconnect();
      delay(3000);
      ESP.reset();
      delay(3000);
    }
  }
  /* Turn off the debug LED */
  digitalWrite(LED, HIGH);
  /* Sensor data post URL */
  ZG_Zrak_post_url = ZGZrak_WebServer + "/api/v1/" + token + "/telemetry";
  /* Configuring NTP Client */
  Serial.println(F("[2] Initializing NTP Client ..."));
  timeClient.begin();
  /* Get the correct time */
  timeClient.update();
  last_hour = timeClient.getHours();
  last_millis = last_measurement = millis();
  /* Configuring sensors*/ 
  Serial.println(F("[3] Initializing PM sensor ..."));
  pms.init();
  
  #ifndef RELEASE
  /* Prepare the sensor data JSON and send it */
  String sensorData_JSON = jsonSensorData();
  Serial.println(F("[6] Dummy post ..."));
  postJSON(ZG_Zrak_post_url.c_str(), sensorData_JSON);
  /* Over the Air update */
  Serial.println(F("[4] Enabling OTA ..."));
  startOTA();
  #endif
}

void loop() {
  /* Update NTP Client */
  timeClient.update();
  /* Read the data, store in the buffer */
  if ( (millis() - last_measurement) >= SENSOR_UPDATE_TIME) {
    
    Serial.printf("\n\r\tCurrent time: %d:%d\n\r", timeClient.getHours(), timeClient.getMinutes());
    //Serial.printf("\tLast stored hours: %d\n\r", last_hour);
    Serial.println(F("[5] Read sensor data ..."));
    if (read_sensor_data()){  
      pm01_avg   += (float)pms.pm01;
      pm25_avg   += (float)pms.pm25;
      pm10_avg   += (float)pms.pm10;
      pm0p3_avg  += (float)pms.n0p3;
      pm0p5_avg  += (float)pms.n0p5;
      pm1p0_avg  += (float)pms.n1p0;
      pm2p5_avg  += (float)pms.n2p5;
      pm5p0_avg  += (float)pms.n5p0;
      pm10p0_avg += (float)pms.n10p0;
      num_measurements++;
    }  
    else {
      Serial.println(F("\tFailed to read the sensor data."));
    }
    last_measurement = millis();
  }

  /* Every hour, calc average and post to server */
  if ( timeClient.getHours() != last_hour ){
    
    last_hour = timeClient.getHours();
    /* Calculate averages */  
    pm01_avg   /= num_measurements;
    pm25_avg   /= num_measurements;
    pm10_avg   /= num_measurements;
    pm0p3_avg  /= num_measurements;
    pm0p5_avg  /= num_measurements;
    pm1p0_avg  /= num_measurements;
    pm2p5_avg  /= num_measurements;
    pm5p0_avg  /= num_measurements;
    pm10p0_avg /= num_measurements;
    Serial.printf("\n\r\tNumber of averages: %d.\n\r", num_measurements);
    
    /* Prepare the sensor data JSON and send it */
    String sensorData_JSON = jsonSensorData();
    Serial.println(F("[6] Posting measurements ..."));
    postJSON(ZG_Zrak_post_url.c_str(), sensorData_JSON);
    
    /* Set all averages back to 0 */
    pm01_avg   = 0.0f;
    pm25_avg   = 0.0f;
    pm10_avg   = 0.0f;
    pm0p3_avg  = 0.0f;
    pm0p5_avg  = 0.0f;
    pm1p0_avg  = 0.0f;
    pm2p5_avg  = 0.0f;
    pm5p0_avg  = 0.0f;
    pm10p0_avg = 0.0f;
    num_measurements = 0;
  }
  
  /* Check WiFi connection every x minutes, reboot if bad. */
  if (millis() - last_millis >= WIFI_CHECK_TIME){
    digitalWrite(LED, LOW);
    if (WiFi.isConnected()){
      Serial.println(F("\n\rWiFi Connection OK.\n"));
      delay(500);
      digitalWrite(LED, HIGH);
    }
    else{
      Serial.println(F("\n\rWiFi Connection not OK. Rebooting ...\n"));
      WiFi.disconnect();
      delay(3000);
      ESP.reset();
      delay(3000);
    }
    last_millis = millis();
  }

  #ifndef RELEASE
  /* listen for OTA events */
  ArduinoOTA.handle();      
  #endif
}

/* This function reads and displays the sensor data */
bool read_sensor_data()
{
  pms.read();
  if(pms) {

      /* Print data to Serial port */ 
      Serial.println(F("\t-------- Mass -------   ----------------- Number ------------------  "));
      Serial.println(F("\tConcentration [μg/m3]             Concentration [#/100 cm3]          "));
      Serial.println(F("\tP1.0\tP2.5\tP10\tP0.3\tP0.5\tP1.0\tP2.5\tP5.0\tP10"));

      Serial.print(F("\t"));
      Serial.print(pms.pm01);
      Serial.print(F("\t"));
      Serial.print(pms.pm25);
      Serial.print(F("\t"));
      Serial.print(pms.pm10);

      if (pms.has_number_concentration()){ 
        Serial.print(F("\t"));
        Serial.print(pms.n0p3);
        Serial.print(F("\t"));
        Serial.print(pms.n0p5);
        Serial.print(F("\t"));
        Serial.print(pms.n1p0);
        Serial.print(F("\t"));
        Serial.print(pms.n2p5);
        Serial.print(F("\t"));
        Serial.print(pms.n5p0);
        Serial.print(F("\t"));
        Serial.print(pms.n10p0);
        Serial.print(F("\n\r"));
      }

      return(true);
  }
  else { // something went wrong
    switch (pms.status) {
        case pms.OK: // should never come here
          break;     // included to compile without warnings
        case pms.ERROR_TIMEOUT:
          Serial.println(F(PMS_ERROR_TIMEOUT));
          break;
        case pms.ERROR_MSG_UNKNOWN:
          Serial.println(F(PMS_ERROR_MSG_UNKNOWN));
          break;
        case pms.ERROR_MSG_HEADER:
          Serial.println(F(PMS_ERROR_MSG_HEADER));
          break;
        case pms.ERROR_MSG_BODY:
          Serial.println(F(PMS_ERROR_MSG_BODY));
          break;
        case pms.ERROR_MSG_START:
          Serial.println(F(PMS_ERROR_MSG_START));
          break;
        case pms.ERROR_MSG_LENGTH:
          Serial.println(F(PMS_ERROR_MSG_LENGTH));
          break;
        case pms.ERROR_MSG_CKSUM:
          Serial.println(F(PMS_ERROR_MSG_CKSUM));
          break;
        case pms.ERROR_PMS_TYPE:
          Serial.println(F(PMS_ERROR_PMS_TYPE));
          break;
    }
    return (false); 
  }

}

/******   HTTP Client functions ********/
/* This functions inquires the token from the server, by posting a certain JSON and receiving the JSON with the token. */
bool getToken(){
  /* Prepare request JSON */
  String tokenRequest_JSON = jsonTokenRequest(String(ESP.getChipId())); 
  /* Token request URL */
  String ZG_Zrak_token_url = ZGZrak_WebServer + ZGZrak_provision;
  String payload = postJSON(ZG_Zrak_token_url.c_str(), tokenRequest_JSON);

  /* Store the token into the EEPROM */
  if ( jsonTokenProvision(payload.c_str() ) ){
    Serial.print(F("\n\tStoring token:\t"));
    Serial.println(token);
    writeStringToEEPROM(0, token);
    String retrievedString = readStringFromEEPROM(0);
    Serial.print(F("\tToken stored:\t"));
    Serial.println(retrievedString);
    return(true);
  }
  else{
    Serial.println(F("\n\r\tERROR: Token unobtained."));
    return (false);
  }
}

/* This function posts JSON to the given url and returns the response content as String */
String postJSON(const char * url, String json){
  Serial.print(F("\n\r\tConnecting to the Otvoreni Zrak Server...\n\r\tPosting to:\t"));
  Serial.println(url);    
  Serial.print("\n\tJSON:\t");
  Serial.println(json);
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "close");
  http.addHeader("Content-Length", String(json.length())); 
  auto httpCode = http.POST(json);
  Serial.print("\tReturn code:\t"); 
  Serial.println(httpCode); //Print HTTP return code 
  String payload = http.getString(); 
  Serial.print("\tResponse:\t"); 
  Serial.println(payload); //Print request response payload 
  Serial.println(F("\tClosing connection."));
  http.end(); //Close connection Serial.println(); 
  return payload;
}

#ifndef RELEASE
/******   Over the Air flashing handle *****/
void startOTA() { 
  ArduinoOTA.setHostname(OTAName);
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]() {
      Serial.println(F("\tOTA Start"));
      });
  ArduinoOTA.onEnd([]() {
        Serial.println(F("\r\n OTA End"));
      });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
      });
  ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
        else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
        else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
        else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
        else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
      });
  ArduinoOTA.begin();
  Serial.println(F("OTA ready\r\n"));
}
#endif

/******** DEBUG Functions *******/
#ifdef DEBUG
/* Function to delete EEPROM Conten upon the button interrupt. */
ICACHE_RAM_ATTR void resetToFactoryDefaults() {
    Serial.print(F("FACTORY RESET BUTTON PRESSED."));
    WiFi.disconnect();
    EEPROMcleanCredentials();
    delay(3000);
    ESP.reset();
    delay(3000);
}
#endif