  //------------------------------------------Include Libraries Below-------------------------------------------------
  #include <EEPROM.h>
  #include <SPI.h>
  #include <Ucglib.h>
  #include "CustomClasses.h"
  #include "ModBusFunctions.h"

  //------------------------------------------Constant Macro Definitions------------------------------------
  constexpr float PF_LIMIT = 0.92f; // limit for both lagging & leading
  constexpr uint16_t ModBusRegAddr_pf = 30063;
  constexpr uint16_t ModBusRegAddr_kVAr = 30061;
  constexpr int addrEepromValidString = 500;
  //------------------------------------------Gloabl Variables Below-------------------------------------------------

  // Pins used from Bit-0 to Bit-6 on the PCB are: 2,3,4,5,6,7,8 respectively
  CapacitorBank objCapBank(2, 3, 4, 5); // Bit_3_CtrlPin, Bit_2_CtrlPin, Bit_1_CtrlPin, Bit_0_CtrlPin
  
  Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 9, /*cs=*/ -1, /*reset=*/ 10); //CS -> GND, Pin 12(MISO) cannot be used for any purpose

  ModBusVariables objModBusVar(PF_LIMIT);
 
  //------------------------------------------Main Setup Code Below-------------------------------------------------
  void setup() {
    //Serial.begin(9600);
    delay(1000);
    erase_eeprom_if_req(addrEepromValidString);
    initializeDisplay();
    objCapBank.doInitialSetup();
    initializeModBusSettings();

    //executeCtrlPinTestSwitching(2, 8);
   
  } // ---------------------------------------------- Main Setup ends ---------------------------------------------

  //------------------------------------------Main Loop Code Below-------------------------------------------------
  void loop(){

    objModBusVar.pf = getRegisterValue(ModBusRegAddr_pf);
    objModBusVar.kVAr = getRegisterValue(ModBusRegAddr_kVAr);
    objModBusVar.identify_pfRange();
    
    if(modBusReadSuccess){
        if(objModBusVar.pfRange == PF_Ranges::xtrmLagg){ // extream lagging
          objCapBank.cap_kVAr_OneStep(StepDirections::increase); delay(2000); //give some delay after switching
          // Cap will be taken into service
        }
        else if(objModBusVar.pfRange == PF_Ranges::xtrmLead){ // extream leading
          objCapBank.cap_kVAr_OneStep(StepDirections::decrease); delay(2000); //give some delay after switching
          // Cap will be taken out of service
        }
        else{
          // Do nothing
        }
        //Serial.print("Power Factor: "); Serial.println(pf_ModBus);
        objModBusVar.pf = getRegisterValue(ModBusRegAddr_pf);
        objModBusVar.kVAr = getRegisterValue(ModBusRegAddr_kVAr);
        objModBusVar.identify_pfRange();

        displayDashBoard();
        delay(5000);
        displayBanksInService();
        delay(5000);
        displayCounters();
    }
    else{
      displayModBusReadFail();
    }
    delay(5000);
    
  } // ---------------------------------------------- infinite loop ends ---------------------------------------------
