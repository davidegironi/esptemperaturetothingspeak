/*
ESP LSI Wind meter v01

copyright (c) Davide Gironi, 2020

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#ifndef eepromconfig_h
#define eepromconfig_h

#include <Arduino.h>

//EEPROM library
#include <EEPROM.h>
//include for limits
#include <spi_flash.h>

//EEPROMConfigStruct
struct EEPROMConfigStruct
{
  uint8_t initialized;
  int deviceid;
  char thingspeakapikey[20];
  char thingspeakchannelid[10];
  char unitoftemperature[2];
};

//export functions
extern void eepromRead();
extern void eepromWrite();
extern void eepromInit();

#endif
