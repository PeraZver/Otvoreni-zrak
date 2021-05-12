/* Here's where I all JSON related stuff live. 
 * 
 * Pero, October 2019 */

#ifndef ZGZRAK_JSON_H
#define ZGZRAK_JSON_H

#include "Arduino.h"
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>   
#include <PMserial.h>
#include <NTPClient.h>
#include <secret.h>

//extern const char* provisionDeviceKey;
//extern const char* provisionDeviceSecret;
extern String token; 
extern NTPClient timeClient;

/*** JSON related functions */
String jsonTokenRequest(String chip_id);
bool jsonTokenProvision(const char* inputString);

String jsonSensorData();

/* helper functions */
#endif