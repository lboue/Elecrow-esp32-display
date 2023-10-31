#define D_PIN 11

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(D_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(D_PIN, HIGH);//pull high level
  delay(500);
  digitalWrite(D_PIN, LOW);//pull low level
  delay(500);
}
