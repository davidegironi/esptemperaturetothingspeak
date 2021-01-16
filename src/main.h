/*
ESP Temperature to ThingSpeak v01

copyright (c) Davide Gironi, 2020

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#ifndef main_h
#define main_h

#include <Wire.h>

//ADS1X15 library
#include <Adafruit_ADS1015.h>

//set the serial output debug
#define SERIAL_ENABLED 1
//set the serial output debug
#define SERIALDEBUGSENSOR_ENABLED 1

//connection status led
#define WIFI_STATUSLEDPIN 16
//connection status led enabled
#define WIFI_STATUSLEDENABLED 1
//serial enabled
#define WIFI_SERIALENABLED SERIAL_ENABLED
//do not block on connection and captive portal
#define WIFI_SKIPIFNOTCONNECTED 0
//do not block on connection and captive portal timeout (seconds)
#define WIFI_SKIPIFNOTCONNECTEDTIME 180
//check connection at selected interval (milliseconds)
#define WIFI_CHECKCONNECTIONINTERVAL 10000
//check connection status enabled
#define WIFI_CHECKCONNECTIONENABLED 1
//captive portal by button pin
#define WIFI_CAPTIVEPORTALBUTTONPIN 4
//captive portal by button is enabled
#define WIFI_CAPTIVEPORTALBUTTONENABLED 0
//hozone
#define WIFI_HOSTNAME "esptemperaturetothingspeak"
//hostname length
#define WIFI_HOSTNAMELENGTH 15
//captive portal name
#define WIFI_CAPTIVEPORTALNAME "ESPTemperatureToThingSpeak-AP"

//get data interval ms
#define GETDATA_INTERVALMS 1000

//ADS1X15 address
#define ADS1115_ADDRESS 0x48
//ADS1X15 gain
#define ADS1115_GAIN GAIN_TWOTHIRDS
//ADS1X15 mV per bit
//ex. 16 bit on +/- 6.144V = 15bit on 6.144V
//    6.144V*2 / 2^16 = 0.0001875V
#define ADS1115_MVSTEP 0.1875

//NTC pullup voltage
#define NTC_PULLUPVOLT 3.3f
//NTC pullup resistors
#define NTC_PULLUPRES 22000

//define channels enabled
#define ADC_ENABLEDCHANNELS 4

//NTC specific parameters
#define NTC_BETA 3470
#define NTC_TREF 25
#define NTC_RREF 10000

//invalid temp constant
#define TEMP_INVALID -999

//adc ema filter alpha
#define ADC_EMAFILTERALPHA 50

//send data to thingspeak interval ms
#define THINGSPEAK_SENDDATAINTERVALMS 30000
//connection status led
#define THINGSPEAK_STATUSLEDPIN 14

//functions
extern String getLastdata();

#endif
