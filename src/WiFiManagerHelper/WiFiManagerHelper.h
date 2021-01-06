/*
WiFiManagerHelper, an helper for WiFiManager

copyright (c) Davide Gironi, 2017

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#ifndef WiFiManagerHelper_h
#define WiFiManagerHelper_h

//WiFi connection manager library
#include "WiFiManager.h"
//Multicast DNS library
#include <ESP8266mDNS.h>
//Ticker library
#include <Ticker.h>
//interface SDK library
extern "C" {
  #include "user_interface.h"
}

class WiFiManagerHelper {
  public:
    WiFiManagerHelper();
    void setSerialDebug(uint8_t enabled);
    void setStatusLed(uint8_t pin, uint8_t enabled);
    void setCheckConnection(unsigned long interval, uint8_t enabled);
    void setCaptivePortalOnButton(uint8_t pin, uint8_t enabled);
    void connect(char *hostname, char *apname, int configportaltimeout, uint8_t skipifnotconnected);
    void resetSettings();
    int checkConnection();
    int isConnected();
  private:
    uint8_t _checkconnection_enabled = 0;
    unsigned long _checkconnection_timer = 0;
    unsigned long _checkconnection_interval = 0;
    uint8_t _captiveportalonbutton_enabled = 0;
    uint8_t _captiveportalonbutton_pin = 0;
};

#endif
