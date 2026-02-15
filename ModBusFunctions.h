/* Keep macros, includes, function declarations but change global objects to extern -> NO memory allocation here */
#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ModbusMaster.h>

#define MAX485_DE_RE A2 // DE and RE tied together
#define MAX485_RO A0 // Max485_Tx, for SoftwareSerial-> Rx
#define MAX485_DI A1 // Max485_Rx, for SoftwareSerial-> Tx
#define SLAVE_ID 1
#define SLAVE_BAUD_RATE 9600

//extern SoftwareSerial Max485_Serial; -> not required bcz 'Max485_Serial' is not used anywhere outside MosBusFunctions.cpp
//extern ModbusMaster node; -> not required bcz 'node' is not used anywhere outside MosBusFunctions.cpp
extern bool modBusReadSuccess;
extern uint8_t modBusComResult;

void preTransmission();
void postTransmission();
void initializeModBusSettings();
float getRegisterValue(uint16_t regAddr);