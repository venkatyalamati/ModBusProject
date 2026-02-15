/* This .cpp file is the place where objects are defined -> memory is allocated */
#include "ModBusFunctions.h"

SoftwareSerial Max485_Serial(MAX485_RO, MAX485_DI); // Create SoftwareSerial port
// Note:- SoftwareSerial library does NOT support parity. Hence, set the parity as None in the slave device

ModbusMaster node;
bool modBusReadSuccess;
uint8_t modBusComResult;

void preTransmission() {
  digitalWrite(MAX485_DE_RE, HIGH);
  delayMicroseconds(100); // Ensure stable switching
}

void postTransmission() {
  delayMicroseconds(100); // Ensure stable switching
  digitalWrite(MAX485_DE_RE, LOW);
}

void initializeModBusSettings(){
  pinMode(MAX485_DE_RE, OUTPUT);
  digitalWrite(MAX485_DE_RE, LOW); // Initiate to receive

  Max485_Serial.begin(SLAVE_BAUD_RATE); // SERIAL_8E1 -> 8 data bits, Even parity, 1 stop bit

  node.begin(SLAVE_ID, Max485_Serial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

float getRegisterValue(uint16_t regAddr){
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

  