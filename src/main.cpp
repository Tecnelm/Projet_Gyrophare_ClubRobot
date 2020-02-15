#include <Arduino.h>
#include "config.h"
#include "ESP8266WiFi.h"
#include "discord.h"
#include "ESP8266HTTPClient.h"


#define EDUORA
#include "wpa2_enterprise.h"

String lastID;
void setup()
{
    pinMode(GIROPIN, OUTPUT);
    Serial.begin(9600);
    #ifdef EDUORAM
    initWifiESP();
    #else
    initNormalWifi();
    #endif
}
void mf()
{
    
     HTTPClient http;  //Declare an object of class HTTPClient

    http.begin("http://garrigues.ovh");  //Specify request destination
    int httpCode = http.GET();     //Send the request                                                           

    if (httpCode > 0) { //Check the returning code

      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload

    }
}
void loop()
{
   if(WiFi.status() == WL_CONNECTED){
    String currentID = requestLastMessageID();

    if(!currentID.equals(lastID))
    {
        digitalWrite(GIROPIN,HIGH);
        delay(TIMEGIROON);
        digitalWrite(GIROPIN,LOW);
        lastID = currentID;

    }
    delay(2000);
   }else
   {
     #ifdef EDUORAM
    initWifiESP();
    #else
    initNormalWifi();
    #endif
   }
}

void  initWifiESP(){
    int nbtry = 0;

  Serial.print("Connecting to ");
  Serial.println(SSID);
   // Setting ESP into STATION mode only (no AP mode or dual mode)
  wifi_set_opmode(0x01);
  struct station_config wifi_config;
  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char*)wifi_config.ssid, SSID);
  wifi_station_set_config(&wifi_config);

  wifi_station_set_wpa2_enterprise_auth(1);
  wifi_station_set_enterprise_identity((uint8*)USERNAME, strlen(USERNAME));
  wifi_station_set_enterprise_username((uint8*)USERNAME, strlen(USERNAME));
  wifi_station_set_enterprise_password((uint8*)PASSWORD, strlen(PASSWORD));
  
  wifi_station_connect();

  Serial.println("Wifi station connect status:");
  // Wait for connection AND IP address from DHCP
  while (WiFi.status() != WL_CONNECTED) {
  
  delay(2000);
    Serial.println("Not connected"); 
    nbtry++;
    if(nbtry >= NBTRYWIFI)
    {
        while (1);
        
    }
  }

  wifi_station_clear_enterprise_identity();
  wifi_station_clear_enterprise_password();
  wifi_station_clear_enterprise_username();
  
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void initNormalWifi()
{
    int nbTry = 0;
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID,PASSWORD);
    Serial.println("Try Wifi Connection");
    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.println(".");
        delay(500);
        nbTry++;
        if(nbTry> NBTRYWIFI)
        {
            while (1);
            
        }
    }

    Serial.println("ESP CONNECTED");
    
}


String requestLastMessageID()
{
  HTTPClient http;
  String messageID;
  http.begin(HOST);

  int httpCode = http.GET();

  if(httpCode >0)
  {
    messageID = http.getString();
  }
  return messageID;
}

