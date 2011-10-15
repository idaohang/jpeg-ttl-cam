/*
  Copyright (C)2011 Kostas Tamateas <nosebleedKT@gmail.com> 
 
  This program is distributed under the terms of the GNU 
  General Public License, version 2. You may use, modify, 
  and redistribute it under the terms of this license. 
  A copy should be included with this source. 
  
  This is an arduino compatible code. 
  In brief it does the following:
  Speaks the LinkSprite JPEG TTL Camera protocol
  Saves the image data on a file on a microSD
  Injects flight data into the image 
  
  Note A: To get max serial baudrate (115200) use a 7.3Mhz crystal
  Note B: Input voltage 3v3
  Note C: Picture names only in 7.3 format
  Note D: Up to 9999 different names can be saved. Then it will roll back to 0 and overwrite. 
          EEPROM's 1st and 2nd byte are used to keep the last picture ID so in case of reboot
          the naming continues from where it stopped and not by 0.
  Note E: When reprogramming the avr all eeprom contents are overwritten with 0xFF.

  
  Example sketch
  -----------------
  #include <Fat16.h>
  #include <EEPROM.h>
  
  void setup()
  {  
    camera.setup();
  }
  void loop()
  {
    camera.shoot(gps.Time, gps.Latitude, gps.Longitude, gps.Altitude);
  }  
  -----------------
*/

#ifndef __CAM_H__
#define __CAM_H__

#include <WProgram.h>
#include <Fat16.h>
#include <stdio.h>
#include <EEPROM.h>
#include "Settings.h"

class CAM
{
  public:
    CAM();
    boolean setup();
    void shoot(char *, char *, char *, char *);
  
  private:  
    void SendTakePhotoCmd();
    void SendResetCmd();
    void SendReadDataCmd();
    void StopTakePhotoCmd();
    void SetImageSize();
    void SetCompressRatio();
    void EnterPowerSave();
    void ExitPowerSave(); 
    void SetBaudRate(long int baudrate);
    
    byte b;
    boolean jpegEnd;
    unsigned int i, j, count;  
    long int curAddr;
    byte MH, ML;           // Starting Address
    byte LH, LL;           // Length High, Length Low --> 80.
    byte chunk[80];
    unsigned int chunkSize;
    byte DH, DL;           // Delay High, Delay Low
    byte readDelay;   
    unsigned int PICid;
    byte healthFlag;
};

#endif
