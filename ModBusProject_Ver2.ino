  //------------------------------------------Include Libraries Below------------------------------------------------
  #include "CustomClasses.h"
  //--------------------------------------------Constants Definitions------------------------------------------------
  constexpr float PF_LIMIT = 0.92f; // limit for both lagging & leading
  constexpr uint16_t ModBusRegAddr_pf = 30063;
  constexpr uint16_t ModBusRegAddr_kVAr = 30061;
  constexpr int addrEepromValidString = 500;
  //------------------------------------------Gloabl Variable Definitions--------------------------------------------

  // Pins used from Bit-0 to Bit-6 on the PCB are: 2,3,4,5,6,7,8 respectively
  CapacitorBank objCapBank(2, 3, 4, 5); // Bit_3_CtrlPin, Bit_2_CtrlPin, Bit_1_CtrlPin, Bit_0_CtrlPin

  ModBusComHandle objModBusCom(1, A0, A1);
  
  TFTDisplay tftST7735Display(9, 10);

  MFT_Data_Handle objMFT_Data(PF_LIMIT);
 
  //------------------------------------------Main Setup Code Below--------------------------------------------------
  void setup() {
    //Serial.begin(9600);
    delay(1000);
    erase_eeprom_if_req(addrEepromValidString);
    tftST7735Display.begin();
    objCapBank.doInitialSetup();
    objModBusCom.begin();

    //executeCtrlPinTestSwitching(2, 8);
   
  } // ------------------------------------------Main Setup ends-----------------------------------------------------

  //------------------------------------------Main Loop Code Below---------------------------------------------------
  void loop(){

    objMFT_Data.pf = objModBusCom.getRegisterValue(ModBusRegAddr_pf);
    objMFT_Data.kVAr = objModBusCom.getRegisterValue(ModBusRegAddr_kVAr);
    objMFT_Data.identify_pfRange();
    
    if(objModBusCom.modBusReadSuccess){
        if(objMFT_Data.pfRange == PF_Ranges::xtrmLagg){ // extream lagging
          objCapBank.cap_kVAr_OneStep(StepDirections::increase); delay(2000); //give some delay after switching
          // Cap will be taken into service
        }
        else if(objMFT_Data.pfRange == PF_Ranges::xtrmLead){ // extream leading
          objCapBank.cap_kVAr_OneStep(StepDirections::decrease); delay(2000); //give some delay after switching
          // Cap will be taken out of service
        }
        else{
          // Do nothing
        }
        //Serial.print("Power Factor: "); Serial.println(pf_ModBus);
        objMFT_Data.pf = objModBusCom.getRegisterValue(ModBusRegAddr_pf);
        objMFT_Data.kVAr = objModBusCom.getRegisterValue(ModBusRegAddr_kVAr);
        objMFT_Data.identify_pfRange();

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
