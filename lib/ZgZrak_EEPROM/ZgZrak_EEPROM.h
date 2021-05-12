/* Here's where I put all the  EEPROM r/w stuff. 
 * 
 * Pero, November 2020 */

#ifndef ZGZRAK_EEPROM_H
#define ZGZRAK_EEPROM_H

#include "Arduino.h"
#include <EEPROM.h>

/* EEPROM stuff prototypes*/
void EEPROMcleanCredentials();
void writeStringToEEPROM(int addrOffset, const String &strToWrite);
String readStringFromEEPROM(int addrOffset);

#endif