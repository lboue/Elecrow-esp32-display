#include <WiFi.h>

const char *ssid = "elecrow888"; //Your network name
const char *password = "elecrow2014"; //Your network password
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);//Setting up automatic reconnection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("connecting");
  }
  Serial.println("WiFi is connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
//  WiFi.disconnect();//Disconnect WIFI
}

void loop() {

}
