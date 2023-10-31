#include <lvgl.h>
#include <TFT_eSPI.h>
#include <demos/lv_demos.h>
#include <examples/lv_examples.h>

#include <Arduino.h>
#include <U8g2lib.h>
//#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
//#endif
#include <WiFi.h>
#include "MusicDefinitions.h"
#include "SoundData.h"
#include "XT_DAC_Audio.h"

#include "BLEDevice.h"              //BLE Driver Library
#include "BLEServer.h"              //BLE Bluetooth Server Library
#include "BLEUtils.h"               //BLE Utility Library
#include "BLE2902.h"                //Feature Addition Descriptor Library
#include <BLECharacteristic.h>      //BLE Feature Function Library
BLEAdvertising* pAdvertising = NULL;
BLEServer* pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic* pCharacteristic = NULL;
#define bleServerName "Wizee-Series-3.5"  //Name of the BLE server
#define SERVICE_UUID "6479571c-2e6d-4b34-abe9-c35116712345"  //UUID of the service
#define CHARACTERISTIC_UUID "826f072d-f87c-4ae6-a416-6ffdcaa02d73"

const char *ssid = "elecrow888"; //Your network name
const char *password = "elecrow2014"; //Your network password
char buf[100] = {};
int bufindex = 0;
int wifi_close_flag = 0;
char *info[100] = {};
int wifi_flag = 0;
int i = 0;
int home_flag = 0;
int touch_flag = 0;

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C  u8g2(U8G2_R0, /* clock=*/ 21, /* data=*/ 22, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED

//UI
#include "ui.h"
#include <Ticker.h>          //Call the ticker. H Library
Ticker ticker1;
static int first_flag = 0;
extern int zero_clean;
extern int goto_widget_flag;
extern int bar_flag;
extern lv_obj_t * ui_MENU;
extern lv_obj_t * ui_TOUCH;
extern lv_obj_t * ui_JIAOZHUN;
extern lv_obj_t * ui_Label2;
static lv_obj_t * ui_Label;//TOUCH interface label
static lv_obj_t * ui_Label3;//TOUCH interface label3
static lv_obj_t * ui_Labe2;//Menu interface progress bar label
static lv_obj_t * bar;//Menu interface progress bar
static int val = 100;

//3.5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_SCK 18
#define SD_CS 5

//SPEAK
// Data for the melody. Note followed by optional change in playing length in 1/4 beats. See documentation for more details
int8_t PROGMEM TwinkleTwinkle[] = {
  NOTE_C5, NOTE_C5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_A5, NOTE_G5, BEAT_2,
  NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_D5, NOTE_C5, BEAT_2,
  NOTE_G5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_D5, BEAT_2,
  NOTE_G5, NOTE_G5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_D5, BEAT_2,
  NOTE_C5, NOTE_C5, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_A5, NOTE_G5, BEAT_2,
  NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_D5, NOTE_C5, BEAT_4,
  NOTE_SILENCE, BEAT_5, SCORE_END
};
XT_DAC_Audio_Class DacAudio(26, 0);         // Create the main player class object. Use GPIO 25, one of the 2 DAC pins and timer 0
XT_Wav_Class ForceWithYou(Force);           // create WAV object and pass in the WAV data
XT_MusicScore_Class Music(TwinkleTwinkle, TEMPO_ALLEGRO, INSTRUMENT_PIANO); // The music score object, pass in the Music data
XT_Sequence_Class Sequence;                // The sequence object, you add your sounds above to this object (see setup below)

/*Changing the screen resolution*/
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[ screenWidth * screenHeight / 13 ];

TFT_eSPI lcd = TFT_eSPI(); /* TFT Example */

bool connected_state = false;   //Creating a Device Connection Identifier

class MyServerCallbacks: public BLEServerCallbacks  //Create connect and disconnect calling classes
{
    void onConnect(BLEServer *pServer)//Start connection function
    {
      connected_state = true;   //Equipment is properly connected
    }
    void onDisconnect(BLEServer *pServer)//Disconnect Function
    {
      connected_state = false;  //Device connection error
    }

};

//_______________________
/* Display Refresh */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
  uint32_t w = ( area->x2 - area->x1 + 1 );
  uint32_t h = ( area->y2 - area->y1 + 1 );

  lcd.startWrite();
  lcd.setAddrWindow( area->x1, area->y1, w, h );
  lcd.pushColors( ( uint16_t * )&color_p->full, w * h, true );
  lcd.endWrite();

  lv_disp_flush_ready( disp );
}

uint16_t touchX, touchY;
/*Read Touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{

  bool touched = lcd.getTouch( &touchX, &touchY, 600);
  if ( !touched )
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Setting the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    Serial.print( "Data x " );
    Serial.println( touchX );

    Serial.print( "Data y " );
    Serial.println( touchY );
  }
}


unsigned char buffer[256]; // buffer array for data recieve over serial port
int count = 0;   // counter for buffer array
void clearBufferArray()              // function to clear buffer array
{
  for (int i = 0; i < count; i++)
  {
    buffer[i] = NULL; // clear all index of array with command NULL
  }
}

char CloseData;
int NO_Test_Flag = 0;
int Test_Flag = 0;
int Close_Flag = 0;
uint16_t calData[5] = { 353, 3568, 269, 3491, 7  };
void callback1()  //Callback function
{
  if (bar_flag == 6)
  {
    if (val > 1)
    {
      val--;
      lv_bar_set_value(bar, val, LV_ANIM_OFF);
      lv_label_set_text_fmt(ui_Labe2, "%d %%", val);
    }
    else
    {
      lv_obj_clear_flag(ui_touch, LV_OBJ_FLAG_CLICKABLE);
      lv_label_set_text(ui_Labe2, "Loading");
      delay(150);
      val = 100;
      bar_flag = 0; //Stop progress bar sign
      goto_widget_flag = 1; //Widget Enter logo

    }
  }
}



void setup()
{
  Serial.begin( 9600 ); /*Serial Port Initializing */
  Serial2.begin( 9600 ); /*Serial Port Initializing 2*/

  //IO Port Pins
  pinMode(25, OUTPUT);
  digitalWrite(25, LOW);

  //BLE
  BLEDevice::init(bleServerName);  //Create BLE and set the name
  pServer = BLEDevice::createServer();  //Creating a BLE server
  pServer->setCallbacks(new MyServerCallbacks());  //Setting up the connect and disconnect calling classes
  pService = pServer->createService(SERVICE_UUID); //Creating BLE service
  pCharacteristic = pService->createCharacteristic(  //Create ble feature（Characterristic_UUID）
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->setValue("ELECROW");
  //start broadcasting
  pAdvertising = BLEDevice::getAdvertising();  //A bleadvertising class pointer padvertising is defined, which points to bledevice:: getadvertising()
  pAdvertising->addServiceUUID(SERVICE_UUID);

  //OLED
  //  u8g2.begin();
  //  u8g2.enableUTF8Print();        // enable UTF8 support for the Arduino print() function
  //  u8g2.setFont(u8g2_font_ncenB14_tr);
  //  u8g2.setFontDirection(0);
  //  for (int i = 128; i > -78; i -= 20)
  //  {
  //    u8g2.firstPage();
  //    do {
  //      u8g2.drawStr(i, 25, "ELECROW");
  //      delay(2);
  //    } while ( u8g2.nextPage() );
  //  }

  //lvgl initialization
  lv_init();

  //LCD initialization
  lcd.begin();          /*initialization*/
  lcd.setRotation(1); /* revolve */
  lcd.fillScreen(TFT_BLACK);
  delay(100);
  //Backlight Pins
  pinMode(27, OUTPUT);
  digitalWrite(27, HIGH);
  //  lcd.fillScreen(TFT_RED);
  //  delay(500);
  //  lcd.fillScreen(TFT_GREEN);
  //  delay(500);
  //  lcd.fillScreen(TFT_BLUE);
  //  delay(500);
  //  lcd.fillScreen(TFT_BLACK);
  //  delay(500);

  //SD Card
  //  SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
  //  delay(100);
  //  if (SD_init() == 1)
  //  {
  //    Serial.println("SD Cardinitialization failed！");
  //  }
  //  else
  //    Serial.println("SD Cardinitialization  successed");
  //  delay(2000);

  //Calibration modes. One is four-corner positioning, and the other is direct input of analog values for direct positioning.
  //screen calibration
  //  touch_calibrate();
  lcd.setTouch(calData);


  lv_disp_draw_buf_init( &draw_buf, buf1, NULL, screenWidth * screenHeight / 13 );

  /*display Initialization*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  /*Change the following line to display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register( &disp_drv );

  /*Initialization (virtualization) of input device drivers*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register( &indev_drv );

  ui_init();//Boot UI
  while (1)
  {
    if (goto_widget_flag == 1)//Go to widget
    {
      if (ticker1.active() == true)
      {
        ticker1.detach();
      }
      goto_widget_flag = 0;
      delay(300);
      break;
    }

    if (goto_widget_flag == 3)//Go to the touch screen and close the progress bar thread first
    {
      bar_flag = 0; //Stop progress bar sign
      if (ticker1.active() == true)
      {
        ticker1.detach();
      }
      if (first_flag == 0 || first_flag == 1)
      {
        label_xy();
        first_flag = 2;
      }
      if (zero_clean == 1)
      {
        touchX = 0;
        touchY = 0;
        zero_clean = 0;
      }
      lv_label_set_text(ui_Label, "Touch Adjust:");
      lv_label_set_text_fmt(ui_Label3, "%d  %d", touchX, touchY); //Display touch information
    }

    if (goto_widget_flag == 4)//Touch the screen to return to the Menu screen to fill the progress bar.
    {
      val = 100;
      delay(100);
      ticker1.attach_ms(35, callback1);//Callback1 is called every 35ms.
      goto_widget_flag = 0;
    }

    if (goto_widget_flag == 5) //Trigger calibration signal
    {
      lv_scr_load_anim(ui_touch_calibrate, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
      lv_timer_handler();
      lv_timer_handler();
      delay(100);
      touch_calibrate();//Touch Calibration
      lcd.setTouch( calData );
      lv_scr_load_anim(ui_TOUCH, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
      lv_timer_handler();
      delay(100);
      goto_widget_flag = 3; //Access to the touch screen logo
      touchX = 0;
      touchY = 0;
    }

    if (bar_flag == 6)//Runs the progress bar once when you first boot into the Menu screen, then stops running after that
    {
      if (first_flag == 0)
      {
        lv_example_bar();
        ticker1.attach_ms(35, callback1);//Callback1 is called every 35ms.
        first_flag = 1;
      }
    }

    lv_timer_handler();
  }


  lcd.fillScreen(TFT_BLACK);
  lv_demo_widgets();//Main UI
  Serial.println( "Setup done" );
}

void loop()
{

  while (NO_Test_Flag == 0)
  {
    lv_timer_handler();
    delay( 5 );
    CloseData = Serial.read();
    if (CloseData == 'b')  //Exit
    {
      NO_Test_Flag = 1;
      Test_Flag = 1;
      lcd.fillScreen(TFT_BLACK);
      Serial.println("Access to the test program");
      WiFi.disconnect();
      break;
    }
  }

  //*********************************Test Program***************************************
  //*********************************Test Program***************************************
  //*********************************Test Program***************************************
  while (Test_Flag == 1)
  {
    Ce_shi();
  }
}


//Touch Label Controls
void label_xy()
{
  ui_Label = lv_label_create(ui_TOUCH);
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Label, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label, LV_SIZE_CONTENT);    /// 1
  lv_obj_set_x(ui_Label, -35);
  lv_obj_set_y(ui_Label, -60);
  lv_obj_set_align(ui_Label, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

  ui_Label3 = lv_label_create(ui_TOUCH);
  lv_obj_enable_style_refresh(true);
  lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);   /// 1
  lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);    /// 1
  lv_obj_set_x(ui_Label3, 65);
  lv_obj_set_y(ui_Label3, -60);
  lv_obj_set_align(ui_Label3, LV_ALIGN_CENTER);
  lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_opa(ui_Label3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(ui_Label3, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
}

//Progress bar control
void lv_example_bar(void)
{
  //////////////////////////////
  bar = lv_bar_create(ui_MENU);
  lv_bar_set_value(bar, 0, LV_ANIM_OFF);
  lv_obj_set_width(bar, 300);
  lv_obj_set_height(bar, 20);
  lv_obj_set_x(bar, 0);
  lv_obj_set_y(bar, 120);
  lv_obj_set_align(bar, LV_ALIGN_CENTER);
  lv_obj_set_style_bg_img_src(bar, &ui_img_bar_480_01_png, LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_set_style_bg_img_src(bar, &ui_img_bar_480_02_png, LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_color(bar, lv_color_hex(0x2D8812), LV_PART_INDICATOR | LV_STATE_DEFAULT);
  lv_obj_set_style_outline_opa(bar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
  //////////////////////
  ui_Labe2 = lv_label_create(bar);//Creating Tags
  lv_obj_set_style_text_color(ui_Labe2, lv_color_hex(0x09BEFB), LV_STATE_DEFAULT);
  lv_label_set_text(ui_Labe2, "0%");
  lv_obj_center(ui_Labe2);
}


//Setting a black background
void Home_Page_Create(void)
{
  static lv_style_t style;                  //Creating Styles
  lv_style_init(&style);                    //initialization style

  lv_obj_t* bgk;
  bgk = lv_obj_create(lv_scr_act());//create object
  lv_obj_set_style_bg_color(bgk, lv_color_hex(0x000000 ), LV_STATE_DEFAULT); // obj background color set to yellow

  lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_NONE)); //Setting the frame color
  lv_obj_add_style(bgk, &style, 0);         //Adding Styles to Text Objects
  lv_obj_set_size(bgk, 480, 320);             // Set to screen size

}

void Ce_shi() {
  char serialData = Serial.read();
  switch (serialData) {
    case 'R':
      Close_Flag = 1;
      //      WiFi.disconnect();//Disconnect
      lcd.fillScreen(TFT_BLACK);
      Serial.println("Red Screen");
      while (Close_Flag == 1)
      {
        lcd.fillScreen(TFT_RED);
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Exit Red Screen");
          break;
        }
      }
      break;

    case 'G':
      Close_Flag = 1;
      lcd.fillScreen(TFT_BLACK);
      Serial.println("green screen");
      while (Close_Flag == 1)
      {
        lcd.fillScreen(TFT_GREEN);
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Exit green screen");
          break;
        }
      }
      break;

    case 'B':
      Close_Flag = 1;
      lcd.fillScreen(TFT_BLACK);
      Serial.println("Blue Screen");
      while (Close_Flag == 1)
      {
        lcd.fillScreen(TFT_BLUE);
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Exit Blue Screen");
          break;
        }
      }
      break;

    case 'S':
      Close_Flag = 1;
      lcd.fillScreen(TFT_BLACK);
      lcd.setTextFont(1);
      SPI.end();
      Serial.println("TF Card initialization");
      SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
      SD_init();
      while (Close_Flag == 1)
      {
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Exit TF Card initialization");
          SPI.end();
          SPI.begin(14, 12, 13, 33);//Touch Pins
          break;
        }
      }
      break;

    case 'T':
      Close_Flag = 1;
      lcd.fillScreen(TFT_BLACK);
      Serial.println("touch screens");
      delay(100);
      touch_flag = 0;
      lcd.setCursor(70, 70, 4);
      lcd.printf("Please Touch");
      Serial.println("TOUCH");
      delay(100);
      while (Close_Flag == 1)
      {
        uint16_t x, y;
        if (touch_flag < 3)
        {
          if (lcd.getTouch(&x, &y))
          {
            if (x > 0 && x < 70 && y > 0 && y < 70 && touch_flag == 0)
            {
              lcd.setCursor(140, 120, 4);
              lcd.printf("               ");
              lcd.setCursor(140, 150, 4);
              lcd.printf("               ");
              lcd.setCursor(120, 120, 4);
              lcd.printf("x: %i     ", x);
              lcd.setCursor(120, 150, 4);
              lcd.printf("y: %i    ", y);
              Serial.print( "Data x " );
              Serial.println( x );
              Serial.print( "Data y " );
              Serial.println( y );
              touch_flag = 1;
            }
            if (x > 410 && x < 480 && y > 250 && y < 320 && touch_flag == 1)
            {
              lcd.setCursor(140, 120, 4);
              lcd.printf("               ");
              lcd.setCursor(140, 150, 4);
              lcd.printf("               ");
              lcd.setCursor(120, 120, 4);
              lcd.printf("x: %i     ", x);
              lcd.setCursor(120, 150, 4);
              lcd.printf("y: %i    ", y);
              Serial.print( "Data x " );
              Serial.println( x );
              Serial.print( "Data y " );
              Serial.println( y );
              touch_flag = 3;
              Serial.println("OVER");
              delay(100);
              Serial.println("OVER");
            }
            if (x > 0 && x < 4000 && y > 0 && y < 4000)
            {
              lcd.setCursor(140, 120, 4);
              lcd.printf("               ");
              lcd.setCursor(140, 150, 4);
              lcd.printf("               ");
              lcd.setCursor(120, 120, 4);
              lcd.printf("x: %i     ", x);
              lcd.setCursor(120, 150, 4);
              lcd.printf("y: %i    ", y);
              Serial.print( "Data x " );
              Serial.println( x );
              Serial.print( "Data y " );
              Serial.println( y );
            }
            CloseData = Serial.read();
            if (CloseData == 'I')  //Exit
            {
              Close_Flag = 0;
              touch_flag = 0;
              lcd.fillScreen(TFT_BLACK);
              break;
            }
            delay(80);
          }
        }
        if (touch_flag == 3)
        {
          Serial.println("OVER");
          if (lcd.getTouch(&x, &y))
          {
            lcd.fillScreen(TFT_BLACK);
            if (x > 0 && x < 4000 && y > 0 && y < 4000)
            {
              lcd.setCursor(140, 120, 4);
              lcd.printf("               ");
              lcd.setCursor(140, 150, 4);
              lcd.printf("               ");
              lcd.setCursor(120, 120, 4);
              lcd.printf("x: %i     ", x);
              lcd.setCursor(120, 150, 4);
              lcd.printf("y: %i    ", y);
              Serial.print( "Data x " );
              Serial.println( x );
              Serial.print( "Data y " );
              Serial.println( y );
            }
          }
          delay(80);
        }
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Exit touch screens");
          break;
        }
      }
      break;

    case 'L':
      Close_Flag = 1;
      Serial.println("IO Port Test");
      lcd.fillScreen(TFT_BLACK);
      lcd.setCursor(50, 120, 4);
      lcd.printf("IO port output testing");
      while (Close_Flag == 1)
      {
        digitalWrite(25, HIGH);//Pull high level to turn on the light
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          digitalWrite(25, LOW);
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Exit IO Port Test");
          break;
        }
      }
      break;

    case 'i':
      Close_Flag = 1;
      u8g2.begin();
      u8g2.enableUTF8Print();        // enable UTF8 support for the Arduino print() function
      u8g2.setFont(u8g2_font_ncenB14_tr);
      u8g2.setFontDirection(0);
      Serial.println("IIC");
      lcd.fillScreen(TFT_BLACK);
      lcd.setCursor(100, 120, 4);
      lcd.printf("IIC testing");
      while (Close_Flag == 1)
      {
        for (int i = 128; i > -90; i -= 20)
        {
          u8g2.firstPage();
          do {
            u8g2.drawStr(i, 25, "ELECROW");
            delay(2);
          } while ( u8g2.nextPage() );
        }
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Exit IIC");
          break;
        }
      }
      break;

    case 'W':
      Close_Flag = 1;
      Serial.println("Wifi function testing");
      lcd.fillScreen(TFT_BLACK);
      lcd.setCursor(100, 120, 4);
      lcd.printf("WiFi testing");
      wifi_flag = 0;
      while (Close_Flag == 1)
      {
        while (Serial.available() && wifi_flag == 0)
        {
          CloseData = Serial.read();
          buf[bufindex++] = CloseData;
          if (CloseData == '\n')
          {
            buf[bufindex] = '\0';
            info[0] = buf;
            info[0] = strtok_r(info[0], "@", &info[1]);
            info[1] = strtok_r(info[1], "@", &info[2]);
            //sscanf(buf, "@%s@%s\n", wifi_account, wifi_password);
            delay(5);
            wifi_flag = 1;
          }
          if (bufindex >= 128)
          {
            bufindex = 0;
          }
        }
        if (wifi_flag == 1)
        {
          WiFi.disconnect();
          delay(100);
          WiFi.begin(info[0], info[1]);
          WiFi.setAutoReconnect(true);
          delay(100);
          Serial.println("begining");
          while (WiFi.status() != WL_CONNECTED)
          {
            delay(100);
            CloseData = Serial.read();
            if (CloseData == 'I')  //Exit
            {
              memset(buf, 0, 100);
              bufindex = 0;
              break;
            }
            Serial.println("connecting");
          }
          Serial.println("WiFi is connected.");
          Serial.println("IP address: ");
          Serial.println(WiFi.localIP());
          //          lcd.fillScreen(TFT_BLACK);
          //          lcd.setCursor(90, 120, 4);
          //          lcd.printf("Wifi is connected");
          WiFi.disconnect();
          WiFi.mode(WIFI_OFF);
          wifi_flag = 2;
          memset(buf, 0, 100);
          bufindex = 0;
          delay(2500);
          ESP.restart();
        }
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          wifi_flag = 0;
          Close_Flag = 0;
          memset(buf, 0, 100);
          bufindex = 0;
          WiFi.disconnect();
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Exit Wifi function testing");
          break;
        }
      }
      break;


    case 'U':
      Close_Flag = 1;
      Serial.println("Open UART1");
      lcd.fillScreen(TFT_BLACK);
      lcd.setCursor(100, 120, 4);
      lcd.printf("UART1 testing");
      while (Close_Flag == 1)
      {
        if (Serial2.available())              // if date is comming from softwareserial port ==> data is comming from SoftSerial shield
        {
          while (Serial2.available())         // reading data into char array
          {
            buffer[count++] = Serial2.read();   // writing data into array
            if (count == 256)break;
            CloseData = Serial.read();
            if (CloseData == 'I')  //Exit
            {
              break;
            }
          }
          Serial.write(buffer, count);           // if no data transmission ends, write buffer to hardware serial port
          clearBufferArray();              // call clearBufferArray function to clear the storaged data from the array
          count = 0;                       // set counter of while loop to zero
        }
        if (Serial.available())            // if data is available on hardwareserial port ==> data is comming from PC or notebook
          Serial2.write(Serial.read());       // write it to the SoftSerial shield
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Close UART1");
          break;
        }
      }
      break;

    case 'P':
      Close_Flag = 1;
      Serial.println("Open SPEAK");
      lcd.fillScreen(TFT_BLACK);
      lcd.setCursor(100, 120, 4);
      lcd.printf("SPEAK testing");
      Sequence.AddPlayItem(&ForceWithYou);      // Add the first sound item, this will play first
      Sequence.AddPlayItem(&Music);             // Add the music score, this will play after the first item
      DacAudio.Play(&Sequence);                 // Play the sequence, will play just the once and then stop
      while (Close_Flag == 1)
      {
        DacAudio.FillBuffer();//play
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          DacAudio.StopAllSounds();
          delay(50);
          Close_Flag = 0;
          //          digitalWrite(25, LOW);
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Close SPEAK");
          break;
        }
      }
      break;

    case 'V':
      Close_Flag = 1;
      Serial.println("Bluetooth function");
      pAdvertising->start();  //Start broadcasting
      pService->start();
      lcd.fillScreen(TFT_BLACK);
      while (Close_Flag == 1)
      {
        if (connected_state == true) //equipped with a connection
        {
          lcd.fillScreen(TFT_BLACK);
          lcd.setCursor(20, 120, 4);
          lcd.printf("Bluetooth is connected");
        }
        else
        {
          lcd.fillScreen(TFT_BLACK);
          lcd.setCursor(20, 120, 4);
          lcd.printf("Bluetooth is not connected");
        }
        delay(100);
        CloseData = Serial.read();
        if (CloseData == 'I')  //Exit
        {
          Close_Flag = 0;
          pAdvertising->stop();  //stop broadcasting
          pService->stop();
          delay(50);
          lcd.fillScreen(TFT_BLACK);
          Serial.println("Turn off Bluetooth function");
          break;
        }
      }
      break;

    //Exit Software Test Program
    case 'C':
      NO_Test_Flag = 0;
      Test_Flag = 0;
      Wire.end();
      lcd.fillScreen(TFT_BLACK);
      Serial.println("Exit the software test program");
      delay(500);
      break;
  }
}


//SD Cardinitialization
int SD_init()
{

  if (!SD.begin(SD_CS))
  {
    Serial.println("Card Mount Failed");
    return 1;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No TF card attached");
    return 1;
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("TF Card Size: %lluMB\n", cardSize);
  listDir(SD, "/", 2);

  //  listDir(SD, "/", 0);
  //  createDir(SD, "/mydir");
  //  listDir(SD, "/", 0);
  //  removeDir(SD, "/mydir");
  //  listDir(SD, "/", 2);
  //  writeFile(SD, "/hello.txt", "Hello ");
  //  appendFile(SD, "/hello.txt", "World!\n");
  //  readFile(SD, "/hello.txt");
  //  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  //  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
  //  Serial.println("SD init over.");

  return 0;
}

//Traversing the SD Card Catalog
void listDir(fs::FS & fs, const char *dirname, uint8_t levels)
{
  //  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    //Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  i = 0;
  while (file)
  {
    if (file.isDirectory())
    {
      //      Serial.print("  DIR : ");
      //      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("FILE: ");
      Serial.print(file.name());
      lcd.setCursor(0, 2 * i);
      lcd.printf("FILE:%s", file.name());

      Serial.print("SIZE: ");
      Serial.println(file.size());
      lcd.setCursor(180, 2 * i);
      lcd.printf("SIZE:%d", file.size());
      i += 16;
    }

    file = root.openNextFile();
  }
}

void touch_calibrate()//screen calibration
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;
  Serial.println("screen calibration");

  //calibration
  //  lcd.fillScreen(TFT_BLACK);
  //  lcd.setCursor(20, 0);
  //  Serial.println("setCursor");
  //  lcd.setTextFont(2);
  //  Serial.println("setTextFont");
  //  lcd.setTextSize(1);
  //  Serial.println("setTextSize");
  //  lcd.setTextColor(TFT_WHITE, TFT_BLACK);

  //  lcd.println("Touch corners as directed");
  Serial.println("Touch corners as directed");
  //  lcd.setTextFont(1);
  //  lcd.println();
  //  Serial.println("setTextFont(1)");
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
  //  lcd.fillScreen(TFT_BLACK);
  //
  //  lcd.setTextColor(TFT_GREEN, TFT_BLACK);
  //  lcd.println("XZ OK!");
  //  lcd.println("Calibration code sent to Serial port.");

}
