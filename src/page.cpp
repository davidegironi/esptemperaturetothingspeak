/*
ESP Temperature to ThingSpeak v01

copyright (c) Davide Gironi, 2020

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include "main.h"

#include "page.h"

//initializer server
ESP8266WebServer server(80);

/**
 * Main page content
 * @return return the main page content
 */
String page() {
  String content = "";
  content += FPSTR(HTTP_PAGE);
  content += FPSTR(HTTP_HTMLSTART);
  content += FPSTR(HTTP_HEADSTART);
  content += FPSTR(HTTP_HEADMETA);
  content += FPSTR(HTTP_HEADTITLE);
  content += FPSTR(HTTP_HEADSTYLE);
  content += FPSTR(HTTP_HEADSCRIPT);
  content += FPSTR(HTTP_HEADEND);
  content += FPSTR(HTTP_BODYSTART);
  content += FPSTR(HTTP_CONTENTSTART);
  content += "{c}";
  content += FPSTR(HTTP_CONTENTEND);
  content += FPSTR(HTTP_BODYEND);
  content += FPSTR(HTTP_HTMLEND);
  return content;
}

/**
 * Emit the home page
 */
void pageHome() {
  //set page content
  String contentpage = FPSTR(HTTP_PAGEHOMESTATUS);

  //get main page content
  String content = page();
  content.replace("{c}", contentpage);
  
  //get last data
  content.replace("{d1}", getLastdata());

  //write page
  server.send(200, "text/html", content);
}

/**
 * Emit the WiFi settings page
 */
void pageWifi() {
  String contentpage = "";
  int deviceid = 0;
  String deviceids = "";

  //get vars
  if (server.arg("s") != "") {
    if (server.arg("n") != "") {
      eepromConfig.deviceid = atoi(server.arg("n").c_str());
    }
    if (server.arg("r") == "1") {
      wifiManagerHelper.resetSettings();
    }
    eepromWrite();
  }

  //read vars
  eepromRead();
  deviceid = eepromConfig.deviceid;
  char deviceidc[4];
  snprintf(deviceidc, sizeof(deviceidc), "%03d", deviceid);
  
  //set page output
  if (server.arg("s") != "") {
    //save and reboot
    contentpage = FPSTR(HTTP_PAGEWIFISAVE);
    char hostname [WIFI_HOSTNAMELENGTH + 4];
    strcpy(hostname, WIFI_HOSTNAME);
    strcat(hostname, deviceidc);
    contentpage.replace("{l}", hostname);
  } else {
    contentpage = FPSTR(HTTP_PAGEWIFI);
    contentpage.replace("{n}", deviceidc);
  }

  //get main page content
  String content = page();
  content.replace("{c}", contentpage);

  //write page
  server.send(200, "text/html", content);

  //reboot on save
  if (server.arg("s") != "") {
    delay(1000);
    ESP.restart();
    delay(1000);
  }
}

/**
 * Emit the Settings page
 */
void pageSettings() {
  String contentpage = "";
  String thingspeakapikey = "";
  String thingspeakchannelid = "";
  String unitoftemperature = "";
  String unitoftemperatureCselected = "";
  String unitoftemperatureFselected = "";

  //get vars
  if (server.arg("s") != "") {
    thingspeakapikey = server.arg("ta");
    if(thingspeakapikey.length() < sizeof(eepromConfig.thingspeakapikey)/sizeof(eepromConfig.thingspeakapikey[0]))
      strcpy(eepromConfig.thingspeakapikey, thingspeakapikey.c_str());
    thingspeakchannelid = server.arg("tc");
    if(thingspeakchannelid.length() < sizeof(eepromConfig.thingspeakchannelid)/sizeof(eepromConfig.thingspeakchannelid[0]))
      strcpy(eepromConfig.thingspeakchannelid, thingspeakchannelid.c_str());
    unitoftemperature = server.arg("ut");
    strcpy(eepromConfig.unitoftemperature, unitoftemperature.c_str());
    eepromWrite();
  }

  //read vars
  eepromRead();
  thingspeakapikey = String(eepromConfig.thingspeakapikey);
  thingspeakchannelid = String(eepromConfig.thingspeakchannelid);
  if(eepromConfig.unitoftemperature[0] == 'C')
    unitoftemperatureCselected = "selected";
  else if(eepromConfig.unitoftemperature[0] == 'F')
    unitoftemperatureFselected = "selected";

  //set page content
  if (server.arg("s") != "") {
    contentpage = FPSTR(HTTP_PAGESETTINGSSAVE);
  } else {
    contentpage = FPSTR(HTTP_PAGESETTINGS);
    contentpage.replace("{ta}", thingspeakapikey);
    contentpage.replace("{tc}", thingspeakchannelid);
    contentpage.replace("{utC}", unitoftemperatureCselected); 
    contentpage.replace("{utF}", unitoftemperatureFselected);   
  }

  //get main page content
  String content = page();
  content.replace("{c}", contentpage);

  //write page
  server.send(200, "text/html", content);
}

/**
 * Emit the Page Not Found
 */
void pageNotFound() {
  //set page content
  String contentpage = FPSTR(HTTP_PAGENOTFOUND);

  //get main page content
  String content = page();
  content.replace("{c}", contentpage);

  //write page
  server.send(200, "text/html", content);
}

/**
 * initialized the web server
 */
void pageInit() {
  //initialize server
  server.on("/", pageHome);
  server.on("/wifi", pageWifi);
  server.on("/settings", pageSettings);
  server.onNotFound(pageNotFound);
  server.begin();
}

void pageHandleClient() {
  server.handleClient();
}
