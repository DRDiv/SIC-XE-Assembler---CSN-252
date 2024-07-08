#include "Pass2.h"


map<string, unsigned char>* registers = NULL;

map<string, unsigned char>* getRegMap() {
	if (registers != NULL)
		return registers;

	registers = new map<string, unsigned char>();
	registers->insert(pair<string, unsigned char>("A", 0));
	registers->insert(pair<string, unsigned char>("X", 1));
	registers->insert(pair<string, unsigned char>("L", 2));
	registers->insert(pair<string, unsigned char>("B", 3));
	registers->insert(pair<string, unsigned char>("S", 4));
	registers->insert(pair<string, unsigned char>("T", 5));
	registers->insert(pair<string, unsigned char>("F", 6));
	return registers;
}

opCode generateOpCode(parsedLine pl, int loCtr, cSect* current) {
	opCode toRet;
	string modOp = (pl.opcode).at(0) == '+' ? (pl.opcode).substr(1) : pl.opcode;
	modOp = toUpperCase(modOp);
	
	string op1 = toUpperCase(pl.operand1);
	string op2 = toUpperCase(pl.operand2);

	OpInfo* info;
	try {
		info = getOpTab()->at(modOp); //Only if instruction, not directive
	} catch (out_of_range& orr) { //Directive
		int value = 0;
		if (modOp.compare("WORD") == 0) {
			toRet.format = 11;

			value = evaluateOperand(pl.operand1, current);

			if (value >= -(1 << 23) && value <= (1 << 23) - 1) {
				toRet.displacement = value;
			} else {
				throw new string("Too large to store");
			}
		} else if (modOp.compare("BYTE") == 0) {
			toRet.format = 22;
			
			if (op1.at(0) == 'C') { //max 7 ascii characters +3 'C' '\'' '\''
					
					toRet.value="";
					for (int i=2;i<pl.operand1.size()-1;i++){
						string temp=intToString((int)pl.operand1[i],true);
						adjustStringLength(&temp,2,'0',true);
						toRet.value+=temp;
					}
				
			} else if (op1.at(0) == 'X') {
				toRet.value="";
					for (int i=2;i<pl.operand1.size()-1;i++){
						string temp=pl.operand1[i]+"";
						
						toRet.value+=temp;
					}
			}
		} else if (modOp.at(0) == '=') {
			toRet.format = 22;
			toRet.displacement = valueOfLiteralOperand(pl.opcode);
		} else {
			toRet.format = 33;
		}
		return toRet;
	}

	unsigned char format = info->format;

	if (format == FORMAT_1) {
		toRet.operation = info->opCode;
		toRet.displacement = 0;
		toRet.format = FORMAT_1;
		toRet.flags[5] = 1;
		toRet.flags[4] = 1;
		toRet.flags[3] = 0;
		toRet.flags[2] = 0;
		toRet.flags[1] = 0;
		toRet.flags[0] = 0;
	} else if (format == FORMAT_2) {
		toRet.operation = info->opCode;
		toRet.reg1 = getRegMap()->at(op1);
		toRet.reg2 = (op2).compare("") != 0 ? registers->at(op2) : 0;
		toRet.format = FORMAT_2;
		toRet.flags[5] = 1;
		toRet.flags[4] = 1;
		toRet.flags[3] = 0;
		toRet.flags[2] = 0;
		toRet.flags[1] = 0;
		toRet.flags[0] = 0;
	} else {
		// format 3 or 4
		toRet.operation = info->opCode;
		
		toRet.format = FORMAT_3_4;

		if (modOp.compare("RSUB") == 0) {
			toRet.displacement = 0;
			toRet.flags[5] = 1; //n
			toRet.flags[4] = 1; //i
			toRet.flags[3] = 0; //x
			return toRet;
		}
		toRet.flags[0] = (pl.opcode).at(0) == '+' ? 1 : 0; //e

		if (op1.at(0) == '#') { //Immediate
			toRet.flags[5] = 0; //n
			toRet.flags[4] = 1; //i
			toRet.flags[3] = 0; //x
			
		} else if ((pl.operand1).at(0) == '@') { //Indirect
			toRet.flags[5] = 1; //n
			toRet.flags[4] = 0; //i
			toRet.flags[3] = 0; //x
		} else {
			toRet.flags[5] = 1; //n
			toRet.flags[4] = 1; //i
			toRet.flags[3] = (op2).compare("X") == 0 ? 1 : 0; //x
		}

		int address = 0;
		try {
			if (findExpressionType(pl.operand1) == 2)
				address = addressOfLiteralOperand(pl.operand1);
			else
				address = evaluateOperand(pl.operand1, current);
		} catch (string& err) {
			throw err;
		}

		if (toRet.flags[0]) { //Extended? (20 bits)
			toRet.flags[2] = 0; //b
			toRet.flags[1] = 0; //p
			if (address >= -(1 << 19) && address <= (1 << 19) - 1)
				toRet.displacement = address;
			else
				throw new string("Too large to store");
		} else { //only 12 bits
			toRet.flags[2] = 0; //b
			toRet.flags[1] = 0; //p
			int disp = 0;
			if (findExpressionType(pl.operand1) == 0
							&& !isValidOpNm(pl.operand1)) {
								disp = address;
								toRet.displacement=disp;
								return toRet;
							}
			if (findExpressionType(pl.operand1) == 6
					|| findExpressionType(pl.operand1) == 1
					|| findExpressionType(pl.operand1) == 2
					|| (findExpressionType(pl.operand1) == 0
							&& isValidOpNm(pl.operand1)))
				disp = address - loCtr;
			else
				disp = address; //Numeric value after #
			if (disp >= -(1 << 11) && disp <= (1 << 11) - 1) { //Because PC is signed
				toRet.displacement = disp;
				toRet.flags[1] = 1;
				return toRet;

			} 
			
			if (current->baseAdd=="")throw new string("Too large without format 4");
			
			disp = 0;
			
			if (findExpressionType(pl.operand1) == 6
					|| findExpressionType(pl.operand1) == 1
					|| findExpressionType(pl.operand1) == 2
					|| (findExpressionType(pl.operand1) == 0
							&& isValidOpNm(pl.operand1)))
				disp = address - current->symTab[current->baseAdd]->address;
			else
				disp = address; //Numeric value after #
			if (disp >= (0) && disp <= (1 << 12) - 1) { 
				toRet.displacement = disp;
				toRet.flags[2] = 1;
				return toRet;
				
			} 
			throw new string("Too large without format 4");
		}
	}
	return toRet;
}

string calcOpCode(opCode op) {

	string returnedString = "";
	if (op.format == 11) { // reserved word, length = 6
		string final_opCode = intToString(op.displacement, true);
		adjustStringLength(&final_opCode, 6, '0', true);
		return final_opCode;
	} else if (op.format == 22) { // reserved byte, length = 2
		string final_opCode = op.value;
		
		return final_opCode;
	} else if (op.format == 33) {
		return returnedString;
	} else if (op.format == FORMAT_1) {
		returnedString = intToString(op.operation, true);
	} else if (op.format == FORMAT_2) {
		returnedString = ((intToString(op.operation, true)
				+ intToString(op.reg1, false)) + intToString(op.reg2, false));
	} else { // format = 3 or 4
	
		int ni_val = 0, xbpe_val = 0;
		binaryToDec(op.flags, &ni_val, &xbpe_val);
		string disp_str = intToString(op.displacement, true);
		if (op.flags[0] == 0) {
			adjustStringLength(&disp_str, 3, '0', true);
			disp_str = disp_str.substr(disp_str.length() - 3);
		} else {
			adjustStringLength(&disp_str, 5, '0', true);
			disp_str = disp_str.substr(disp_str.length() - 5);
		}
		string op1=intToString(op.operation + ni_val, true);
		adjustStringLength(&op1,2,'0',true);
		returnedString =op1
				+ intToString(xbpe_val, true) + disp_str;
		
	}
	return returnedString;
}

void binaryToDec(bitset<6> flags, int* ni_val, int* xbpe_val) { // takes all 6 flags
	bitset<4> xbpe_flags;
	for (int i = 0; i < 4; i++)
		xbpe_flags[i] = flags[i];

	bitset<2> ni_flags;
	for (int i = 0; i < 2; i++)
		ni_flags[i] = flags[i + 4];

	*ni_val = *xbpe_val = 0;
	stringstream ss_1, ss_2;
	ss_1 << uppercase << std::hex << ni_flags.to_ulong();
	ss_1 >> std::hex >> *ni_val;
	ss_2 << std::hex << uppercase << xbpe_flags.to_ulong();
	ss_2 >> std::hex >> *xbpe_val;
}

string errorMessages;
void runPass2(ofstream* opFile, vector<cSect>* cSects) {

	errorMessages = "";
	writeBorder_Pass2(opFile);
	writeHeader_Pass2(opFile);
	vector<vector<string> > objectCodes;
	bool success = true;
	int starting = cSects->at(0).locpl.at(0).loc;
	cSect cc = cSects->at(cSects->size() - 1);
	int ending = cc.locpl.at(cc.locpl.size() - 1).loc;
	for (unsigned int i = 0; i < cSects->size(); i++) {
		vector<string> ii;
		objectCodes.push_back(ii);
		vector<locatedParsedLine> v = cSects->at(i).locpl;
		for (unsigned j = 0; j < v.size() - 1; j++) {
			opCode x;
			string objectCode = "";
			try {
				x = generateOpCode(v[j].p, v[j + 1].loc, &(cSects->at(i)));
			
				objectCode = calcOpCode(x);
			
			} catch (string* err) {
				errorMessages += "\n";
				errorMessages += *err;
				success = false;
			
				delete err;
			} catch (string& err) {
				errorMessages += "\n";
				errorMessages += err;
				success = false;
				
			}

			string operands = mergeStrings(v[j].p.operand1, v[j].p.operand2,
					',');
			string line = constructLine_Pass2(v[j].loc, v[j].p.lable,
					v[j].p.opcode, operands, x.flags, objectCode);

			objectCodes[i].push_back(objectCode);
			writeLine(opFile, line);
			
			if (errorMessages.compare("") != 0) {
				cout<<errorMessages<<endl;
				writeError(opFile, errorMessages);
				errorMessages = "";
			}
		}
	}

	if (!success)
		return;

	opFile->close();

	string out2 = "objectFile.txt";
	deleteFile(out2.c_str());
	opFile->open(out2.c_str(), std::ios::app);

	for (unsigned int i = 0; i < cSects->size(); i++) {
		
		writeHeaderRecord(opFile, cSects->at(i).name, starting, ending - starting);
		writeDefineRecord(opFile, &(cSects->at(i).extDef),
				&(cSects->at(i).symTab));
		writeReferRecord(opFile, &(cSects->at(i).extRef));
		writeTextRecords(opFile, &(objectCodes[i]), &(cSects->at(i).locpl));
		writeModRecords(opFile, &(cSects->at(i).mods));
		writeEndRecord(opFile,cSects->at(i).firstInstruction);
		writeBorder_Pass2(opFile);
	}

	return;
}
