#include <TFT_eSPI.h>
TFT_eSPI lcd = TFT_eSPI(); /* TFT Example */
uint16_t touchX, touchY;
uint16_t calData[5] = { 189, 3416, 359, 3439, 1 };

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 ); /*Serial Port Initializing */
  //LCD initialization
  lcd.begin(); 
  lcd.setRotation(1); /* revolve */
  //Calibration modes. One is four-corner positioning, and the other is direct input of analog values for direct positioning.
  //screen calibration
//  touch_calibrate();
  lcd.setTouch(calData);

  /*initialization*/
}

void loop() {
  // put your main code here, to run repeatedly:
  bool touched = lcd.getTouch( &touchX, &touchY, 600);
  if ( touched )
  {
    Serial.print( "Data x " );
    Serial.println( touchX );

    Serial.print( "Data y " );
    Serial.println( touchY );
  }
}

void touch_calibrate()//screen calibration
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;
  Serial.println("screen calibration");

  //calibration
  Serial.println("Touch corners as directed");

  lv_timer_handler();
  lcd.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
  Serial.println("calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15)");
  Serial.println(); Serial.println();
  Serial.println("// Use this calibration code in setup().");
  Serial.print("uint16_t calData[5] = ");
  Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }

  Serial.println(" };");
  Serial.print("  tft.setTouch(calData);");
  Serial.println(); Serial.println();


}
