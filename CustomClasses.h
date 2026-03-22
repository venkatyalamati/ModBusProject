#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <ModbusMaster.h>
#include <Ucglib.h>
#include <SPI.h>
// ----------------------------------- CapacitorBank Class ----------------------------
enum class StepDirections : uint8_t {increase, decrease};
class CapacitorBank{
private:
  static constexpr uint8_t NUM_CB_BITS = 4;
public:
  uint8_t statusReprByte; // holds any value from 0 to 15, for a 4-bit control
  uint8_t capBankCtrlPins[NUM_CB_BITS]; // lower index -> LSB Capacitor Ctrl Pin
  // each of the below array elements will be assigned with a byte value of either 1 or 0
  uint8_t capBankStatusCurr[NUM_CB_BITS];
  uint8_t capBankStatusPrev[NUM_CB_BITS];
  uint16_t switchingCnt[NUM_CB_BITS];
  
  CapacitorBank(uint8_t p3, uint8_t p2, uint8_t p1, uint8_t p0);
  void cap_kVAr_OneStep(StepDirections stepDrctn);
  void switchCapBanks();
  void doInitialSetup();
};
// ------------------------------------ MFT_Data_Handle Class ---------------------------
enum class PF_Ranges : uint8_t {unitypf, lagging, leading, xtrmLagg, xtrmLead, invalpf};
class MFT_Data_Handle{
  public:
  float pf; float kVAr; float pfLimit;
  PF_Ranges pfRange;
  MFT_Data_Handle(float pfLimit);
  void identify_pfRange();
};
// ------------------- ModBusComHandle class ------------
class ModBusComHandle{
private:
  uint8_t _SLAVE_ID;
  //Note:- SoftwareSerial library does NOT support parity. Hence, set the parity as None in the slave device
  SoftwareSerial Max485_Serial;
  ModbusMaster node;
  static constexpr uint32_t SLAVE_BAUD_RATE = 9600;
  static constexpr uint8_t MAX485_DE_RE = A2; // Cannot be made non-static. Define more for more objects of main class
  // MAX485_DE_RE -> DE and RE tied together
public:
  bool modBusReadSuccess;
  uint8_t modBusComResult;
  ModBusComHandle(uint8_t SLAVE_ID, uint8_t MAX485_RO, uint8_t MAX485_DI);
  // MAX485_RO -> Max485_Tx, for SoftwareSerial-> Rx
  // MAX485_DI -> Max485_Rx, for SoftwareSerial-> Tx
  void begin();
  float getRegisterValue(uint16_t regAddr);
  static void preTransmission(); // Cannot be made non-static. Define more for more objects of main class
  static void postTransmission(); // Cannot be made non-static. Define more for more objects of main class
};

class TFTDisplay{
  private:
    static constexpr uint8_t CHAR_BUFF_SIZE = 17;
    Ucglib_ST7735_18x128x160_HWSPI ucg;
  public:
    char charBuffer[CHAR_BUFF_SIZE];
    TFTDisplay(uint8_t cdPin, uint8_t rstPin);          // constructor — wires up ucg pins
    void begin();
    void clearScreen();
    void setSmallFont();
    void setBigFont();
    void textPrintSmallFont(float linePos);   // linePos 1,2,3...
    void textPrintBigFont(float linePos);
    void clearCharBuffer();
};