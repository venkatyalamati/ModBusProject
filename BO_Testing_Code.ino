extern char arr16[17];
void executeCtrlPinTestSwitching(byte startPin, byte endPin){
    byte pin, BO_Num, i, flag;
    if(erase_eeprom_if_req(addrEepromValidString)){
      EEPROM.write(901, 1); delay(10);
    }
        
    for(pin=startPin; pin<=endPin; pin++){
      BO_Num = pin-startPin+1;
      flag = EEPROM.read(900+BO_Num);
      if(flag == 1){
        EEPROM.write(900+BO_Num, 0); delay(10);
        if(BO_Num < (endPin-startPin+1)){
          EEPROM.write(900+BO_Num+1, 1); delay(10);
        }
        else{
          EEPROM.write(901, 1); delay(10);
        }

        for(i=0; i<3; i++){
          displayWarningTestSwitching(BO_Num); delay(2500);
        }

        pinMode(pin, OUTPUT); displaySwitchingPinDetails(BO_Num);
        for(i=1; i<=5; i++){
          digitalWrite(pin, HIGH); display_Switching_Status_n_Count(1, i); delay(2000);
          digitalWrite(pin, LOW); display_Switching_Status_n_Count(0, i); delay(2000);
        }
        break;
      } // if(flag == 1)
      
    } // outer for loop
    while(true){}
  }

  void displaySwitchingPinDetails(byte BO_Num){
    ucg.clearScreen(); ucg.setFont(ucg_font_9x18_tf);
    strcpy_P(arr16, (PGM_P)F(" Test Switching ")); textPrintOnTFT_SmallFont(arr16, 1);
    strcpy_P(arr16, (PGM_P)F("  BO No: 1 - 7  ")); textPrintOnTFT_SmallFont(arr16, 2);
    strcpy_P(arr16, (PGM_P)F(" Curr. BO No: n ")); arr16[14] = BO_Num + 48; textPrintOnTFT_SmallFont(arr16, 4);
  }

  void display_Switching_Status_n_Count(byte sta, byte cnt){
      strcpy_P(arr16, (PGM_P)F("Cnt: n , Sta: n ")); arr16[5] = cnt + 48; arr16[14] = sta + 48;
      textPrintOnTFT_SmallFont(arr16, 5);
  }

  void displayWarningTestSwitching(byte BO_Num){
    ucg.clearScreen(); ucg.setFont(ucg_font_9x18_tf);
    strcpy_P(arr16, (PGM_P)F(" ## Warning! ## ")); textPrintOnTFT_SmallFont(arr16, 1);
    strcpy_P(arr16, (PGM_P)F("   Program is   ")); textPrintOnTFT_SmallFont(arr16, 2);
    strcpy_P(arr16, (PGM_P)F("       in       ")); textPrintOnTFT_SmallFont(arr16, 3);
    strcpy_P(arr16, (PGM_P)F(" Test Switching ")); textPrintOnTFT_SmallFont(arr16, 4);
    strcpy_P(arr16, (PGM_P)F("     Mode!!     ")); textPrintOnTFT_SmallFont(arr16, 5);
    strcpy_P(arr16, (PGM_P)F("   Test BO: x   ")); arr16[12] = BO_Num + 48; textPrintOnTFT_SmallFont(arr16, 6);
  }