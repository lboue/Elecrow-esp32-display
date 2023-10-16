#include <SPI.h>
#include <WiFi.h>
#include "Audio.h"
Audio audio;
String music_url;
int music_id = 1818615872;

#define I2S_DOUT      20
#define I2S_BCLK      35
#define I2S_LRC       19
#define BUTTON_PIN    38

const char *ssid = "elecrow888"; //Your network name
const char *password = "elecrow2014"; //Your network password
void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 ); /*Serial Port Initializing */
  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);
  delay(100);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.println("connecting");
  }
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(21); // 0...21
  pinMode(BUTTON_PIN, INPUT);
  music_url = "https://music.163.com/song/media/outer/url?id=" + String(music_id) + ".mp3";
  audio.connecttohost(music_url.c_str()); 
}

void loop() {
  // put your main code here, to run repeatedly:
  audio.loop();
  if (digitalRead(BUTTON_PIN) == HIGH)
  {
    delay(5);
    if (digitalRead(BUTTON_PIN) == HIGH)
    {
      Serial.println("Access to information...");
      audio.stopSong();
      music_id = music_id + 1;
      music_url = "https://music.163.com/song/media/outer/url?id=" + String(music_id) + ".mp3";
      audio.connecttohost(music_url.c_str()); 
      Serial.println(music_url);
      while (digitalRead(BUTTON_PIN) == HIGH);      
      Serial.println("start playing...");
    }
  }
}
