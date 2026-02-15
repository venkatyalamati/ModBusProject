// -------------- EEPROM related functions ---------------
  bool erase_eeprom_if_req(int addrValStr){
    int checkResult = 0;
    char validationString[12]; char checkString[12];
    
    validationString[11]='\0';
    
    strcpy_P(validationString, (PGM_P)F("DATAISVALID"));
    
    for(int j = 0; j < 11; j++)
      checkString[j] = ' ';
    checkString[11] = '\0';

    for(int j = 0; j < 11; j++) {   
      checkString[j] = EEPROM.read(addrValStr + j);
      delay(1);
    }

    checkResult = strcmp(validationString, checkString);

    if(checkResult!=0){ // The strings are NOT equal, hence internal eeprom needs to be erased
      for(int j = 0; j < 1024; j++){ 
        EEPROM.write(j, 0);
        delay(10);
      }
      for(int j = 0; j < 11; j++) { // put validation string on internal eeprom to avoid erase on every startup
        EEPROM.write((addrValStr + j), validationString[j]);
        delay(10);
      }
      return true;
    }
    else{
      return false;
    }
    
  } // erase_eeprom_if_req ends