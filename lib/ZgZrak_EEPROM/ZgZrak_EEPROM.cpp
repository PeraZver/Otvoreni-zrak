#include <ZgZrak_EEPROM.h>

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.begin(30);
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);

  EEPROM.commit();
}

String readStringFromEEPROM(int addrOffset)
{
  EEPROM.begin(30);
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];

  for (int i = 0; i < newStrLen; i++)
    data[i] = EEPROM.read(addrOffset + 1 + i); 

  data[newStrLen] = '\0'; 
  // EEPROM.end();
  return String(data);
}


void EEPROMcleanCredentials(){
    EEPROM.begin(30);
    Serial.println(F("\n\tClearing EEPROM"));
    for (uint8_t i = 0; i < 30; ++i)  
        EEPROM.write(i, 0); 
    EEPROM.commit();
}


