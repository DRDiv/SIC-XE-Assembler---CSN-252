#ifndef PASS2_H_
#define PASS2_H_



#include <iostream>
#include "OpTab.h"
#include "IO.h"
#include "StringUtilities.h"
#include "structs.h"
#include "OperandHandler.h"

map<string, unsigned char>* getRegMap();
opCode generateOpCode(parsedLine pl, int loCtr, cSect* current);
string calcOpCode(opCode op);
void binaryToDec(bitset<6> flags, int* ni_val, int* xbpe_val);
void runPass2(ofstream* opFile, vector<cSect>* cSects);
#endif
