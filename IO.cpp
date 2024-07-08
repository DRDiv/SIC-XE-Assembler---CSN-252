#include "IO.h"


int counter = 0;

string constructLine(int address, string label, string op, string operands) {
	int length;
	char spaceChar = SPACE_CHAR;
	char zeroChar = ZERO_CHAR;
	string line;
	string lineNumberStr = intToString(++counter, false);
	length = LINE_NUMBER_LENGTH;
	string addressStr = intToString(address, true);
	adjustStringLength(&lineNumberStr, length, spaceChar, false);
	length = 6;
	adjustStringLength(&addressStr, length, zeroChar, true);
	length = ADDRESS_LENGTH;
	adjustStringLength(&addressStr, length, spaceChar, false);
	length = LABEL_LENGTH;
	if (op.at(0) == '+')
		length -= 1;
	adjustStringLength(&label, length, spaceChar, false);
	length = MNEMONIC_LENGTH;
	if (op.at(0) == '+')
		length += 1;
	adjustStringLength(&op, length, spaceChar, false);
	length = OPERANDS_LENGTH;
	adjustStringLength(&operands, length, spaceChar, false);
	line = lineNumberStr + addressStr + label + op + operands;
	return line;
}

string constructLine_Pass2(int address, string label, string op_str,
		string operands, bitset<6> flags, string opCode) {
	int length;
	char spaceChar = SPACE_CHAR;
	char zeroChar = ZERO_CHAR;
	string line;
	string lineNumberStr = intToString(++counter, false);
	length = LINE_NUMBER_LENGTH;
	string addressStr = intToString(address, true);
	adjustStringLength(&lineNumberStr, length, spaceChar, false);
	length = 6;
	adjustStringLength(&addressStr, length, zeroChar, true);
	length = ADDRESS_LENGTH;
	adjustStringLength(&addressStr, length, spaceChar, false);
	length = LABEL_LENGTH;
	if (op_str.at(0) == '+')
		length -= 1;
	adjustStringLength(&label, length, spaceChar, false);
	length = MNEMONIC_LENGTH;
	if (op_str.at(0) == '+')
		length += 1;
	adjustStringLength(&op_str, length, spaceChar, false);
	length = OPERANDS_LENGTH;
	adjustStringLength(&operands, length, spaceChar, false);
	string flags_str = flags.to_string(), mod_flags_str = "";
	if (flags_str.compare("000000") != 0) {
		for (int i = 0; (unsigned) i < flags_str.size(); i++) {
			mod_flags_str += flags_str[i];
			if ((unsigned) i + 1 < flags_str.size())
				mod_flags_str += " ";
		}
	}
	length = FLAGS_LENGTH;
	adjustStringLength(&mod_flags_str, length, spaceChar, false);
	length = OPCODE_LENGTH;
	adjustStringLength(&opCode, length, spaceChar, false);
	line = lineNumberStr + addressStr + label + op_str + operands
			+ mod_flags_str + opCode;
	return line;
}

void deleteFile(string name) {
	remove(name.c_str());

}

void writeLine(ofstream* file, string line) {
	*file << line << endl;
}

string readLine(ifstream* file) {
	string line = "";
	getline(*file, line);
	return line;
}

void writeHeader(ofstream* file) {

	int fileLineLength = FILE_LINE_LENGTH;
	int lineNumberLength = LINE_NUMBER_LENGTH;
	int addressLength = ADDRESS_LENGTH;
	int labelLength = LABEL_LENGTH;
	int mnemonicLength = MNEMONIC_LENGTH;
	int operandsLength = OPERANDS_LENGTH;
	char spaceChar = SPACE_CHAR;
	char equalChar = EQUAL_CHAR;
	string line = "";
	line = "\t Pass\t1 ... \n";
	writeLine(file, line);
	line = "";
	string temp = "line number";
	adjustStringLength(&temp, lineNumberLength, spaceChar, false);
	line += temp;
	temp = "address";
	adjustStringLength(&temp, addressLength, spaceChar, false);
	line += temp;
	temp = "label";
	adjustStringLength(&temp, labelLength, spaceChar, false);
	line += temp;
	temp = "op";
	adjustStringLength(&temp, mnemonicLength, spaceChar, false);
	line += temp;
	temp = "operands";
	adjustStringLength(&temp, operandsLength, spaceChar, false);
	line += temp;
	writeLine(file, line);
	line = "";
	adjustStringLength(&line, fileLineLength, equalChar, false);
	writeLine(file, line);

}

void writeHeader_Pass2(ofstream* file) {
	int pass2Length = PASS_2_LENGTH;
	int lineNumberLength = LINE_NUMBER_LENGTH;
	int addressLength = ADDRESS_LENGTH;
	int labelLength = LABEL_LENGTH;
	int mnemonicLength = MNEMONIC_LENGTH;
	int operandsLength = OPERANDS_LENGTH;
	int flagsLength = FLAGS_LENGTH;
	int opCodeLength = OPCODE_LENGTH;
	char spaceChar = SPACE_CHAR;
	char equalChar = EQUAL_CHAR;
	string line = "";
	line = "\t Pass\t2 ... \n";
	writeLine(file, line);
	line = "";
	string temp = "line number";
	adjustStringLength(&temp, lineNumberLength, spaceChar, false);
	line += temp;
	temp = "address";
	adjustStringLength(&temp, addressLength, spaceChar, false);
	line += temp;
	temp = "label";
	adjustStringLength(&temp, labelLength, spaceChar, false);
	line += temp;
	temp = "op";
	adjustStringLength(&temp, mnemonicLength, spaceChar, false);
	line += temp;
	temp = "operands";
	adjustStringLength(&temp, operandsLength, spaceChar, false);
	line += temp;
	temp = "n i x b p e";
	adjustStringLength(&temp, flagsLength, spaceChar, false);
	line += temp;
	temp = "opcode";
	adjustStringLength(&temp, opCodeLength, spaceChar, false);
	line += temp;
	writeLine(file, line);
	line = "";
	adjustStringLength(&line, pass2Length, equalChar, false);
	writeLine(file, line);

}

void writeError(ofstream* file, string error) {
	vector<string> vecOfStrs = split(error, '\n');

	for (unsigned int i = 0; i < vecOfStrs.size(); i++) {
		error = vecOfStrs.at(i);
		int errLength = error.length();
		int offset = 19;
		int excess;
		char spaceChar = SPACE_CHAR;
		char asterisk = ASTERISK_CHAR;

		adjustStringLength(&error, errLength + 3, asterisk, true);
		adjustStringLength(&error, errLength + offset, spaceChar, true);
		excess = 63 - errLength;

		adjustStringLength(&error, errLength + excess, spaceChar, false);

		writeLine(file, error);

	}
}

void writeBorder(ofstream* file) {
	char newLine = NEW_LINE_CHAR;
	int fileLineLength = FILE_LINE_LENGTH;
	string line = "";
	char asterickChar = ASTERISK_CHAR;
	for (int i = 0; i < fileLineLength; i++)
		line += asterickChar;
	writeLine(file, newLine + line + newLine);
}

void writeBorder_Pass2(ofstream* file) {
	char newLine = NEW_LINE_CHAR;
	int fileLineLength = PASS_2_LENGTH;
	string line = "";
	char asterickChar = ASTERISK_CHAR;
	for (int i = 0; i < fileLineLength; i++)
		line += asterickChar;
	writeLine(file, newLine + line + newLine);
}

void writeSymTab(ofstream* file, map<string, symInfo*>* theMap) {

	if (theMap->empty())
		return;
	char spaceChar = SPACE_CHAR;
	char dashChar = DASH_CHAR;
	char equalChar = EQUAL_CHAR;
	char zeroChar = ZERO_CHAR;

	string temp, headerLine = "";
	writeLine(file, "\t Symbol\t\t Table\t\t (values in hex)\n");
	temp = "";
	adjustStringLength(&temp, 33, equalChar, false);
	writeLine(file, temp);
	temp = "|\tname";
	adjustStringLength(&temp, 11, spaceChar, false);
	headerLine += temp;
//	writeLine(file, temp);
	temp = "address";
	adjustStringLength(&temp, 10, spaceChar, false);
	headerLine += temp;
	temp = "Abs/Rel";
	temp += "\t|";
	headerLine += temp;
	writeLine(file, headerLine);
	temp = "|\t";
	adjustStringLength(&temp, 28, dashChar, false);
	temp += "\t|";
	writeLine(file, temp);
	for (map<string, symInfo*>::const_iterator it = theMap->begin(); it
			!= theMap->end(); it++) {
		string symbol = "|\t" + it->first;
		symInfo* n = it->second;
		string address = intToString(n->address, true);
		string rel = "";
		if (n->relocatble)
			rel = "Rel";
		else
			rel = "Abs";
		adjustStringLength(&symbol, 11, spaceChar, false);
		string line = symbol;
		adjustStringLength(&address, 4, zeroChar, true);
		adjustStringLength(&address, 5, spaceChar, true);
		adjustStringLength(&address, 10, spaceChar, false);
		line += address;
		adjustStringLength(&address, 10, spaceChar, true);
		adjustStringLength(&rel, 7, spaceChar, true);
		rel += "\t|";
		line += rel;
		writeLine(file, line);
	}
	temp = "";
	adjustStringLength(&temp, 33, equalChar, false);
	writeLine(file, temp);
}

void writeComment(ofstream* file, string comment) {
	int lineNumberLength = LINE_NUMBER_LENGTH;
	int addressLength = ADDRESS_LENGTH;
	char spaceChar = SPACE_CHAR;
	adjustStringLength(&comment,
			comment.size() + lineNumberLength + addressLength, spaceChar, true);
	writeLine(file, comment);
}

void writeHeaderRecord(ofstream* file, string prog_name, int starting_add, int prog_length) { 
	char spaceChar = SPACE_CHAR;
	char zeroChar = ZERO_CHAR;

	// string line = "\t Header Record :\t\t\n";
	// writeLine(file, line);
	string line = "H^";
	adjustStringLength(&line, 11, spaceChar, true);
	
	if (prog_name.compare("") != 0) adjustStringLength(&prog_name, 6, spaceChar, false);

	line += prog_name;
	line += "^";
	string starting_add_str = intToString(starting_add, true);
	adjustStringLength(&starting_add_str, 6, zeroChar, true);
	line += starting_add_str;
	line += "^";
	string prog_length_str = intToString(prog_length, true);
	adjustStringLength(&prog_length_str, 6, zeroChar, true);
	line += prog_length_str;
	writeLine(file, line);
	line = "";
	adjustStringLength(&line, 11, spaceChar, true);
	// line += "Program Name: ";
	// if (prog_name.compare("") == 0) line += "Unknown.";
	// else line += prog_name;
	// writeLine(file, line);
	line = "";
	adjustStringLength(&line, 11, spaceChar, true);
	// line += "Loading Address: ";
	// line += starting_add_str;
	// line += "\n";
	// writeLine(file, line);

}

void writeTextRecords(ofstream* file, vector<string>* opCodes, vector<locatedParsedLine>* lpl) { 
	char zeroChar = ZERO_CHAR;
	char spaceChar = SPACE_CHAR;
	int start_address = 0;
	int end_address = 0;
	string start_address_str;
	string length_str;

	// string line = "\tText Record(s) :\t\t\n";
	string temp_line = "";
	// writeLine(file, line);
	string line;
	
	for (unsigned int i = 0; i< opCodes->size();){
		if ((*opCodes)[i].compare("") == 0 || (*lpl)[i].p.lable.compare("*") == 0){
				i++;
				continue;
		}
		
		line ="T^";
		adjustStringLength(&line, 11, spaceChar, true);
		start_address = (*lpl)[i].loc;
		start_address_str = intToString(start_address, true);
		adjustStringLength(&start_address_str, 6, zeroChar, true);
		line += start_address_str;
		int x = 0;
		string temp = "";
		while (1){
		
			if (i >= (*opCodes).size() || (x+(*opCodes)[i].length()) > 60)
				break;
				
			if ((*opCodes)[i].compare("") == 0 || (*lpl)[i].p.lable.compare("*") == 0){
					i++;
					break;
			}
			temp += "^";
			temp += (*opCodes)[i];
			if (i+1 < (*opCodes).size()) end_address = (*lpl)[i+1].loc;
			else end_address = (*lpl)[i].loc;
			
			x+=(*opCodes)[i].length();
			i++;
		
		}
		length_str = intToString(end_address - start_address, true);
		adjustStringLength(&length_str, 2, zeroChar, true);
		line += "^";
		line += length_str;
		line += temp;
		writeLine(file, line);
	}
}

void writeModRecords(ofstream* file, vector<modification>* mods) { 
	char zeroChar = ZERO_CHAR;
	char spaceChar = SPACE_CHAR;
	int address = 0, hBytes = 0;
	string address_str, hBytes_str;
	// string line = "\tModification Record(s) :\t\t\n";
	// writeLine(file, line);
	string line;
	for (int i = 0; (unsigned)i < mods->size(); i++) {
		modification mod = (*mods)[i];
		if (mod.exRef) {
			// multiple mod records
			for (int j = 0; (unsigned)j < mod.refs.size(); j++) {
				line = "M^";
				adjustStringLength(&line, 11, spaceChar, true);
				address = mod.address;
				address_str = intToString(address, true);
				
				adjustStringLength(&address_str, 6, zeroChar, true);
				line += address_str;
				line += "^";
				hBytes = mod.hbytes;
				hBytes_str = intToString(hBytes, false);
				adjustStringLength(&hBytes_str, 2, zeroChar, true);
				line += hBytes_str;
				line += "^";
				line += mod.signs[j];
				line += mod.refs[j];
				writeLine(file, line);
			}
		}
		else {
			line = "M^";
			adjustStringLength(&line, 11, spaceChar, true);
			address = mod.address;
			address_str = intToString(address, true);
			adjustStringLength(&address_str, 6, zeroChar, true);
			line += address_str;
			line += "^";
			hBytes = mod.hbytes;
			hBytes_str = intToString(hBytes, false);
			adjustStringLength(&hBytes_str, 2, zeroChar, true);
			line += hBytes_str;
			writeLine(file, line);
		}
	}
	writeLine(file, "");
}

void writeDefineRecord(ofstream* file, vector<string>* exdefs, map<string, symInfo*>* theMap) { 
	char zeroChar = ZERO_CHAR;
	char spaceChar = SPACE_CHAR;

	int address = 0;
	int i = 0;
	string address_str;
	// string line = "\tDefine Record :\t\t\n";
	// writeLine(file, line);
	string line;
	for (; (unsigned)i < exdefs->size(); i++) {
		line = "D^";
		adjustStringLength(&line, 11, spaceChar, true);
		do {
			if ((unsigned)i >= exdefs->size()) break;
			string s = (*exdefs)[i];
			symInfo* sInfo = theMap->at(s);
			address = sInfo->address;
			address_str = intToString(address, true);
			adjustStringLength(&address_str, 6, zeroChar, true);
			line += (*exdefs)[i];
			line += "^";
			line += address_str;
			if ((i+1) % 5 != 0 && (unsigned)(i+1) < exdefs->size()) line += "^";
			i++;
		} while (i % 5 != 0);
		// if ((unsigned)i >= exdefs->size())line += "\n";
		i--;
		writeLine(file, line);
	}

}

void writeReferRecord(ofstream* file, vector<string>* exrefs) { 
	char spaceChar = SPACE_CHAR;
	int i = 0;

	// string line = "\tRefer Record :\t\t\n";
	// writeLine(file, line);
	string line;
	for (; (unsigned)i < exrefs->size(); i++) {
		line = "R^";
		adjustStringLength(&line, 11, spaceChar, true);
		do {
			if ((unsigned)i >= exrefs->size()) break;
			line += (*exrefs)[i];
			if ((i+1) % 5 != 0 && (unsigned)(i+1) < exrefs->size()) line += "^";
			i++;
		} while (i % 5 != 0);
		// if ((unsigned)i >= exrefs->size()) line += "\n";
		i--;
		writeLine(file, line);
	}
}

void writeEndRecord(ofstream* file,int firstInstruc) {
	char zeroChar = ZERO_CHAR;
	char spaceChar = SPACE_CHAR;
	
	string line = "E^";
	adjustStringLength(&line, 11, spaceChar, true);
	string firstInstruc_str = intToString(firstInstruc, true);
	
	adjustStringLength(&firstInstruc_str, 6, zeroChar, true);
	line += firstInstruc_str;
	writeLine(file,line);
}
