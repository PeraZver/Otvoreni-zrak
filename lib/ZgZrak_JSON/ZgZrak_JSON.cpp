#include <ZgZrak_JSON.h>

extern float pm01_avg, pm25_avg, pm10_avg, pm0p3_avg, pm0p5_avg, pm1p0_avg, pm2p5_avg, pm5p0_avg, pm10p0_avg;

String jsonTokenRequest(String chip_id){
    String jsonString;
    StaticJsonDocument<128> doc;

    doc["deviceName"]            = chip_id.c_str(); //"ZG_ZRAK_Pero";
    doc["provisionDeviceKey"]    = provisionDeviceKey;
    doc["provisionDeviceSecret"] = provisionDeviceSecret;

    serializeJson(doc, jsonString);
    return jsonString;
}

bool jsonTokenProvision(const char* inputString){

    StaticJsonDocument<192> doc;
    DeserializationError error = deserializeJson(doc, inputString, strlen(inputString));

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return false;
    }
    const char* credentialsValue = doc["credentialsValue"];  // secret token 
    const char* credentialsType  = doc["credentialsType"];   // "ACCESS_TOKEN"
    const char* status           = doc["status"];            // "SUCCESS"

    if ( !strcmp(status, "SUCCESS") && !strcmp(credentialsType, "ACCESS_TOKEN")){
        Serial.print("\n\tSecret token:\t");
        Serial.println(credentialsValue);
        token = String(credentialsValue);
        return true;
    }

    else if ( !strcmp(status, "FAILURE")){
        Serial.print(F("\n\tMost likely device already registered."));
        return false;
    }

    else {
        Serial.println(F("\n\tBad token received. Aborting..."));
        return false;
    }
}

String jsonSensorData(){
          /* Serialize JSON */
        String sensorData_JSON; 
        StaticJsonDocument<256> doc;

        doc["pm10"] = pm10_avg;
        doc["pm25"] = pm25_avg;
        doc["measure_time"] = (unsigned long long)(timeClient.getEpochTime())*1000; //0;

        JsonObject data = doc.createNestedObject("data");
        data["ESP_id"] = ESP.getChipId();

        JsonObject data_Mass = data.createNestedObject("Mass");
        data_Mass["pm01"] = pm01_avg;
        data_Mass["pm25"] = pm25_avg;
        data_Mass["pm10"] = pm10_avg;

        JsonObject data_Num = data.createNestedObject("Num");
        data_Num["n0p3"]  = pm0p3_avg;
        data_Num["n0p5"]  = pm0p5_avg;
        data_Num["n1p0"]  = pm1p0_avg;
        data_Num["n2p5"]  = pm2p5_avg;
        data_Num["n5p0"]  = pm5p0_avg;
        data_Num["n10p0"] = pm10p0_avg;

        serializeJson(doc, sensorData_JSON);
        return(sensorData_JSON);
}