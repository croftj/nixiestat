#ifndef ESQUARE_H
#define ESQUARE_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <avr/io.h>
# include <avr/pgmspace.h>
# include "core/HardwareSerial.h"
# include "core/wiring.h"
# include "EEPROM.h"
# include "TempSensors.h"

extern char ibuf[];

namespace Settings
{
   enum 
   {
      Heat,
      Cool,
      CURRENT = 0,
      SETTING_1,
      SETTING_2,
      SETTING_3,
      SETTING_4,
      NUM_SETTINGS,
      MAX_WRITES = 99000,
      SIGNATURE_SIZE = 6,
      MAX_EESQUARE = 4 * 1024
   };
}; 


/**********************************************************************/
/*   This  is  the  minimum block of data to be able to search for a  */
/*   valid block of eesquare (if present).                            */
/*                                                                    */
/*   THE  MEMBERS  OF THIS STRUCTURE MUST MATCH THE FIRST MEMBERS OF  */
/*   THE ESquareSpace_t TYPE                                          */
/**********************************************************************/
typedef struct 
{
   char     signature[Settings::SIGNATURE_SIZE];
   uint32_t count;
   uint16_t nextBlock;
} MiniBlock_t;

typedef struct 
{
   char     signature[Settings::SIGNATURE_SIZE];
   uint32_t count;
   uint16_t nextBlock;
   uint32_t version;
   uint32_t resetCounter;
   float    coolTemps[Settings::NUM_SETTINGS];
   int16_t  coolTimes[Settings::NUM_SETTINGS];
   float    heatTemps[Settings::NUM_SETTINGS];
   int16_t  heatTimes[Settings::NUM_SETTINGS];
   uint8_t  currentMode;
   bool     autoMode;
   bool     vacationMode;
   float    heatHyst;
   float    coolHyst;
   uint8_t  heatMinCycleTime;
   uint8_t  coolMinCycleTime;

   int8_t   heatCycleSensor;
   int8_t   coolCycleSensor;
   int8_t   cycleSensor;
   int8_t   outsideSensor;
   int8_t   targetSensor;

   bool     heatPidEnabled;
   float    heatCycleMaximum;
   float    heatCycleMinimum;
   float    heatCycleHyst;
   float    heatOutsideCurve;
   float    heatOutsideAdj;

   bool     coolPidEnabled;
   float    coolCycleMaximum;
   float    coolCycleMinimum;
   float    coolCycleHyst;
   float    coolOutsideCurve;
   float    coolOutsideAdj;

   float    cycleTemp;
   float    outsideCurve;
   float    outsideAdj;
   float    outsideTemp;
   float    targetTemp;
   float    vacationTemp;

   float    heatKp;
   float    heatKi;
   float    heatKd;

   float    coolKp;
   float    coolKi;
   float    coolKd;
   uint8_t  coolCycleTime;

   uint8_t  maxCycleTime;

   uint8_t  cycleTime;
   bool     pidEnabled;
   float    cycleMaximum;
   float    cycleMinimum;
   float    cycleHyst;
   float    Kp;
   float    Ki;
   float    Kd;

   uint8_t  keyTouch;
   uint8_t  keyRelease;
//   uint8_t  sensorAddr[NUM_SENSORS][8];

   uint32_t logTime;
} ESquareSpace_t;

class ESquare 
{
public:
   ESquare()
   {
   }

   void DisplaySettings();

   void write()
   {
      uint16_t ee_offset = GetEEStart();
//      Serial << __PRETTY_FUNCTION__ << "found block @ " << (int)ee_offset <<endl;
      ESquareSpace_t *ep = &(this->data);
//      Serial << __PRETTY_FUNCTION__ << "ep = " << (int)(ep) <<endl;
      uint8_t *cp;
//      Serial << __PRETTY_FUNCTION__ << "data count = " << (int)data.count <<endl;
      data.count++;
//      if ( false && data.count > Settings::MAX_WRITES ) 
//      {
//         Serial << __PRETTY_FUNCTION__ << "To many writes, marking block as bad " << (int)data.count <<endl;
//         cp = (uint8_t *)BAD_SIGNATURE;
//         for (int x = ee_offset; x < ee_offset+Settings::SIGNATURE_SIZE; x++)
//         {
//            EEPROM.write(x, *(cp++));
//         }
//
//         Serial << __PRETTY_FUNCTION__ << "moving esquare data, size = " << (int)sizeof(ESquare) <<endl;
//         cp = (uint8_t*)(ep);
//         for (int x = ee_offset; x < ee_offset+sizeof(ESquare); x++)
//            EEPROM.write(x, *(cp++));
//      }
//      else 
//      {
//         Serial << __PRETTY_FUNCTION__ << "Writing esquare data, size = " << (int)sizeof(ESquare) << " to offset: " << (int)ee_offset <<endl;
         cp = (uint8_t*)(ep);
         for (int x = ee_offset; x < ee_offset + sizeof(ESquare); x++)
         {
//            Serial << "Ofs: " << (int)x << " = 0x" << itoa(*cp, ibuf, 16) << endl;
            EEPROM.write(x, *(cp++));
         }
//         Serial << "\n";

//         Serial << __PRETTY_FUNCTION__ << "Signature = " << endl;
//         for (int x = ee_offset; x < ee_offset + Settings::SIGNATURE_SIZE; x++) 
//         {
//            Serial << "Ofs: " << (int)x << " = 0x" << itoa(EEPROM.read(x), ibuf, 16) << endl;
//         }
//         Serial << "\n";
//      }
//      Serial << __PRETTY_FUNCTION__ << "exit()" << endl;
   }

   void read()
   {
      uint16_t ee_offset = GetEEStart();
      ESquareSpace_t *ep = &this->data;
      Serial << __PRETTY_FUNCTION__ << "ep = " << (int)(ep) <<endl;
      uint8_t *cp = (uint8_t*)(ep);
      Serial << __PRETTY_FUNCTION__ 
         << "Loading esquare: " << (int)sizeof(ESquareSpace_t)
         << " bytes to RAM @: 0x" << itoa((int)cp, ibuf, 16) << endl;
      delay(500);
      for (int x = ee_offset; x < sizeof(ESquareSpace_t); x++)
      {
         *(cp++) = EEPROM.read(x);
      }
      Serial << __PRETTY_FUNCTION__ << "Loading esquare COMPLETE, signature = " << data.signature << endl;
      delay(500);
      if ( strcmp(data.signature, GOOD_SIGNATURE) != 0) 
      {
         Serial << __PRETTY_FUNCTION__ << "No Signature found!" << endl;
         data.count = 0;
         for (uint8_t x = 0; x < Settings::NUM_SETTINGS; x++) 
         {
            data.coolTemps[x] = 75;
         }
         for (uint8_t x = 0; x < Settings::NUM_SETTINGS; x++) 
         {
            data.heatTemps[x] = 67;
         }

         Serial << __PRETTY_FUNCTION__ 
            << "Presetting " << (int)sizeof(data.coolTimes)
            << "bytes of memory @: 0x" << itoa((int)data.coolTimes, ibuf, 16) << endl;

         memset(data.coolTimes, '\xff', sizeof(data.coolTimes));

         Serial << __PRETTY_FUNCTION__ 
            << "Presetting " << (int)sizeof(data.heatTimes)
            << "bytes of memory @: 0x" << itoa((int)data.heatTimes, ibuf, 16) << endl;

         memset(data.heatTimes, '\xff', sizeof(data.heatTimes));

//         Serial << __PRETTY_FUNCTION__ 
//            << "Presetting " << (int)sizeof(data.sensorAddr)
//            << "bytes of memory @: 0x" << itoa((int)data.sensorAddr, ibuf, 16) << endl;
//         memset(data.sensorAddr, '\xff', sizeof(data.sensorAddr));

         data.currentMode = Settings::Heat;
         data.heatHyst = 2;
         data.coolHyst = 2;
         strcpy(data.signature, GOOD_SIGNATURE);
         Serial << __PRETTY_FUNCTION__ << "Signature: |" << data.signature << "|" << endl;
         write();
      }
      else 
      {
         Serial << __PRETTY_FUNCTION__ << "Signature found!" << endl;
      }
   };

   ESquareSpace_t *settings()
   {
      return(&data);
   }

   uint16_t GetEEStart()
   {
      uint16_t rv = 0xffff;
      uint16_t ee_offset = 0;
      MiniBlock_t buf;
      MiniBlock_t *ep = &buf;
      uint8_t *cp = (uint8_t*)(ep);
      Serial << __PRETTY_FUNCTION__ << "Searching for start of esquare" << endl;
//      Serial << __PRETTY_FUNCTION__ << PSTR("Searching for start of esquare") << endl;
      delay(500);
      while (rv == 0xffff && ee_offset < Settings::MAX_EESQUARE) 
      {
         for (int x = ee_offset; x < sizeof(MiniBlock_t); x++)
         {
            *(cp++) = EEPROM.read(x);
         }
         if ( strcmp(data.signature, BAD_SIGNATURE) == 0) 
         {
            Serial << __PRETTY_FUNCTION__ << "Bad block, skipping bytes:" << (int)buf.nextBlock << endl;
//            Serial << __PRETTY_FUNCTION__ << PSTR("Bad block, skipping bytes:") << (int)buf.nextBlock << endl;
            ee_offset += buf.nextBlock;
         }
         else 
         {
            Serial << __PRETTY_FUNCTION__ << "Good or initial block found at " << ee_offset << endl;
//            Serial << __PRETTY_FUNCTION__ << PSTR("Good or initial block found at ") << ee_offset << endl;
            rv = ee_offset;
         }
      }
      return(rv);
   }
   
   char *version()
   {
      uint8_t* vp = (uint8_t*)&VERSION;
      memset(ver_buf, '\0', sizeof(ver_buf));
      char ibuf[8];
      for (uint8_t x = 0; x < 4; x++) 
      {
         itoa(*vp, ibuf, 10);
         strcat(ver_buf, ibuf);
         if ( x < 3 ) 
         {
            strcat(ver_buf, ".");
         }
      }
   }

   void printRevisionTag();

private:
   static const char       GOOD_SIGNATURE[Settings::SIGNATURE_SIZE];
   static const char       BAD_SIGNATURE[Settings::SIGNATURE_SIZE];
   static const uint32_t   VERSION;
   char                    ver_buf[17];
   ESquareSpace_t          data;
};

extern ESquare eeprom;

#endif
