#include <MQ135.h>
#include<bits/stdc++.h>
#include <DHT22.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#define BOTtoken ""
#define CHAT_ID ""
char buffer[100];
float thershold=10;
const char* ssid = "";
const char* password = "";
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
#define PIN_MQ135 34
#define PIN_DHT22 4
MQ135 mq135_sensor(PIN_MQ135);
DHT22 dht22(PIN_DHT22);
char* floatToString(float number, uint8_t precision, char* buffer, uint8_t size) {
  dtostrf(number, size, precision, buffer);
  return buffer;
}

void setup() {
  pinMode(5,OUTPUT);
  Serial.begin(9600);
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");
}
float avg1=0;
float avg2=0;
void loop() {
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(34, 64);
  float resistance = mq135_sensor.getResistance();
  int window=10;
  for(int i=0;i<window;i++){
    float correctedPPM = mq135_sensor.getCorrectedPPM(34, 64);
      delay(50);
      avg2+=correctedPPM/window;
    }
    avg1=avg2;
  }
  Serial.println(avg2);
  if(((avg2-avg1)>=thershold)){
    for(int i=0;i<5;i++){
      digitalWrite(5,HIGH);
      delay(50);
      digitalWrite(5,LOW);
      delay(50);
    }
  }
  else{
    digitalWrite(5,LOW);
    delay(500);
  }
  String s="PPM: "+String(floatToString(avg2,2,buffer,sizeof(buffer)));
  if(((avg2-avg1)>=thershold))s="Smoke Alert! "+s;
  bot.sendMessage(CHAT_ID, s, "");
  Serial.println("AQI");
  delay(1000);
}


