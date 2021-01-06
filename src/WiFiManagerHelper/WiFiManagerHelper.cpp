/*
WiFiManagerHelper, an helper for WiFiManager

copyright (c) Davide Gironi, 2017

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include "WiFiManagerHelper.h"

//define connection status
#define WIFIMANAGERHELPER_STATUSSTARTAP 0
#define WIFIMANAGERHELPER_STATUSCONNECTED 1
#define WIFIMANAGERHELPER_STATUSTRYCONNECTION 2
#define WIFIMANAGERHELPER_STATUSNOTCONNECTED 3

//is connected status
uint8_t wifimanagerhelper_isconnected = 0;

//status ticker
static Ticker wifimanagerhelper_statusLedTicker;
//status led pin
static uint8_t wifimanagerhelper_statusled_pin = 0;
//status led enabled
static uint8_t wifimanagerhelper_statusled_enabled = 0;

//serial debugger enabler
static uint8_t wifimanagerhelper_serialdebug_enabled = 0;

/**
 * Toggle the status led
 * @param pin PIN number
 */
void wifimanagerhelper_statusLedTick(uint8_t pin) {
  //toggle the led state
  digitalWrite(pin, !digitalRead(pin));
}

/**
 * Set the status
 * @param status status values
 */
void wifimanagerhelper_setStatus(uint8_t status) {
  static uint8_t isconnected = 0;
  if(status == WIFIMANAGERHELPER_STATUSSTARTAP) {
    isconnected = 0;
    wifimanagerhelper_isconnected = 0;
    if(wifimanagerhelper_serialdebug_enabled) {
      Serial.println("Configuration mode");
      Serial.println(WiFi.softAPIP());
    }
    if(wifimanagerhelper_statusled_enabled) {
      //set led to config mode
      wifimanagerhelper_statusLedTicker.detach();
      wifimanagerhelper_statusLedTicker.attach(0.2, wifimanagerhelper_statusLedTick, wifimanagerhelper_statusled_pin);
    }
  } else if(status == WIFIMANAGERHELPER_STATUSCONNECTED) {
    wifimanagerhelper_isconnected = 1;
    if(isconnected != 1) {
      isconnected = 1;
      if(wifimanagerhelper_serialdebug_enabled) {
        Serial.println("Connected");
        Serial.println(WiFi.localIP());
      }
      if(wifimanagerhelper_statusled_enabled) {
        //set led to connected mode
        wifimanagerhelper_statusLedTicker.detach();
        digitalWrite(wifimanagerhelper_statusled_pin, HIGH);
      }
    }
  } else if(status == WIFIMANAGERHELPER_STATUSTRYCONNECTION) {
    isconnected = 0;
    wifimanagerhelper_isconnected = 0;
    if(wifimanagerhelper_serialdebug_enabled) {
      Serial.println("Trying to connect");
    }
    if(wifimanagerhelper_statusled_enabled) {
      //set let to connection mode
      wifimanagerhelper_statusLedTicker.detach();
      wifimanagerhelper_statusLedTicker.attach(0.5, wifimanagerhelper_statusLedTick, wifimanagerhelper_statusled_pin);
    }
  } else if(status == WIFIMANAGERHELPER_STATUSNOTCONNECTED) {
    wifimanagerhelper_isconnected = 0;
    if(isconnected) {
      isconnected = 0;
      if(wifimanagerhelper_serialdebug_enabled) {
        Serial.println("Not connected");
      }
      if(wifimanagerhelper_statusled_enabled) {
        //set led to not connected mode
        wifimanagerhelper_statusLedTicker.detach();
        digitalWrite(wifimanagerhelper_statusled_pin, LOW);
      }
    }
  }
}

/**
 * WiFiManger config callback
 * @param myWiFiManager wifimanger instance
 */
void wifimanagerhelper_configModeCallback(WiFiManager *myWiFiManager) {
  //set wifi status
  wifimanagerhelper_setStatus(WIFIMANAGERHELPER_STATUSSTARTAP);
}

/**
 * Constructor
 */
WiFiManagerHelper::WiFiManagerHelper() {
}

/**
 * Enable or disable the serial debug
 * @param enabled enabler
 */
void WiFiManagerHelper::setSerialDebug(uint8_t enabled) {
  wifimanagerhelper_serialdebug_enabled = enabled;
}

/**
 * Enable or disable the status led
 * @param pin     PIN number
 * @param enabled enabler
 */
void WiFiManagerHelper::setStatusLed(uint8_t pin, uint8_t enabled) {
  wifimanagerhelper_statusled_pin = pin;
  wifimanagerhelper_statusled_enabled = enabled;
  wifimanagerhelper_statusLedTicker.detach();
  if(wifimanagerhelper_statusled_enabled) {
    //set wifi status led as output
    pinMode(wifimanagerhelper_statusled_pin, OUTPUT);
  }
}

/**
 * Enable or disable the check connection
 * @param interval interval in ms
 * @param enabled  enabler
 */
void WiFiManagerHelper::setCheckConnection(unsigned long interval, uint8_t enabled) {
  _checkconnection_enabled = enabled;
  _checkconnection_interval = interval;
  _checkconnection_timer = millis();
}

/**
 * Enable the captive portal on button press on initialization
 * @param pin     button pin
 * @param enabled enabler
 */
void WiFiManagerHelper::setCaptivePortalOnButton(uint8_t pin, uint8_t enabled) {
  _captiveportalonbutton_enabled = enabled;
  _captiveportalonbutton_pin = pin;
  if(_captiveportalonbutton_enabled) {
    pinMode(pin, INPUT);
  }
}

/**
 * Try to connect to an AP
 * @param hostname            hostname
 * @param apname              captive portal AP name
 * @param configportaltimeout config portal timeout
 * @param skipifnotconnected  do not block on connection
 */
void WiFiManagerHelper::connect(char *hostname, char *apname, int configportaltimeout, uint8_t skipifnotconnected) {
  //initialize wifi manager
  WiFiManager wifiManager;

  //print out info
  if(wifimanagerhelper_serialdebug_enabled) {
    Serial.print("Hostname: ");
    Serial.println(hostname);
    Serial.print("Captive portal name: ");
    Serial.println(apname);
  }

  //connecting...
  wifimanagerhelper_setStatus(WIFIMANAGERHELPER_STATUSTRYCONNECTION);

  //set hostname
  wifi_station_set_hostname(hostname);

  //try to connect
  if(wifimanagerhelper_serialdebug_enabled) {
    wifiManager.setDebugOutput(true);
  }
  wifiManager.setConfigPortalTimeout(configportaltimeout);
  wifiManager.setAPCallback(wifimanagerhelper_configModeCallback);

  //check if captive portal is force by button
  int forcecaptiveportal = 0;
  if(_captiveportalonbutton_enabled) {
    if (digitalRead(_captiveportalonbutton_pin) == LOW) {
      forcecaptiveportal = 1;
    }
  }

  //try to connect
  if(forcecaptiveportal) {
    wifiManager.startConfigPortal(apname);
  } else {
    if(!wifiManager.autoConnect(apname)) {
      if(wifimanagerhelper_serialdebug_enabled) {
        Serial.println("Failed to connect and hit timeout");
      }
      if(!skipifnotconnected) {
        if(wifimanagerhelper_serialdebug_enabled) {
          Serial.println("Restaring");
        }
        ESP.reset();
        delay(1000);
      }
    }
  }

  //multicast dns
  if(MDNS.begin(hostname)) {
    if(wifimanagerhelper_serialdebug_enabled) {
      Serial.println("MDNS responder started");
    }
  }

  //set connection status
  if(WiFi.status() == WL_CONNECTED) {
    wifimanagerhelper_setStatus(WIFIMANAGERHELPER_STATUSCONNECTED);
  } else {
    wifimanagerhelper_setStatus(WIFIMANAGERHELPER_STATUSNOTCONNECTED);
  }
}

/**
 * Reset WiFi settings
 */
void WiFiManagerHelper::resetSettings() {
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.restart();
  delay(1000);
}

/**
 * Check connection runner
 * @return is connected
 */
int WiFiManagerHelper::checkConnection() {
  if(_checkconnection_enabled) {
    //check connection
    if (millis() > _checkconnection_timer) {
      _checkconnection_timer = millis() + _checkconnection_interval;
      if(WiFi.status() != WL_CONNECTED) {
        if(wifimanagerhelper_serialdebug_enabled) {
          Serial.println("Connection checked failed");
        }
        wifimanagerhelper_setStatus(WIFIMANAGERHELPER_STATUSTRYCONNECTION);
      } else {
        if(wifimanagerhelper_serialdebug_enabled) {
          Serial.println("Connection checked successfully");
        }
        wifimanagerhelper_setStatus(WIFIMANAGERHELPER_STATUSCONNECTED);
      }
    }
  }
  return wifimanagerhelper_isconnected;
}

/**
 * Check if is connected
 * @return is connected
 */
int WiFiManagerHelper::isConnected() {
  return wifimanagerhelper_isconnected;
}
