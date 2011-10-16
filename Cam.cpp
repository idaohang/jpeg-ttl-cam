/*
  Copyright (C)2011 Kostas Tamateas <nosebleedKT@gmail.com> 
 
  This program is distributed under the terms of the GNU 
  General Public License, version 2. You may use, modify, 
  and redistribute it under the terms of this license. 
  A copy should be included with this source. 
*/

#include "Cam.h"

Fat16 pictureFile;
char PICname[] = "PIC0000.JPG";

CAM::CAM()
{
  LH = 0x00;
  LL = 0x50;
  chunkSize = 80;
  DH = 0x00;
  DL = 0x0A;
  readDelay = 25;  
}

boolean CAM::setup()
{ 
  // Get last picture id from eeprom
  byte a = EEPROM.read(0);
  byte b = EEPROM.read(1);   
  PICid = b << 8 | a & 0xFF;
  
  if(PICid > 9999)
  {
    PICid = 0;
    EEPROM.write(0, 0);
    EEPROM.write(1, 0);  
  }    
  
  Serial1.begin(38400);
  SetBaudRate(115200);
  SetImageSize();
  Reset();    
  return true; 
}

void CAM::shoot(char *time, char *lat, char *lon, char *alt)
{  
   snprintf(PICname,12,"PIC%04d.JPG",++PICid);
   
   if(DEBUG_ENABLE)
   {
     DEBUG.print("Shooting & Saving.. ");  
     DEBUG.print(PICname);
   }
          
   pictureFile.open(PICname, O_CREAT | O_WRITE);
   if (!pictureFile.isOpen()) 
   {
     if(DEBUG_ENABLE)
       DEBUG.println("..Picture file not created. Quit shooting!");
     return;
   }
   
   StartShooting(); 
      
   // Wait for Interval Time
   delay(readDelay);
  
   // Skip camera messages
   while(Serial1.available())
   {
     b = Serial1.read();
     if(DEBUG_ENABLE)
       DEBUG.print(b);
   }
   
   unsigned long startTime = millis();
   jpegEnd = false;
   curAddr = 0;  
   
   // Until we reach jpeg end, poll chunks, transfer chunks, write chunks
   while(!jpegEnd)
   {  
      j=0;
      count=0;
      
      // Very critical part. 
      // A 640x480 image transfered at max baudrate(115200) and written on microSD takes about 30sec.
      // Increase ImgTimeout if you operate at lower baudrates 
      if (millis() - startTime > ImgTimeout*1000) 
      { 
        if(DEBUG_ENABLE)
          DEBUG.println("Timed out"); 
        break;
      }
      
      // Request data
      ReadData();     

      // Wait for Interval Time
      delay(readDelay);
          
      // Read Request-data response
      byte k=0;
      while(Serial1.available()>0 && k<5)
      {
        b = Serial1.read();
        k++;
      }
      
      // Move jpeg chunk from camera's internal buffer to microSD 
      // Chunk size is always 80 byte
      while(Serial1.available())
      {
        b = Serial1.read();
        if((j<chunkSize)&&(!jpegEnd))
        {
           chunk[j] = b;
           pictureFile.write(b);
                      
           // Check whether we reached jpeg end
           if((chunk[j-1]==0xFF)&&(chunk[j]==0xD9))
           {
             StopShooting(); 
             jpegEnd = true; 
           }  
           j++;
           count++;
         }
      } 
	  
      /* Print chunk 
      for(j=0;j<count;j++)
      {   
        if(chunk[j]<0x10)
          DEBUG.printChar('0');
        DEBUG.printHEX(chunk[j]);
      }                  
      */ 
   }  

  // Inject gps data into comments field
  if(jpegEnd)    
  {
    // [FF D8]  [FF FE] [size ] [comments]  <-- Field Name
    // ---2---  ---2--- ---2--- ----34----  <-- Field capacity
    // 0     1  2    3  4    5  6       40  <-- Field start / end position in the jpeg header
    
    // FF D8 is already written
    
    // FF FE is already written
	
    // SIZE is 34 ( 0x00 0x24 )      
    pictureFile.seekSet(4);
    pictureFile.write((byte)0);
    pictureFile.write(0x24);

    // comments are 34 byte --> "@@@@,2059,xx.xxxx,yy.yyyy,zzzzz,@@@@"
    byte i;
    
    for(i=0; i<4; i++)
      pictureFile.write('@');
    
    for(i=0; i<4; i++)
      pictureFile.write(time[i]);
    pictureFile.write(',');
    
    for(i=0; i<7; i++)
      pictureFile.write(lat[i]);
    pictureFile.write(',');  
      
    for(i=0; i<7; i++)
      pictureFile.write(lon[i]);
    pictureFile.write(',');
    
    for(i=0; i<5; i++)
      pictureFile.write(alt[i]);
      
    for(i=0; i<4; i++)
      pictureFile.write('@');
  }  
              
  pictureFile.close();  
  
  // Save last picture id on eeprom   
  EEPROM.write(0, PICid & 0xFF);
  EEPROM.write(1, (PICid >> 8) &  0xFF);  
}

// Send get file size command
void CAM::GetFileSize()
{
      // Command [56 00 34 01 00]
   
      // Returns --> 76 00 34 00 04 00 00 XH XL
      // XH XL is the file length of JPEG file
   
      Serial1.write(0x56);
      Serial1.write(0x00);
      Serial1.write(0x34);
      Serial1.write(0x01);
      Serial1.write(0x00);
}

//Send Reset command
void CAM::Reset()
{
      Serial1.write(0x56);
      Serial1.write(0x00);
      Serial1.write(0x26);
      Serial1.write(0x00);  
}

//Send take picture command
void CAM::StartShooting()
{
      Serial1.write(0x56);
      Serial1.write(0x00);
      Serial1.write(0x36);
      Serial1.write(0x01);
      Serial1.write(0x00);  
}

//Read data
void CAM::ReadData()
{  
      MH = curAddr >> 8;
      ML = curAddr & 0xFF;
      Serial1.write(0x56);
      Serial1.write(0x00);
      Serial1.write(0x32);
      Serial1.write(0x0c);
      Serial1.write(0x00); 
      Serial1.write(0x0a);
      Serial1.write(0x00);
      Serial1.write(0x00);
      Serial1.write(MH);
      Serial1.write(ML);   
      Serial1.write(0x00);
      Serial1.write(0x00);
      Serial1.write(LH);
      Serial1.write(LL);
      Serial1.write(DH);  
      Serial1.write(DL);
      curAddr += chunkSize; // address increases according to chunk size
}

void CAM::StopShooting()
{
    Serial1.write(0x56);
    Serial1.write(0x00);
    Serial1.write(0x36);
    Serial1.write(0x01);
    Serial1.write(0x03);        
}

void CAM::SetImageSize()
{
   // Command + Reset [56 00 26 00]
   
   // 56 00 31 05 04 01 00 19 00 --> VGA=640x480
   // 56 00 31 05 04 01 00 19 11 --> QVGA=320x240
   // 56 00 31 05 04 01 00 19 22 --> QQVGA=160x120
   
   // Returns --> 76 00 31 00 00
   
   Serial1.write(0x56);
   Serial1.write(0x00);
   Serial1.write(0x31);
   Serial1.write(0x05);
   Serial1.write(0x04);
   Serial1.write(0x01);
   Serial1.write(0x00);
   Serial1.write(0x19);
   Serial1.write(0x00);
}

void CAM::SetCompressRatio()
{
  // Command + Ratio (last hex number 00 to FF)
  // 56 0 31 05 01 01 12 04 36 --> 36%
  
  // Returns --> 76 00 31 00 00 36
  
   Serial1.write(0x56);
   Serial1.write(0x00);
   Serial1.write(0x31);
   Serial1.write(0x05);
   Serial1.write(0x01);
   Serial1.write(0x01);
   Serial1.write(0x12);
   Serial1.write(0x04);
   Serial1.write(0x36);     
}

void CAM::EnterPowerSave()
{
  // Command + Enable byte (last hex number)
  // 56 00 3E 03 00 01 01 --> enter power save
  
  // Returns --> 76 00 3E 00 00
}

void CAM::ExitPowerSave()
{
  // Command + Disable byte (last hex number)
  // 56 00 3E 03 00 01 00 --> exit power save
  
  // Returns --> 76 00 3E 00 00
}

void CAM::SetBaudRate(long int baudrate)
{  
  // Command + XX XX (two last hex numbers)
  // 56 00 24 03 01 1C 4C --> 57600 baud

  // AE C8 9600
  // 56 E4 19200
  // 2A F2 38400
  // 1C 4C 57600
  // 0D A6 115200
  
  // Returns --> 76 00 24 00 00
  
   Serial1.write(0x56);
   Serial1.write(0x00);
   Serial1.write(0x24);
   Serial1.write(0x03);
   Serial1.write(0x01);
   
   switch(baudrate)
   {
     case 9600:
       Serial1.write(0xAE);
       Serial1.write(0xC8);        
       break;
     case 19200:
       Serial1.write(0x56);
       Serial1.write(0xE4);          
       break;  
     case 38400:
       Serial1.write(0x2A);
       Serial1.write(0xF2);              
       break; 
     case 57600:
       Serial1.write(0x1C);
       Serial1.write(0x4C);        
       break;         
     case 115200:
       Serial1.write(0x0D);
       Serial1.write(0xA6);         
       break;         
     default:
       Serial1.write(0x2A);
       Serial1.write(0xF2);         
       break;      
   }
   
   Serial1.end();
   Serial1.begin(baudrate); 
}
