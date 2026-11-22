/*
 * NanoOS 0.80 - last firmware for NanoBot REV1 (RoboCup Junior Rescue Line)
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
 * Open points at the time:
 *   - show a USB symbol in debug mode
 *   - wheel guards
 *   - reset the program with a button press, e.g. when stuck in a loop in
 *     the evacuation zone
 *   - adjust the motor speed to the battery voltage
 *   - contest mode is of little use because the calibration values are lost
 *     after an upload
 *   - crossing with a green marker: if L_M is still black it is a crossing
 *     with green, otherwise a right turn with green
 *   - update the tiles display only on force update (fewer display updates,
 *     more time for measuring)
 *   - drive past a T-junction tile
 *   - compare several green and red data sets
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

  if (CalState == false) {
    TilesCase = 11;
    ForceDispUpdate = true;

    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);
    delay(3500);
    if (ENCButtonState == 1) {
      ENCButtonState = 0;
    } else {
      Calibrate(2500);
      PlayTone(0);
      delay(3000);
    }
    digitalWrite(STAT_LED_BLUE, HIGH);
  }

  ShiftRegisterWrite(SR_LED_RGB, HIGH);
  ShiftRegisterWrite(SR_LED_PT, HIGH);
}

/////////////////////////////////////////////

////////////////////Loop/////////////////////

void loop() {
  if (DEBUGMODE == 1) {

    MeasureLight();
    PrintDataSerial(0);

  } else {

    if (!DEBUGRESCUE) {

      LoopState = 1;

      MeasureLight();

      CaseDistinction();

      if (XAngle > 18) {  //Ramp up
        TilesCase = 9;
        digitalWrite(STAT_LED_RED, LOW);
        digitalWrite(STAT_LED_GREEN, LOW);
        digitalWrite(STAT_LED_BLUE, HIGH);
        FollowLine(Up);
      } else if (XAngle < -18) {  //Ramp down
        TilesCase = 10;
        digitalWrite(STAT_LED_RED, HIGH);
        digitalWrite(STAT_LED_GREEN, LOW);
        digitalWrite(STAT_LED_BLUE, LOW);
        FollowLine(Down);
      } else {  //Flat ground
        TilesCase = 0;
        digitalWrite(STAT_LED_RED, LOW);
        digitalWrite(STAT_LED_GREEN, HIGH);
        digitalWrite(STAT_LED_BLUE, LOW);
        FollowLine(Normal);
      }

    } else {
      FindExit();
    }
  }
}

/////////////////////////////////////////////