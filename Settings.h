#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define Revision "C04.05.11"

#define LOG_ENABLE 0
#define DEBUG_ENABLE 0
#define DEBUG Serial1

/*
  Watchdog Timeout value
*/
#define WDT_TIMEOUT  8

/*
  Quick, low power, non repeated transmissions of aprs status messages 
  for a couple of hours won't kill the aprs network.
*/
#define STATUS_PERIOD   32000  
#define STATUS_SIZE     71 + sizeof(SIGNATURE)-1

#define POSITION_PERIOD   64000
#define POSITION_SIZE     44 + sizeof(SIGNATURE)-1

/*
  AX.25 addresses, tx delay and aprs signature
*/
#define S_CALLSIGN      "SW2HYX"  // your own callsign
#define S_CALLSIGN_ID   11        // ... and SSID (-11 = balloons, -9 = cars, -8 = boats)
#define D_CALLSIGN      "APZIKA"  // destination callsign
#define D_CALLSIGN_ID   0         // ... and SSID (-0 = home station) 
#define D_CALLSIGN2     "SW2HYX"  // destination callsign
#define D_CALLSIGN_ID2  8         // ... and SSID (-8 = boats) 
#define DIGI_PATH1      "WIDE2"   // second digipeater (leave undefined if none)
#define DIGI_PATH1_TTL  1         // ... and time-to-live
#define SIGNATURE       "SLAROS"  // this goes in the APRS comment field
#define TX_DELAY        300 

// ATMega644p pins
#define SDChipSelect 3   // PB3 
#define CS           4   // PB4
#define MOSI         5   // PB5
#define MISO         6   // PB6
#define SCK          7   // PB7
#define BLUE         12  // PD4
#define GREEN        13  // PD5
#define RED          14  // PD6
#define RADIO_TX     15  // PD7
#define RADIO_PTT    21  // PC5
#define HUMID_PIN    25  // PA0
#define TEMP_PIN     29  // PA2

// Misc
#define NumOfThermometers 2

#endif
