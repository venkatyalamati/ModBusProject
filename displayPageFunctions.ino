  void displayDashBoard(){
    tftST7735Display.clearScreen(); tftST7735Display.setSmallFont(); tftST7735Display.clearCharBuffer();

  // ------------------------ display first line ----------------------------
    strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("Mod Bus Received")); tftST7735Display.textPrintSmallFont(1);

  // ------------------------ display second line --------------------------
    if(objMFT_Data.pfRange == PF_Ranges::lagging)
      strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("pf: just lagging"));
    else if(objMFT_Data.pfRange == PF_Ranges::leading)
      strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("pf: just leading"));
    else if(objMFT_Data.pfRange == PF_Ranges::xtrmLagg)
      strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("pf: xtrm lagging"));
    else if(objMFT_Data.pfRange == PF_Ranges::xtrmLead)
      strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("pf: xtrm leading"));
    else if(objMFT_Data.pfRange == PF_Ranges::unitypf)
      strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("  - unity pf -  "));
    else
      strcpy_P(tftST7735Display.charBuffer, (PGM_P)F(" - invalid pf - "));

    tftST7735Display.textPrintSmallFont(2);

  // ----------------- display power factor from mod bus--------------------
    strcpy_P(tftST7735Display.charBuffer, (PGM_P)F(" pf Value: 0.00 ")); 
    insertFloatVal_intoCharArray(tftST7735Display.charBuffer, 12, 4, 2, objMFT_Data.pf, false);
    tftST7735Display.textPrintSmallFont(3);

  // ----------------- display low power factor limit -----------------------
    strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("low pf lim: 0.00")); // for both lagging & leading
    insertFloatVal_intoCharArray(tftST7735Display.charBuffer, 13, 4, 2, PF_LIMIT, false);
    tftST7735Display.textPrintSmallFont(4);  

  // ----------------- display reactive power from mod bus------------------
    strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("Net_kVAr: +00.00"));
    insertFloatVal_intoCharArray(tftST7735Display.charBuffer, 11, 6, 2, objMFT_Data.kVAr, true);
    tftST7735Display.textPrintSmallFont(6);
    
  }

  void displayBanksInService(){
    tftST7735Display.clearScreen(); tftST7735Display.setSmallFont(); tftST7735Display.clearCharBuffer();
    strcpy_P(tftST7735Display.charBuffer, (PGM_P)F(" CapBank Status ")); tftST7735Display.textPrintSmallFont(1);
    for(uint8_t i=0; i<4; i++){
      strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("CapBankPos-n : x"));
      tftST7735Display.charBuffer[11] = 1 << i; tftST7735Display.charBuffer[11] += 48;
      tftST7735Display.charBuffer[15] = objCapBank.capBankStatusCurr[i]; tftST7735Display.charBuffer[15] += 48;
      tftST7735Display.textPrintSmallFont(i+2);
    }
  }

  void displayCounters(){
    tftST7735Display.clearScreen(); tftST7735Display.setSmallFont(); tftST7735Display.clearCharBuffer();
    strcpy_P(tftST7735Display.charBuffer, (PGM_P)F(" Counter Values ")); tftST7735Display.textPrintSmallFont(1);
    for(uint8_t i=0; i<4; i++){
      strcpy_P(tftST7735Display.charBuffer, (PGM_P)F(" CB Bit-n 00000 "));
      tftST7735Display.charBuffer[8] = 1 << i; tftST7735Display.charBuffer[8] += 48;
      insertUintVal_intoCharArray(tftST7735Display.charBuffer, 11, 5, objCapBank.switchingCnt[i]);
      tftST7735Display.textPrintSmallFont(i+3);
    }
  }

  void displayModBusReadFail(){
    tftST7735Display.clearScreen(); tftST7735Display.setSmallFont(); tftST7735Display.clearCharBuffer();
    strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("  Mod Bus Read  ")); tftST7735Display.textPrintSmallFont(1);
    strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("     Failed!    ")); tftST7735Display.textPrintSmallFont(2);
    strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("   Error Code   ")); tftST7735Display.textPrintSmallFont(4);
    strcpy_P(tftST7735Display.charBuffer, (PGM_P)F("                "));
    uint8ToHex(objModBusCom.modBusComResult, tftST7735Display.charBuffer, 7); tftST7735Display.textPrintSmallFont(5);
  }

  void uint8ToHex(uint8_t value, char* buffer, uint8_t startPos) { //startPos value 1,2,3,...
  char hexDigits[17]; hexDigits[16]='\0';
      strcpy_P(hexDigits, (PGM_P)F("0123456789ABCDEF"));
      startPos--; buffer[startPos] = '0';
      startPos++; buffer[startPos] = 'x';
      startPos++; buffer[startPos] = hexDigits[(value >> 4) & 0x0F]; // High nibble
      startPos++; buffer[startPos] = hexDigits[value & 0x0F];        // Low nibble
  }

  #define SIZE_TEMP_STR 7 // must be > fullDcmlLen
  void insertFloatVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, uint8_t numDcmlPts, float dcmlVal, bool dispPosSign){
  //Note:- To get startPos, count 1,2,3.. & fullDcmlLen includes '.' and '-' signs also
    char tempStr[SIZE_TEMP_STR];
    for(uint8_t i=0; i<SIZE_TEMP_STR; i++)
      tempStr[i] = '\0';

    dtostrf(dcmlVal, fullDcmlLen, numDcmlPts, tempStr);
    if(dispPosSign) tempStr[0] = '+';

    for(uint8_t i=0; i<fullDcmlLen; i++)
      charArr[i+startPos-1] = tempStr[i];
  }

  void insertUintVal_intoCharArray(char *charArr, uint8_t startPos, uint8_t fullDcmlLen, uint32_t dcmlVal){
  //Note:- To get startPos, count 1,2,3..
    char tempStr[SIZE_TEMP_STR];
    for(uint8_t i=0; i<SIZE_TEMP_STR; i++)
      tempStr[i] = '\0';

    for(uint8_t i=fullDcmlLen; i>0; i--){
      tempStr[i-1] = dcmlVal % 10; dcmlVal /= 10;
      tempStr[i-1] += 48; // To ASCII
    }

    for(uint8_t i=0; i<fullDcmlLen; i++)
      charArr[i+startPos-1] = tempStr[i];
  }