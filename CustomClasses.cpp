#include "CustomClasses.h"

  CapacitorBank::CapacitorBank(uint8_t p3, uint8_t p2, uint8_t p1, uint8_t p0){
    capBankCtrlPins[3] = p3;
    capBankCtrlPins[2] = p2;
    capBankCtrlPins[1] = p1;
    capBankCtrlPins[0] = p0;
  }

  void CapacitorBank::doInitialSetup(){
    for(uint8_t i=0; i<NUM_CB_BITS; i++){
      pinMode(capBankCtrlPins[i], OUTPUT);
      EEPROM.get(2*i, switchingCnt[i]); // load the previous counter values from EEPROM
      capBankStatusCurr[i] = 0;
    }
    statusReprByte = 0;
  }

  void CapacitorBank::cap_kVAr_OneStep(StepDirections stepDrctn){
    if(stepDrctn == StepDirections::increase){
      if(statusReprByte < 15)
        statusReprByte++;
    }
    else if(stepDrctn == StepDirections::decrease){
      if(statusReprByte > 0)
        statusReprByte--;
    }
    switchCapBanks();
  }

  void CapacitorBank::switchCapBanks(){ // switch as per the status of capBankStatusBitRepr[]
  uint8_t tempByteVar;
    for(uint8_t i=0; i<NUM_CB_BITS; i++){
      capBankStatusPrev[i] = capBankStatusCurr[i];
    }
        
    tempByteVar = statusReprByte;
    // for example if statusReprByte == 7 then, capBankStatusCurr[3]=0, capBankStatusCurr[2]=1, capBankStatusCurr[1]=1, capBankStatusCurr[0]=1
    for(uint8_t i=0; i<NUM_CB_BITS; i++){
      capBankStatusCurr[i] = (tempByteVar >> i) & 0x01;
    }

    for(uint8_t i=0; i<NUM_CB_BITS; i++){
      if(capBankStatusPrev[i] != capBankStatusCurr[i]){
        switchingCnt[i]++;
        EEPROM.put(i*2, switchingCnt[i]);
        delay(10);
      }
    }

    for(uint8_t i=0; i<NUM_CB_BITS; i++){
      digitalWrite(capBankCtrlPins[i], capBankStatusCurr[i]);
    }

  }

  ModBusVariables::ModBusVariables(float pfLimit){
    this->pfLimit = pfLimit;
    this->pfRange = PF_Ranges::unitypf;
  }

  void ModBusVariables::identify_pfRange(){ // works on this convention -> pf is always positive float value 

    pf = fabs(pf);

    if(kVAr > 0 && pf <= pfLimit){ // extream lagging
      pfRange = PF_Ranges::xtrmLagg;
    }
    else if(kVAr < 0 && pf <= pfLimit){ // extream leading
      pfRange = PF_Ranges::xtrmLead;
    }
    else if(kVAr > 0 && pf > pfLimit){ // healthy lagging
      pfRange = PF_Ranges::lagging;
    }
    else if(kVAr < 0 && pf > pfLimit){ // healthy leading
      pfRange = PF_Ranges::leading;
    }
    else{
      pfRange = PF_Ranges::unitypf;
    }
  }  

  