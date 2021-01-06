/*
ESP Temperature to ThingSpeak v01

copyright (c) Davide Gironi, 2020

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include <Arduino.h>

#include <Wire.h>

//include main
#include "main.h"

//WiFiManager helper library
#include "WiFiManagerHelper/WiFiManagerHelper.h"
//initialize WiFiManagerHelper
WiFiManagerHelper wifiManagerHelper;

//include pages
#include "page.h"

//include eepromConfig
#include "eepromconfig.h"
extern struct EEPROMConfigStruct eepromConfig;

//HTTP client library
#include <ESP8266HTTPClient.h>

//ADS1X15 library
#include <Adafruit_ADS1015.h>
Adafruit_ADS1115 ads(ADS1115_ADDRESS);

//ThingSpeak library
#include "ThingSpeak.h"

//include NTCTemp library
#include "ntctemp/ntctemp.h"
//contruct NTCTemp
NTCTemp ntc;

//check getdata interval time
static unsigned long getdataLast = 0;

//check senddata interval time
static unsigned long senddataLast = 0;

//last written data
static float lastdatatemp[ADC_ENABLEDCHANNELS];

// Initialize the client library
WiFiClient client;


/**
 * Exponential Moving Avarage filter
 */
unsigned int adcEmafilter(unsigned int newvalue, unsigned int value) {
	//use exponential moving avarate Y=(1-alpha)*Y + alpha*Ynew, alpha between 1 and 0
	//in uM we use int math, so Y=(63-63alpha)*Y + 63alpha*Ynew  and  Y=Y/63 (Y=Y>>6)
	value = (64-ADC_EMAFILTERALPHA)*value+ADC_EMAFILTERALPHA*newvalue;
	value = (value>>6);
	return value;
}

/**
 * Get data and set display numbers
 */
void getData() {
  static uint16_t firstrun = 1;
  static int16_t adcraw = 0;

#if SERIAL_ENABLED == 1
  Serial.println("Get data");
#endif

  for(uint8_t channel = 0; channel<ADC_ENABLEDCHANNELS; channel++) {
    //get adc for channel
    int16_t adcrawt = ads.readADC_SingleEnded(channel);
    //filter adc for channel
    if(!firstrun)
      adcraw = adcEmafilter(adcrawt, adcraw);
    else
      adcraw = adcrawt;
    //get voltage
    double volt = adcraw * ADS1115_MVSTEP / 1000;
    uint32_t res = 0;
    double temp = TEMP_INVALID;
    if(volt > 0 && volt < NTC_PULLUPVOLT - 0.10) {
      //get resistance
      res = NTC_PULLUPRES * (1 / ((NTC_PULLUPVOLT / volt) - 1));
      //get temperature
      temp = ntc.getB(res, NTC_BETA, NTC_TREF, NTC_RREF);
      //round to 2 decimal digits
      temp = round(temp*100)/100.0;
      //convert to fahrenehit
      if(eepromConfig.unitoftemperature[0] == 'F')
        temp = temp * 1.8 + 32;
    }
#if SERIAL_ENABLED == 1
#if SERIALDEBUGSENSOR_ENABLED == 1
    Serial.print("Channel "); Serial.print(channel + 1);
    Serial.print(" - ");
    if(temp != TEMP_INVALID) {
      Serial.print("Temp: "); Serial.print(temp);
    } else {
      Serial.print("Temp: invalid");
    }
    Serial.print(" - ");
    Serial.print("ADC: "); Serial.print(adcraw);
    Serial.print(" - ");
    Serial.print("Volt: "); Serial.print(volt);
    Serial.print(" - ");
    Serial.print("Res: "); Serial.print(res);
    Serial.println("");
#endif
#endif
    //update last data
    lastdatatemp[channel] = temp;
  }

  if(!firstrun)
    firstrun = 0;
}

/**
 * Get last data
 */
String getLastdata() {
  String ret = "";

  //get data
  for(uint8_t channel = 0; channel<ADC_ENABLEDCHANNELS; channel++) {
    char buft[50];
    if(lastdatatemp[channel] != TEMP_INVALID) {
      sprintf(buft, "Channel %d temperature: <b>%0.2f %s</b><br/>", channel + 1, lastdatatemp[channel], eepromConfig.unitoftemperature);
      ret = ret + buft;
    }
  }

  return ret;
}

/**
 * Send data to ThingSpeak
 */
void sendDataToThingSpeak() {
#if SERIAL_ENABLED == 1
  Serial.print("Sending data to ThingSpeak...");
#endif

  uint8_t allsent = 1;
  for(uint8_t channel = 0; channel<ADC_ENABLEDCHANNELS; channel++) {
    if(lastdatatemp[channel] != TEMP_INVALID) {
      int httpCode = ThingSpeak.writeField(atol(eepromConfig.thingspeakchannelid), (channel + 1), (float)(lastdatatemp[channel]), eepromConfig.thingspeakapikey);
      if (httpCode == 200) {
#if SERIAL_ENABLED == 1
        Serial.print("Channel "); Serial.print(channel + 1); Serial.println(" succesfully sent");
#endif
      } else {
        allsent = 0;
#if SERIAL_ENABLED == 1
        Serial.print("Channel "); Serial.print(channel + 1); Serial.println(" sent with error " + String(httpCode));
#endif
      }
    }
  }
  if(allsent)
    digitalWrite(THINGSPEAK_STATUSLEDPIN, HIGH);
  else
    digitalWrite(THINGSPEAK_STATUSLEDPIN, LOW);
}

/**
 * Main setup
 */
void setup() {
#if SERIAL_ENABLED == 1
  //initialize Serial
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting...");
#endif

  //initialize eeprom
#if SERIAL_ENABLED == 1
  Serial.println("Loading eeprom");
#endif
  eepromInit();
  eepromRead();
  
  //initizlied adc
#if SERIAL_ENABLED == 1
  Serial.println("Initialize ADS1X15 ADC");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV)");
#endif
  ads.begin();
  ads.setGain(ADS1115_GAIN);

  //set WiFi
#if SERIAL_ENABLED == 1
  Serial.println("Set the WiFi");
#endif
  char *apname = (char *)WIFI_CAPTIVEPORTALNAME;
  char deviceidc[4];
  snprintf(deviceidc, sizeof(deviceidc), "%03d", eepromConfig.deviceid);
  char hostname [WIFI_HOSTNAMELENGTH + 4];
  strcpy(hostname, WIFI_HOSTNAME);
  strcat(hostname, deviceidc);
  //set the connection manager
  wifiManagerHelper.setSerialDebug(WIFI_SERIALENABLED);
  wifiManagerHelper.setCheckConnection(WIFI_CHECKCONNECTIONINTERVAL, WIFI_CHECKCONNECTIONENABLED);
  wifiManagerHelper.setStatusLed(WIFI_STATUSLEDPIN, WIFI_STATUSLEDENABLED);
  wifiManagerHelper.setCaptivePortalOnButton(WIFI_CAPTIVEPORTALBUTTONPIN, WIFI_CAPTIVEPORTALBUTTONENABLED);
  //try to connect
  wifiManagerHelper.connect(hostname, apname, WIFI_SKIPIFNOTCONNECTEDTIME, WIFI_SKIPIFNOTCONNECTED);
#if SERIAL_ENABLED == 1
  if(wifiManagerHelper.isConnected()) {
    Serial.println("Status: connected");
    Serial.print("    IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Status: not connected");
  }
#endif

  //initialize webserver
#if SERIAL_ENABLED == 1
  Serial.println("Starting webserver");
#endif
  pageInit();

  //initialize thingspeak
  ThingSpeak.begin(client);
  pinMode(THINGSPEAK_STATUSLEDPIN, OUTPUT);
  digitalWrite(THINGSPEAK_STATUSLEDPIN, LOW);

  //initialize last time
  getdataLast = millis();
  senddataLast = millis();
}

/**
 * Main loop
 */
void loop() {
  //check connection
  wifiManagerHelper.checkConnection();

  //page server handler
  pageHandleClient();

  //get data
  if((millis() - getdataLast) > GETDATA_INTERVALMS) {
    getdataLast = millis();
    getData();
  }
  
  //send to thingspeak
  if(wifiManagerHelper.isConnected()) {
    if((millis() - senddataLast) > THINGSPEAK_SENDDATAINTERVALMS) {
      senddataLast = millis();
      sendDataToThingSpeak();
    }
  }
}
