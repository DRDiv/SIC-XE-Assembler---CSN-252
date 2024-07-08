#include "Pass1.h"
#include "IO.h"
#include "Pass2.h"

ifstream ipfile;
ofstream opfile;

bool started;
bool ended;

map<string, OpInfo*>* mainOpTab;


bool needNoOperands(string s) {
	
	s=toUpperCase(s);
	
	return s.compare("RSUB") == 0 || s.compare("NOBASE") == 0
			|| s.compare("HIO") == 0 || s.compare("FIX") == 0
			|| s.compare("NORM") == 0 || s.compare("TIO") == 0 || s.compare("START") == 0
			|| s.compare("SIO") == 0 || s.compare("FLOAT") == 0||s.compare("CSECT") == 0;
}

parsedLine parse(string line) {
	
	unsigned int i;
	parsedLine pl;
	pl.lable = "";
	pl.opcode = "";
	pl.operand1 = "";
	pl.operand2 = "";

	string s[5] = { "", "", "", "", "" };
	
    i = 0;
	bool boo=true;
	for (unsigned int j = 0; j < 5 && i < line.length(); j++) {
		if(boo) {for (; i < line.length() && (line[i] == ' ' || line[i] == '\t'); i++);
		
		for (; i < line.length() && (line[i] != ' ' && line[i] != '\t'); i++)
			s[j] += line[i];}
		else{
			for(; i < line.length() ; i++)
			s[j] += line[i];
		}
		
        if(j==0){
        	string temp = toUpperCase(s[0]);
            if(temp.compare("EXTREF")==0 || temp.compare("EXTDEF") == 0){
            	pl.opcode = s[0];
                for(int k=i; k<line.length(); k++)
                   
                        pl.operand1 += line[k];
                return pl;
            }
        }else if(j==1){
            string temp = toUpperCase(s[1]);
            if(temp.compare("EXTREF")==0 || temp.compare("EXTDEF") == 0){
                pl.lable = s[0];
                pl.opcode = s[1];
                for(int k=i; k<line.length(); k++)
                
                        pl.operand1 += line[k];
                return pl;
            }
        }
		else if(j==2 && (s[j][0]=='C'||s[j][0]=='c')){
			boo=false;
			j-=1;
		}

	}
    for (; i < line.length() && (line[i] == ' ' || line[i] == '\t'); i++);

	if(s[0] != "" && s[0][0] == '.'){        
        for (i=0; i < line.length() && (line[i] == ' ' || line[i] == '\t'); i++);
        for (; i < line.length(); i++)
            pl.opcode += line[i];
        return pl;
    }

	if(i != line.length())
		throw new string("Too Many Arguments");
	
	

	if (s[0] == "")
		return pl;
	else if (s[1] == "")
		pl.opcode = s[0];
	else if (s[2] == "") {
		if (needNoOperands(s[1])) {
			pl.lable = s[0];
			pl.opcode = s[1];
		} else {
			pl.opcode = s[0];
			pl.operand1 = s[1];
		}
	} else if (s[3] == "") {
		if (s[1][s[1].length() - 1] == ',' || s[2][0] == ',') {
			pl.opcode = s[0];
			pl.operand1 = s[1] + s[2];
		} else {
			pl.lable = s[0];
			pl.opcode = s[1];
			pl.operand1 = s[2];
		}
	} else if (s[4] == "") {
		if (s[2].compare(",") == 0) {
			pl.opcode = s[0];
			pl.operand1 = s[1] + s[2] + s[3];
		} else {
			pl.lable = s[0];
			pl.opcode = s[1];
			pl.operand1 = s[2] + s[3];
		}
	} else {
	    if(s[3].compare(",") != 0)
            throw new string("Too Many Arguments");
					

		pl.lable = s[0];
		pl.opcode = s[1];
		pl.operand1 = s[2] + s[3] + s[4];
	}

	s[0] = "";
	s[1] = "";
	for (i = 0; i < pl.operand1.length() && pl.operand1[i] != ','; i++)
		s[0] += pl.operand1[i];
    if(i == pl.operand1.length()-1 && pl.operand1[i] == ',')
        s[1] = ",";
    else
        for (i++; i < pl.operand1.length(); i++)
            s[1] += pl.operand1[i];
	pl.operand1 = s[0];
	pl.operand2 = s[1];
	return pl;
}


void openInitialStreams(string in, string out) {

	ipfile.open(in.c_str());
	deleteFile(out.c_str());
	opfile.open(out.c_str(), std::ios::app);
}

bool isCSect(parsedLine pl) {

	return (toUpperCase(pl.opcode).compare("CSECT") == 0) ? true : false;
}

parsedLine readAndParse() {
	string before = readLine(&ipfile);
	string x = trim(before);
	return parse(x);
}

void finish() {
	
	opfile.close();
	ipfile.close();
	deleteOpTab();
}

int main(int argc, char **argv) {

	if (argc != 2) {

		cout << "\aInvalid number of parameters" << endl;
		return 1;
	}

  printf("Assembler Started\n");
	openInitialStreams(argv[1],"listFile.txt");
	printf("Input Files is being read form %s\n", argv[1]);
	bool successfullPass1 = true;
	writeHeader(&opfile);
	mainOpTab = getOpTab();
	vector<parsedLine> currCSect;
	vector<cSect> cSects;
	parsedLine pl;

	started = false;
	ended = false;

	while (!ipfile.eof()) {
		pl = readAndParse();

		if (!isCSect(pl)) {
			currCSect.push_back(pl);
			continue;
		}

		if (isCSect(pl)) {
			cSect old = runPass1(currCSect, &started, &ended,
					&successfullPass1);
			cSects.push_back(old);
			currCSect.clear(); 
			currCSect.push_back(pl); 
			
		}

	}

	cSects.push_back(runPass1(currCSect, &started, &ended, &successfullPass1));
	currCSect.clear();

	//Write that text to a file
	for (unsigned int i = 0; i < cSects.size(); i++) {
		for (unsigned int j = 0; j < cSects[i].dataSect.size(); j++) {
			toWrite x = cSects[i].dataSect.at(j);
			if (x.type == -1) {
				writeError(&opfile, x.data);
			} else if (x.type == 0) {
				writeComment(&opfile, x.data);
			} else if (x.type == 1) {
				writeLine(&opfile, x.data);
			} else {
			}
		}
		writeBorder(&opfile);
		writeSymTab(&opfile, &cSects[i].symTab);
	}

	if (!ended) {
		
		writeError(&opfile, "The program has no END clause");
	}

	if (!successfullPass1) {
		finish();
		return 0;
	}

	runPass2(&opfile, &cSects);
	printf("Done Assembling :)\n" );
	finish();
	return 0;

}
