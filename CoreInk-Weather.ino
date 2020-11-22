#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "M5CoreInk.h"

#include "Battery.h"
#include "images/background.h"
#include "images/cloudy.h"
#include "images/rainy.h"
#include "images/rainyandcloudy.h"
#include "images/snow.h"
#include "images/sunny.h"
#include "images/sunnyandcloudy.h"

#define SHOW_LAST_UPDATED false // 天気を更新した時刻を表示するかどうか
#define SHOW_BATTERY_CAPACITY true // 電池残量を表示するかどうか

const char* endpoint = "https://www.drk7.jp/weather/json/13.js";
const char* region = "東京地方";

Ink_Sprite dateSprite(&M5.M5Ink);
Ink_Sprite weatherSprite(&M5.M5Ink);
Ink_Sprite temperatureSprite(&M5.M5Ink);
Ink_Sprite rainfallChanceSprite(&M5.M5Ink);

DynamicJsonDocument weatherInfo(20000);
 
void setup() {
    M5.begin();
    Wire.begin();
    Serial.begin(115200);
    WiFi.begin();
    //WiFi.begin("SSID","Key");
    dateSprite.creatSprite(0,0,200,200); // typo of "create"
    weatherSprite.creatSprite(0,0,200,200);
    temperatureSprite.creatSprite(0,0,200,200);
    rainfallChanceSprite.creatSprite(0,0,200,200);
     
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
    weatherInfo = getJson();
    WiFi.disconnect();
    drawTodayWeather(); 
}
 
void loop() {
    delay(1000);
    // if (M5.BtnUP.wasPressed()) {
    //     drawTodayWeather(); 
    // }
    // if (M5.BtnMID.wasPressed()) {
    //     drawTomorrowWeather();
    // }
    // if (M5.BtnDOWN.wasPressed()) {
    //     drawDayAfterTomorrowWeather();
    // }
    // if( M5.BtnPWR.wasPressed())
    // {
    //     Serial.printf("Btn %d was pressed \r\n",BUTTON_EXT_PIN);
    //     digitalWrite(LED_EXT_PIN,LOW);
    //     M5.PowerDown();
    // }
    // M5.update();
    digitalWrite(LED_EXT_PIN,LOW);
    M5.shutdown(3600);
}

DynamicJsonDocument getJson() {
    DynamicJsonDocument doc(20000);
  
    if ((WiFi.status() == WL_CONNECTED)) {
        HTTPClient http;
        http.begin(endpoint);
        int httpCode = http.GET();
        if (httpCode > 0) {
            //jsonオブジェクトの作成
            String jsonString = createJson(http.getString());
            deserializeJson(doc, jsonString);
        } else {
            Serial.println("Error on HTTP request");
        }
        http.end(); //リソースを解放
    }
    return doc;
}

// JSONP形式からJSON形式に変える
String createJson(String jsonString){
    jsonString.replace("drk7jpweather.callback(","");
    return jsonString.substring(0,jsonString.length()-2);
}

void drawTodayWeather() {
    String today = weatherInfo["pref"]["area"][region]["info"][0];
    drawWeather(today);
}

void drawTomorrowWeather() {
    String tomorrow = weatherInfo["pref"]["area"][region]["info"][1];
    drawWeather(tomorrow);
}

void drawDayAfterTomorrowWeather() {
    String dayAfterTomorrow = weatherInfo["pref"]["area"][region]["info"][2];
    drawWeather(dayAfterTomorrow);
}

void drawWeather(String infoWeather) {
    M5.M5Ink.clear();
    M5.M5Ink.drawBuff((uint8_t *)image_background);
    weatherSprite.clear();
    
    DynamicJsonDocument doc(20000);
    deserializeJson(doc, infoWeather);
    String weather = doc["weather"];
    if (weather.indexOf("雨") != -1) {
        if (weather.indexOf("くもり") != -1) {
            weatherSprite.drawBuff(46,36,108,96,image_rainyandcloudy);
        } else {
            weatherSprite.drawBuff(46,36,108,96,image_rainy);
        }
    } else if (weather.indexOf("晴") != -1) {
        if (weather.indexOf("くもり") != -1) {
            weatherSprite.drawBuff(46,36,108,96,image_sunnyandcloudy);
        } else {
            weatherSprite.drawBuff(46,36,108,96,image_sunny);
        }
    } else if (weather.indexOf("雪") != -1) {
            weatherSprite.drawBuff(46,36,108,96,image_snow);
    } else if (weather.indexOf("くもり") != -1) {
            weatherSprite.drawBuff(46,36,108,96,image_cloudy);
    }
    weatherSprite.pushSprite();
 
   String maxTemperature = doc["temperature"]["range"][0]["content"];
   String minTemperature = doc["temperature"]["range"][1]["content"];
   drawTemperature(maxTemperature, minTemperature);
 
    int rainfallChances[] = {doc["rainfallchance"]["period"][0]["content"].as<int>(),
        doc["rainfallchance"]["period"][1]["content"].as<int>(),
        doc["rainfallchance"]["period"][2]["content"].as<int>(),
        doc["rainfallchance"]["period"][3]["content"].as<int>()};

    int maxRainfallChance = -255;
    int minRainfallChance = 255;

    for(int item : rainfallChances) {
        if(item > maxRainfallChance) maxRainfallChance = item;
        if(item < minRainfallChance) minRainfallChance = item;
    }
    
   drawRainfallChance(String(maxRainfallChance),String(minRainfallChance));
 
   drawDate(doc["date"]);
}

void drawTemperature(String maxTemperature, String minTemperature) {
    temperatureSprite.clear();
    temperatureSprite.drawString(60,147,maxTemperature.c_str(),&AsciiFont8x16);
    temperatureSprite.drawString(60,169,minTemperature.c_str(),&AsciiFont8x16);
    temperatureSprite.pushSprite();
}

void drawRainfallChance(String maxRainfallChance,String minRainfallChance) {
    rainfallChanceSprite.clear();
    rainfallChanceSprite.drawString(142,147,maxRainfallChance.c_str(),&AsciiFont8x16);
    rainfallChanceSprite.drawString(142,169,minRainfallChance.c_str(),&AsciiFont8x16);
    rainfallChanceSprite.pushSprite();
}

void drawDate(String date) {
    dateSprite.clear();
    dateSprite.drawString(60,16,date.c_str(),&AsciiFont8x16);
    
    // Draw last updated time
    if(SHOW_LAST_UPDATED){
        RTC_DateTypeDef RTCdate;
        RTC_TimeTypeDef RTCtime;
        M5.rtc.GetTime(&RTCtime);
        M5.rtc.GetData(&RTCdate); // typo of "Date"
        String nowString = String("Updated: ") + dateTimeToString(RTCdate,RTCtime);
        dateSprite.drawString(0,184,nowString.c_str(),&AsciiFont8x16);
    }

    //Draw battery capacity
    if(SHOW_BATTERY_CAPACITY) dateSprite.drawString(176,184,(String(getBatCapacity()) + String("%")).c_str(),&AsciiFont8x16);

    dateSprite.pushSprite();
}

String dateTimeToString(RTC_DateTypeDef RTCdate, RTC_TimeTypeDef RTCtime) {
    return String(RTCdate.Month)+String("/")+String(RTCdate.Date) + String(" ") + String(RTCtime.Hours)+String(":")+String(RTCtime.Minutes);
}
