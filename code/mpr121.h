#ifndef MPR121_H
#define MPR121_H

# include "IOPort.h"

class MPR121 
{
public:
   MPR121()
   {
      m_addr = ADDRESS_0;
      m_keyStatus = 0;
      m_CDC = 15;
      m_FFI = 0;
      m_CDT = 1;
      m_SFI = 0;
      m_ESI = 4;
      m_TTH = 0x3f;
      m_RTH = 0x2a;
   }

   void setTTH(uint8_t tth)
   {
         m_TTH = tth;
   }

   void setRTH(uint8_t rth)
   {
         m_RTH = rth;
   }

   void enableInterupt()
   {
      /***********************************************/
      /*   Set  the  port  as in Input then turn on  */
      /*   the internal pull up resistor             */
      /***********************************************/
      m_irqPort.SetPort(IOPort::PORT_D, 0x08);
      m_irqPort.on();
   }

   void initialize()
   {
     // Section A
     // This group controls filtering when data is > baseline.
      Wire.begin();
      Wire.enable();

      write(SRR, SOFT_RESET);

      write(MHD_R, 0x01);
      write(NHD_R, 0x01);
      write(NCL_R, 0x00);
      write(FDL_R, 0x00);

      // Section B
      // This group controls filtering when data is < baseline.
      write(MHD_F, 0x01);
      write(NHD_F, 0x01);
      write(NCL_F, 0xFF);
      write(FDL_F, 0x02);

      // Section C
      // This group sets touch and release thresholds for each electrode
      write(ELE0_T, m_TTH);
      write(ELE0_R, m_RTH);
      write(ELE1_T, m_TTH);
      write(ELE1_R, m_RTH);
      write(ELE2_T, m_TTH);
      write(ELE2_R, m_RTH);
      write(ELE3_T, m_TTH);
      write(ELE3_R, m_RTH);
      write(ELE4_T, m_TTH);
      write(ELE4_R, m_RTH);
      write(ELE5_T, m_TTH);
      write(ELE5_R, m_RTH);

      write(ELE6_T, m_TTH);
      write(ELE6_R, m_RTH);
      write(ELE7_T, m_TTH);
      write(ELE7_R, m_RTH);
      write(ELE8_T, m_TTH);
      write(ELE8_R, m_RTH);
      write(ELE9_T, m_TTH);
      write(ELE9_R, m_RTH);
      write(ELE10_T, m_TTH);
      write(ELE10_R, m_RTH);
      write(ELE11_T, m_TTH);
      write(ELE11_R, m_RTH);

      // Section D
      // Set the Filter Configuration
      // Set ESI2
//      write(AFE_2, 0x04);
//      write(AFE_2, 0x24);
      write(AFE_1, m_FFI << 6 | m_CDC << 5);
      write(AFE_2, m_CDT << 5 | m_SFI << 3 | m_ESI << 0);

      // Section E
      // Electrode Configuration
      // Enable 6 Electrodes and set to run mode
      // Set ELE_CFG to 0x00 to return to standby mode
      // write(ELE_CFG, 0x0C);   // Enables all 12 Electrodes
      write(ELE_CFG, 0x0c);      // Enable first 6 electrodes

      // Section F
      // Enable Auto Config and auto Reconfig
/*
      write(ATO_CFG0, 0x0B);
      write(ATO_CFGU, 0xC9);     // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
      write(ATO_CFGL, 0x82);     // LSL = 0.65*USL = 0x82 @3.3V
      write(ATO_CFGT, 0xB5);     // Target = 0.9*USL = 0xB5 @3.3V
*/
      Wire.disable();
   }

   void write(unsigned char address, unsigned char data)
   {
      Wire.beginTransmission(m_addr);  // begin communication with the MPR121 on I2C address 0x5A
      Wire.send(address);            // sets the register pointer
      Wire.send(data);               // sends data to be stored
      Wire.endTransmission();        // ends communication
   }

   uint8_t haveChange()
   {
//      Serial << "mpr121::haveChange(): irq = " << (int)m_irqPort.read() << endl;
      return(! m_irqPort.read());
   }

   uint16_t readKeyboard()
   {
      char ibuf[32];

      if ( ! m_irqPort.read() ) 
      {
         Wire.begin();
         Wire.enable();
         Wire.beginTransmission(m_addr);
         Wire.send(TOUCH_STATUS);
         Wire.endTransmission();
         Wire.requestFrom(m_addr, (uint8_t)2);
         m_keyStatus = Wire.receive();
         m_keyStatus |= Wire.receive() << 8;
   /*
         Wire.beginTransmission(m_addr);
         Wire.send(TOUCH_STATUS);
         Wire.endTransmission();
         Wire.requestFrom(m_addr, (uint8_t)0x2b);
         for (int x = 0; x < 0x2b; x++) 
         {
            Serial << "addr = 0x" << itoa(x, ibuf, 16) << ", val = 0x" << itoa(Wire.receive(), ibuf, 16) << endl;
         }
   */
         Wire.disable();
      }
      return(m_keyStatus);
   }

protected:
   enum 
   {
      ADDRESS_0         = 0x5a,
      ADDRESS_1         = 0x5b,
      ADDRESS_2         = 0x5c,
      ADDRESS_3         = 0x5d,

      TOUCH_STATUS      = 0x00,
      PROXI_STATUS,
      TOUCH_OOR_STATUS,
      PROXI_OOR_STATUS,
      E0TTH             = 0x41,
      ECR               = 0x5e,
      SRR               = 0x80,

      MHD_R             = 0x2B,
      NHD_R             = 0x2C,
      NCL_R             = 0x2D,
      FDL_R             = 0x2E,
      MHD_F             = 0x2F,
      NHD_F             = 0x30,
      NCL_F             = 0x31,
      FDL_F             = 0x32,
      ELE0_T            = 0x41,
      ELE0_R            = 0x42,
      ELE1_T            = 0x43,
      ELE1_R            = 0x44,
      ELE2_T            = 0x45,
      ELE2_R            = 0x46,
      ELE3_T            = 0x47,
      ELE3_R            = 0x48,
      ELE4_T            = 0x49,
      ELE4_R            = 0x4A,
      ELE5_T            = 0x4B,
      ELE5_R            = 0x4C,
      ELE6_T            = 0x4D,
      ELE6_R            = 0x4E,
      ELE7_T            = 0x4F,
      ELE7_R            = 0x50,
      ELE8_T            = 0x51,
      ELE8_R            = 0x52,
      ELE9_T            = 0x53,
      ELE9_R            = 0x54,
      ELE10_T           = 0x55,
      ELE10_R           = 0x56,
      ELE11_T           = 0x57,
      ELE11_R           = 0x58,
      AFE_1             = 0x5C,
      AFE_2             = 0x5D,
      ELE_CFG           = 0x5E,
      GPIO_CTRL0        = 0x73,
      GPIO_CTRL1        = 0x74,
      GPIO_DATA         = 0x75,
      GPIO_DIR          = 0x76,
      GPIO_EN           = 0x77,
      GPIO_SET          = 0x78,
      GPIO_CLEAR        = 0x79,
      GPIO_TOGGLE       = 0x7A,
      ATO_CFG0          = 0x7B,
      ATO_CFGU          = 0x7D,
      ATO_CFGL          = 0x7E,
      ATO_CFGT          = 0x7F,

      TOU_THRESH        = 0x3F,
      REL_THRESH        = 0x2A,

      SOFT_RESET        = 0x63
   };

private:
   uint8_t     m_CDC;
   uint8_t     m_FFI;
   uint8_t     m_CDT;
   uint8_t     m_SFI;
   uint8_t     m_ESI;
   uint8_t     m_TTH;
   uint8_t     m_RTH;
   uint8_t     m_addr;
   uint16_t    m_keyStatus;
   InputPort   m_irqPort;
};

#endif
