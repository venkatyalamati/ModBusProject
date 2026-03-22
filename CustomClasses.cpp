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

// ------------------- MFT_Data_Handle class implementations ------------
  MFT_Data_Handle::MFT_Data_Handle(float pfLimit){
    this->pfLimit = pfLimit; // initializing with user input value
    this->pfRange = PF_Ranges::unitypf; // initializing with default value
  }
  void MFT_Data_Handle::identify_pfRange(){ // works on this convention -> pf is always positive float value 

    pf = fabs(pf);

    if(kVAr > 0 && pf <= pfLimit && pf != 0.0){ // extream lagging
      pfRange = PF_Ranges::xtrmLagg;
    }
    else if(kVAr < 0 && pf <= pfLimit && pf != 0.0){ // extream leading
      pfRange = PF_Ranges::xtrmLead;
    }
    else if(kVAr > 0 && pf > pfLimit && pf < 0.995){ // healthy lagging
      pfRange = PF_Ranges::lagging;
    }
    else if(kVAr < 0 && pf > pfLimit && pf < 0.995){ // healthy leading
      pfRange = PF_Ranges::leading;
    }
    else if(pf >= 0.995 && pf <= 1.0){
      pfRange = PF_Ranges::unitypf;
    }
    else{
      pfRange = PF_Ranges::invalpf;
    }
  }  

// ------------------- ModBusComHandle class implementations ------------
ModBusComHandle::ModBusComHandle(uint8_t SLAVE_ID, uint8_t MAX485_RO, uint8_t MAX485_DI) : Max485_Serial(MAX485_RO, MAX485_DI)
{
  _SLAVE_ID = SLAVE_ID;
}
void ModBusComHandle::preTransmission() {
  digitalWrite(MAX485_DE_RE, HIGH);
  delayMicroseconds(100); // Ensure stable switching
}

void ModBusComHandle::postTransmission() {
  delayMicroseconds(100); // Ensure stable switching
  digitalWrite(MAX485_DE_RE, LOW);
}

void ModBusComHandle::begin(){
  pinMode(MAX485_DE_RE, OUTPUT);
  digitalWrite(MAX485_DE_RE, LOW); // Initiate to receive

  Max485_Serial.begin(SLAVE_BAUD_RATE); // SERIAL_8E1 -> 8 data bits, Even parity, 1 stop bit

  node.begin(_SLAVE_ID, Max485_Serial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

float ModBusComHandle::getRegisterValue(uint16_t regAddr){
  uint16_t data[2];  // Buffer for two input registers (for 32-bit float)
  float readVal; uint32_t combinedVal;
  regAddr = regAddr - 30001;
  // Read 2 input registers (function code 04)
  modBusComResult = node.readInputRegisters(regAddr, 2); // Read two registers for ex: 30001 and 30002
  //modBusComResult = node.readHoldingRegisters(regAddr, 1);
  if (modBusComResult == node.ku8MBSuccess) {
    
    data[0] = node.getResponseBuffer(0); // First register (e.g., 30001)
    data[1] = node.getResponseBuffer(1); // Second register (e.g., 30002)
    
    // Combine registers into 32-bit float (assuming high-word first)
    combinedVal = ((uint32_t)data[0] << 16) | data[1];
    // combinedVal = ((uint32_t)data[1] << 16) | data[0];
    // combinedVal = ((uint32_t)(data[1] & 0xFF) << 24) | ((uint32_t)(data[1] >> 8) << 16) |
    // ((uint32_t)(data[0] & 0xFF) << 8) | ((uint32_t)(data[0] >> 8));
    
    memcpy(&readVal, &combinedVal, sizeof(readVal)); // Convert to float
    modBusReadSuccess = true; // Read successful
    return readVal;
  }
  else{
    modBusReadSuccess = false;
    return 0;
    //Serial.print("Modbus Error: 0x"); Serial.println(modBusComResult, HEX);
  } 
}

// TFTDisplay class implementations
  TFTDisplay::TFTDisplay(uint8_t cdPin, uint8_t rstPin) : ucg(cdPin, -1, rstPin)
  {
    //constructor — wires up ucg pins -> forwards your pins into ucg's constructor
    //CS -> GND, Pin 12(MISO) cannot be used for any purpose
  }
  void TFTDisplay::begin(){
    ucg.begin(UCG_FONT_MODE_SOLID);
    ucg.setRotate270();
    ucg.setColor(1, 0, 0, 0);        // Background color: Black (R,G,B)
    ucg.setColor(0, 255, 255, 255);  // Text color: White (R,G,B)
  }
  void TFTDisplay::clearScreen(){
    ucg.clearScreen();
  }
  void TFTDisplay::setSmallFont(){
    ucg.setFont(ucg_font_9x18_tf); // " " will not overwrite
  }
  void TFTDisplay::setBigFont(){
    ucg.setFont(ucg_font_inr21_mr); // " " will overwrite
  }
  void TFTDisplay::textPrintSmallFont(float linePos){
    ucg.setPrintPos(9, linePos*20);
    ucg.print(charBuffer); // will stop at the first '\0' and ignore everything after it.
  }
  void TFTDisplay::textPrintBigFont(float linePos){
    ucg.setPrintPos(9, linePos*30);
    ucg.print(charBuffer); // will stop at the first '\0' and ignore everything after it.
  }
  void TFTDisplay::clearCharBuffer(){
    for(uint8_t i=0; i<CHAR_BUFF_SIZE; i++)
      charBuffer[i] = '\0';
  }
  
  