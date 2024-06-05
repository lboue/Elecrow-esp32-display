void setup() {
  Serial.begin(115200); // la vitesse de transmission est définie ici 
  Serial.println( "Setup done" );
}

void loop() {
  if (ESP_ARDUINO_VERSION_MAJOR == 2)
    Serial.println( "Arduino ESP 2.0" );
  else if (ESP_ARDUINO_VERSION_MAJOR == 3)
    Serial.println( "Arduino ESP 3.0" );
  sleep(1);
}
