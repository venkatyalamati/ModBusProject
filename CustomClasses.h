#pragma once
#include <Arduino.h>
#include <EEPROM.h>
// ----------------------------------- Capacitor Bank Related Class ----------------------------
const uint8_t NUM_CB_BITS = 4;
enum class StepDirections : uint8_t {increase, decrease};
class CapacitorBank{
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
// ------------------------------------ MFT Parameters Handle Class ---------------------------
enum class PF_Ranges : uint8_t {unitypf, lagging, leading, xtrmLagg, xtrmLead};
class ModBusVariables{
  public:
  float pf; float kVAr; float pfLimit;
  PF_Ranges pfRange;
  ModBusVariables(float _pfLimit);
  void identify_pfRange();
};