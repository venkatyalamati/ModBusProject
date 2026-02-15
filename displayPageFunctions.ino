  char arr16[17];

  void displayDashBoard(){
    ucg.clearScreen(); ucg.setFont(ucg_font_9x18_tf);

  // ------------------------ display first line ----------------------------
    strcpy_P(arr16, (PGM_P)F("Mod Bus Received")); textPrintOnTFT_SmallFont(arr16, 1);

  // ------------------------ display second line --------------------------
    if(objModBusVar.pfRange == PF_Ranges::lagging)
      strcpy_P(arr16, (PGM_P)F("pf: just lagging"));
    else if(objModBusVar.pfRange == PF_Ranges::leading)
      strcpy_P(arr16, (PGM_P)F("pf: just leading"));
    else if(objModBusVar.pfRange == PF_Ranges::xtrmLagg)
      strcpy_P(arr16, (PGM_P)F("pf: xtrm lagging"));
    else if(objModBusVar.pfRange == PF_Ranges::xtrmLead)
      strcpy_P(arr16, (PGM_P)F("pf: xtrm leading"));
    else
      strcpy_P(arr16, (PGM_P)F("pf: unity pf!   "));

    textPrintOnTFT_SmallFont(arr16, 2);

  // ----------------- display power factor from mod bus--------------------
    strcpy_P(arr16, (PGM_P)F(" pf Value: 0.00 ")); 
    insertFloatVal_intoCharArray(arr16, 12, 4, 2, objModBusVar.pf, false);
    textPrintOnTFT_SmallFont(arr16, 3);

  // ----------------- display low power factor limit -----------------------
    strcpy_P(arr16, (PGM_P)F("low pf lim: 0.00")); // for both lagging & leading
    insertFloatVal_intoCharArray(arr16, 13, 4, 2, PF_LIMIT, false);
    textPrintOnTFT_SmallFont(arr16, 4);  

  // ----------------- display reactive power from mod bus------------------
    strcpy_P(arr16, (PGM_P)F("Net_kVAr: +00.00"));
    insertFloatVal_intoCharArray(arr16, 11, 6, 2, objModBusVar.kVAr, true);
    textPrintOnTFT_SmallFont(arr16, 6);
    
  }

  void displayBanksInService(){
    ucg.clearScreen(); ucg.setFont(ucg_font_9x18_tf);
    strcpy_P(arr16, (PGM_P)F(" CapBank Status ")); textPrintOnTFT_SmallFont(arr16, 1);
    for(uint8_t i=0; i<4; i++){
      strcpy_P(arr16, (PGM_P)F("CapBankPos-n : x"));
      arr16[11] = 1 << i; arr16[11] += 48;
      arr16[15] = objCapBank.capBankStatusCurr[i]; arr16[15] += 48;
      textPrintOnTFT_SmallFont(arr16, i+2);
    }
  }

  void displayCounters(){
    ucg.clearScreen(); ucg.setFont(ucg_font_9x18_tf);
    strcpy_P(arr16, (PGM_P)F(" Counter Values ")); textPrintOnTFT_SmallFont(arr16, 1);
    for(uint8_t i=0; i<4; i++){
      strcpy_P(arr16, (PGM_P)F(" CB Bit-n 00000 "));
      arr16[8] = 1 << i; arr16[8] += 48;
      insertUintVal_intoCharArray(arr16, 11, 5, objCapBank.switchingCnt[i]);
      textPrintOnTFT_SmallFont(arr16, i+3);
    }
  }

  void displayModBusReadFail(){
    ucg.clearScreen(); ucg.setFont(ucg_font_9x18_tf);
    strcpy_P(arr16, (PGM_P)F("  Mod Bus Read  ")); textPrintOnTFT_SmallFont(arr16, 1);
    strcpy_P(arr16, (PGM_P)F("     Failed!    ")); textPrintOnTFT_SmallFont(arr16, 2);
    strcpy_P(arr16, (PGM_P)F("   Error Code   ")); textPrintOnTFT_SmallFont(arr16, 4);
    strcpy_P(arr16, (PGM_P)F("                ")); uint8ToHex(modBusComResult, arr16, 7); textPrintOnTFT_SmallFont(arr16, 5);
  }

  void textPrintOnTFT_SmallFont(char* txtLine, uint8_t linePos){ // 13 char max, linPos 1,2,3..
    ucg.setPrintPos(9, linePos*20);
    ucg.print(txtLine);
  }

  void uint8ToHex(uint8_t value, char* buffer, uint8_t startPos) { //startPos value 1,2,3,...
  char hexDigits[17]; hexDigits[16]='\0';
      strcpy_P(hexDigits, (PGM_P)F("0123456789ABCDEF"));
      startPos--; buffer[startPos] = '0';
      startPos++; buffer[startPos] = 'x';
      startPos++; buffer[startPos] = hexDigits[(value >> 4) & 0x0F]; // High nibble
      startPos++; buffer[startPos] = hexDigits[value & 0x0F];        // Low nibble
  }

  void initializeDisplay(){
    arr16[16] = '\0';
    ucg.begin(UCG_FONT_MODE_SOLID);
    ucg.setRotate270();
    ucg.setColor(1, 0, 0, 0);        // Background color: Black (R,G,B)
    ucg.setColor(0, 255, 255, 255);  // Text color: White (R,G,B)
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