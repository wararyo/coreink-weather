#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "M5CoreInk.h"
#include "esp_adc_cal.h"
#include "images/background.c"
#include "images/cloudy.c"
#include "images/rainy.c"
#include "images/rainyandcloudy.c"
#include "images/snow.c"
#include "images/sunny.c"
#include "images/sunnyandcloudy.c"

Ink_Sprite weatherSprite(&M5.M5Ink);
Ink_Sprite temperatureSprite(&M5.M5Ink);
Ink_Sprite rainfallChanceSprite(&M5.M5Ink);

const char* endpoint = "https://www.drk7.jp/weather/json/13.js";
const char* region = "東京地方";
DynamicJsonDocument weatherInfo(20000);
 
void setup() {
    M5.begin();
    Wire.begin();
    Serial.begin(115200);
    WiFi.begin();
    //WiFi.begin("SSID","Key");
    weatherSprite.creatSprite(46,36,108,96);
     
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
    delay(1);
    if (M5.BtnUP.wasPressed()) {
        drawTodayWeather(); 
    }
    if (M5.BtnMID.wasPressed()) {
        drawTomorrowWeather();
    }
    if (M5.BtnDOWN.wasPressed()) {
        drawDayAfterTomorrowWeather();
    }
    if( M5.BtnPWR.wasPressed())
    {
        Serial.printf("Btn %d was pressed \r\n",BUTTON_EXT_PIN);
        digitalWrite(LED_EXT_PIN,LOW);
        M5.PowerDown();
    }
    M5.update();
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
    weatherSprite.clear();
    
    DynamicJsonDocument doc(20000);
    deserializeJson(doc, infoWeather);
    String weather = doc["weather"];
    if (weather.indexOf("雨") != -1) {
        if (weather.indexOf("くもり") != -1) {
            weatherSprite.drawBuff(0,0,108,96,rainyandcloudy);
        } else {
            weatherSprite.drawBuff(0,0,108,96,rainy);
        }
    } else if (weather.indexOf("晴") != -1) {
        if (weather.indexOf("くもり") != -1) {
            weatherSprite.drawBuff(0,0,108,96,sunnyandcloudy);
        } else {
            weatherSprite.drawBuff(0,0,108,96,sunny);
        }
    } else if (weather.indexOf("雪") != -1) {
            weatherSprite.drawBuff(0,0,108,96,snow);
    } else if (weather.indexOf("くもり") != -1) {
            weatherSprite.drawBuff(0,0,108,96,cloudy);
    }

    //weatherSprite.drawString(0,0,filename.c_str(),&AsciiFont8x16);
    weatherSprite.pushSprite();
  
//    String filePath = pictureFolder+filename;
//    M5.Lcd.drawJpgFile(SD,filePath.c_str());
//  
//    String maxTemperature = doc["temperature"]["range"][0]["content"];
//    String minTemperature = doc["temperature"]["range"][1]["content"];
//    drawTemperature(maxTemperature, minTemperature);
//  
//    String railfallchance0_6 = doc["rainfallchance"]["period"][0]["content"];
//    String railfallchance6_12 = doc["rainfallchance"]["period"][1]["content"];
//    String railfallchance12_18 = doc["rainfallchance"]["period"][2]["content"];
//    String railfallchance18_24 = doc["rainfallchance"]["period"][3]["content"];
//    drawRainfallChancce(railfallchance0_6, railfallchance6_12, railfallchance12_18, railfallchance18_24);
//  
//    drawDate(doc["date"]);
}

//void drawTemperature(String maxTemperature, String minTemperature) {
//    M5.Lcd.setTextColor(RED);
//    M5.Lcd.setTextSize(4);
//    M5.Lcd.setCursor(15,80);
//    M5.Lcd.print(maxTemperature);
//  
//    M5.Lcd.setTextColor(WHITE);
//    M5.Lcd.setCursor(70,80);
//    M5.Lcd.print("|");
//  
//    M5.Lcd.setTextColor(BLUE);
//    M5.Lcd.setCursor(105,80);
//    M5.Lcd.print(minTemperature);
//}
//
//void drawRainfallChancce(String rfc0_6, String rfc6_12, String rfc12_18, String rfc18_24) {
//    M5.Lcd.setTextSize(3);
//    M5.Lcd.setTextColor(WHITE);
//    M5.Lcd.setCursor(27,200);
//    M5.Lcd.print(rfc0_6);
//    
//    M5.Lcd.setCursor(92,200);
//    M5.Lcd.print(rfc6_12);
//  
//    M5.Lcd.setCursor(165,200);
//    M5.Lcd.print(rfc12_18);
//  
//    M5.Lcd.setCursor(245,200);
//    M5.Lcd.print(rfc18_24);
//}
//
//void drawDate(String date) {
//    M5.Lcd.setTextColor(WHITE);
//    M5.Lcd.setTextSize(3);
//    M5.Lcd.setCursor(10,10);
//    M5.Lcd.print(date);
//}
