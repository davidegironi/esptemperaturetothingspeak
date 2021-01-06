/*
ESP Temperature to ThingSpeak v01

copyright (c) Davide Gironi, 2020

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#ifndef pages_h
#define pages_h

#include <Arduino.h>

//include main configuration
#include "main.h"

//WebServer library
#include <ESP8266WebServer.h>

//WiFiManager helper library
#include "WiFiManagerHelper/WiFiManagerHelper.h"
extern WiFiManagerHelper wifiManagerHelper;

//include eepromConfig
#include "eepromconfig.h"
extern struct EEPROMConfigStruct eepromConfig;

//html page constants
const char HTTP_PAGE[] PROGMEM = "<!DOCTYPE html>";
const char HTTP_HTMLSTART[] PROGMEM = "<html lang=\"en\">";
const char HTTP_HTMLEND[] PROGMEM = "</html>";
const char HTTP_HEADSTART[] PROGMEM = "<head>";
const char HTTP_HEADEND[] PROGMEM = "</head>";
const char HTTP_HEADMETA[] PROGMEM = "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
const char HTTP_HEADTITLE[] PROGMEM = "<title>ESP/Temperature to ThingSpeak</title>";
const char HTTP_HEADSTYLE[] PROGMEM = "<style>body{text-align:center;font-family:verdana;font-size:80%}div.b{text-align:left;display:inline-block;min-width:260px;max-width:650px;width:100%;margin:20px 0 0}h2{text-align:center}div.m{background-color:#000;padding:10px;color:#fff}div.m a{color:#fff;text-decoration:none}p{margin:0 0 10px;padding:0}input,label{display:block}input[type=text]{width:100%;box-sizing:border-box;webkit-box-sizing:border-box;-moz-box-sizing:border-box}textarea{width:100%;box-sizing:border-box;webkit-box-sizing:border-box;-moz-box-sizing:border-box}</style>";
const char HTTP_HEADSCRIPT[] PROGMEM = "<script></script>";
const char HTTP_BODYSTART[] PROGMEM = "<body>";
const char HTTP_BODYEND[] PROGMEM = "</body>";
const char HTTP_CONTENTSTART[] PROGMEM = "<div class='b'><h2>ESP/Temperature to ThingSpeak</h2><div class='m'><a href='/'>Home</a> | <a href='wifi' class='w'>WiFi</a> | <a href='settings' class='s'>Settings</a></div><div class='p'>";
const char HTTP_CONTENTEND[] PROGMEM = "</div></div>";
const char HTTP_PAGENOTFOUND[] PROGMEM = "<h2>Page not found!</h2>";
const char HTTP_PAGEHOMESTATUS[] PROGMEM = "<h2>Status</h2>{d1}<br/><br/>";
const char HTTP_PAGEWIFI[] PROGMEM = "<h2>WiFi Configuration</h2><script>function validate(e){var n='';if(isNaN(e.elements.n.value)||!(parseInt(e.elements.n.value)>=0&&parseInt(e.elements.n.value)<=999)) n+='Invalid Device ID, must be a number between 0 and 999. ';if(n !=''){alert(n); return false;}return true;}</script><form method='post' onsubmit='return validate(this);' action='wifi'><p><label>Device ID</label><input type='text' id='n' name='n' length=3 value='{n}'/></p><p><label>Reset WiFi settings</label><input type='checkbox' id='r' name='r' value='1'/></p><br/><input type='hidden' id='s' name='s' value='1'/><button type='submit'>save</button></form>";
const char HTTP_PAGEWIFISAVE[] PROGMEM = "<h2>WiFi Configuration</h2>Settings has been saved.<br/>Device will be rebooted in 10s.<br/><script>setTimeout(function(){ window.location.replace('http://{l}'); }, 10000);</script>";
const char HTTP_PAGESETTINGS[] PROGMEM = "<h2>Settings</h2><script>function validate(e){var n=''; if(e.elements.ta.value.length > 20) n+='Invalid ThingSpeak API Key, max 20 characters. '; if(isNaN(e.elements.tc.value) || !(parseInt(e.elements.tc.value) >=0 && parseInt(e.elements.tc.value)<=9999999999)) n+='Invalid ThingSpeak Channel ID, must be a number between 0 and 9999999999. '; if(n !=''){alert(n); return false;}return true;}</script><form method='post' onsubmit='return validate(this);' action='settings'> <p><label>ThingSpeak API Key</label><input type='text' id='ta' name='ta' length=20 value='{ta}'/></p><p><label>ThingSpeak Channel ID</label><input type='text' id='tc' name='tc' length=10 value='{tc}'/></p><br/> <p><label>Unit of temperature</label><select id='ut' name='ut'><option value='C' {utC}>Celsius</option><option value='F' {utF}>Fahrenheit</option></select></p><br/><input type='hidden' id='s' name='s' value='1'/><button type='submit'>save</button></form>";
const char HTTP_PAGESETTINGSSAVE[] PROGMEM = "<h2>Settings</h2>Settings has been saved.<br/>Page will be reloaded in 2s.<br/><script>setTimeout(function(){ window.location=window.location.origin+window.location.pathname; }, 2000);</script>";

//export functions
extern void pageInit();
extern void pageHandleClient();

#endif
