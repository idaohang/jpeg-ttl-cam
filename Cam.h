/*
  Copyright (C)2011 Kostas Tamateas <nosebleedKT@gmail.com> 
 
  This program is distributed under the terms of the GNU 
  General Public License, version 2. You may use, modify, 
  and redistribute it under the terms of this license. 
  A copy should be included with this source. 
  
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
    void SendGetFileSizeCmd();
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
