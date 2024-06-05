# CrowPanel 7.0" Home Assistant Squareline Demo

## Prerequisites
* Select a new sketchbook directory
* Copy libraries from Elecrow website
* Install libraires
    * [async-mqtt-client](https://github.com/marvinroger/async-mqtt-client)
    * [AsyncTCP](https://github.com/dvarrel/AsyncTCP)
    * [Arduino_GFX](https://github.com/moononournation/Arduino_GFX)
    * [LovyanGFX](https://github.com/lovyan03/LovyanGFX)
    * [TAMC_GT911](https://github.com/tamctec/gt911-arduino)
* Copy file `libraries/lvgl/lv_conf_template.h` to the sketchbook libraries directory `./libraries/lv_conf.h`
* Copy the files named `ui*` from [this](/7.0/6%E3%80%81homeassistant/7.0-inch_Squareline_Demo) directory to the following child sub-folder in the sketchbook directory: ./libraries/UI

Note that different projects for ESP32 display require different UI libraries. So you need to replace the UI files when you using different displays.

## Build


# OLD


**If you are using this demo code for the first time, I suggest that you watch this video lesson first.**

video link:

[Design Like a Pro: Create and Export Stunning UI with SquareLine Studio for ESP32 Display](https://www.youtube.com/watch?v=TcWvxw61U_w)

------------------------------------------------------------------------------------------------------------------------------

![](https://raw.githubusercontent.com/Elecrow-RD/esp32-display/master/7.0/3%E3%80%81arduino/7.0-inch_Squareline_Demo/README/1.png)

![](https://raw.githubusercontent.com/Elecrow-RD/esp32-display/master/7.0/3%E3%80%81arduino/7.0-inch_Squareline_Demo/README/2.png)

Please notice the tips inside the demo code,that is very important.

**If you don't want to use the UI export from Squareline Studio ,please del the UI file, and open the demo program again, then you can compile the program, or your program may happen some errors.**

--------------------------------------------------------------------------------------------------------------------------

**And if you are using a 4.3-inch board, when configuring the compilation environment, you need to change "PSRAM" to "QSPI PSRAM" to ensure proper display. Go to Tools -> PSRAM -> QSPI PSRAM. For the 5.0-inch and 7.0-inch boards, follow the tutorial and select "OPI PSRAM" as instructed.**

![](https://raw.githubusercontent.com/Elecrow-RD/esp32-display/master/7.0/3%E3%80%81arduino/7.0-inch_Squareline_Demo/README/3.png)







-------------------------------------------------------------END----------------------------------------------------------
