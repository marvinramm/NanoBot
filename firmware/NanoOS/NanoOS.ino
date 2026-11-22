/*
 * NanoOS 1.15 - firmware for the NanoBot (RoboCup Junior Rescue Line)
 * https://github.com/marvinramm/NanoBot
 *
 * Copyright (c) 2022-2023 Marvin Ramm
 * SPDX-License-Identifier: MIT
 *
 * Arduino IDE settings (ESP32 Arduino core 2.0.x):
 *   Board:            "ESP32 Dev Module"
 *   Upload Speed:     "921600"
 *   CPU Frequency:    "240MHz (WiFi/BT)"
 *   Flash Frequency:  "80MHz"
 *   Flash Mode:       "QIO"
 *   Flash Size:       "16MB (128Mb)" (4MB if the W25Q32 flash is fitted)
 *   Partition Scheme: "Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)"
 *
 * Open points from the last competition season:
 *   - test the speed-bump handling ("Wumper") and the remaining cases
 *   - intersections are not always detected correctly
 *   - reset the robot with a button press (contest mode only), e.g. when it
 *     is stuck in a loop in the evacuation zone
 *   - cover all cases when driving around the obstacle
 *   - store the gyro calibration
 *   - menu item to shut the robot down, standby mode in the start menu
 *   - store MIN/MAX of the outer light sensors and verify the calibration
 *   - show a USB symbol in debug mode
 *   - rename the abbreviations of the calibration values
 */

///////////////////Includes//////////////////

#include "Bitmap.h"
#include "Display.h"
#include "ESP32_WiFi.h"
#include "Functions.h"
#include "Measure.h"
#include "Pins.h"
#include "Pitches.h"
#include "Settings.h"
#include "Tests.h"
#include "Tiles.h"

//////////////////Variables//////////////////


/////////////////////////////////////////////

////////////////////Setup////////////////////

void setup() {
  HardwareInit();

  //MotorTest();

  //Display Menu//

  if (CONTESTMODE == 0) {
    while (ENMainProgram == 0) {
      ReadEncoder();

      if (ENCCounter > 8) {
        ENCCounter = 8;
      }

      if (ENCCounter == 8) {
        MenuPage = 11;
      } else {
        MenuPage = ENCCounter;
      }

      MenuActions();
      OTALOOP();
    }
  } else {
    ENMainProgram = 1;
  }

  /*

  if (CalState == false) {
    TilesCase = 11;
    ForceDispUpdate = true;

    RGBOff();
    SetRGB(2,B);
    delay(3500);
    if (ENCButtonState == 1) {
      ENCButtonState = 0;
    } else {
      Calibrate(2500);
      PlayTone(0);
      delay(3000);
    }
    RGBOff();
  }

  */

  ShiftRegisterWrite(SR_LED_RGB, HIGH);
  ShiftRegisterWrite(SR_LED_PT, HIGH);
  VL53l1xInit(3, 2);
  delay(1500);
  RGBOff();
}

/////////////////////////////////////////////

////////////////////Loop/////////////////////

void loop() {
  if (DEBUGMODE == 1) {

    MeasureLight();
    PrintDataSerial(0);
    Serial.print("Core0 time:");
    Serial.print(Core0Time);
    Serial.println("ms");
    FollowLine(Normal);

  } else {

    if (!DEBUGRESCUE) {

      LoopState = 1;

      MeasureLight();

      CaseDistinction();

      if (WumperTime > millis()){
        RGBOff();
        SetRGB(2,R);
        TilesCase = 14;

        FollowLine(Wumper);
      }else{
        if (XAngle > 15) {  //Ramp up
        TilesCase = 9;
        RGBOff();
        SetRGB(2,G);
        SetRGB(2,B);

        FollowLine(Up);
      } else if (XAngle < -15) {  //Ramp down
        TilesCase = 10;
        RGBOff();
        SetRGB(2,R);
        SetRGB(2,B);

        FollowLine(Down);
      } else {  //Flat ground
        TilesCase = 0;
        RGBOff();
        SetRGB(2,B);

        FollowLine(Normal);
      }
      }

    } else {
      FindExit();
    }
  }
}

/////////////////////////////////////////////