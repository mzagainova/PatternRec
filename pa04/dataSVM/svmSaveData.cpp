#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

using namespace std;

void saveTrainput(char* , int );
void saveTestFile(char* , int i, string);

int main() {
	char file[256];
	string v = "val";
	string ts = "";
	for(int i = 1; i <= 3; i++) {
		sprintf(file, "trPCA_0%i.txt", i);
		saveTrainput(file, i);
		sprintf(file, "tsPCA_0%i.txt", i);
		saveTestFile(file, i, ts);
		sprintf(file, "valPCA_0%i.txt", i);
		saveTestFile(file, i, v);
	}
	return 0;
}



void saveTrainput(char* file, int i) {
	string input = "genderdata/48_60/";
	string curData = "";
	char curLabel[2];
	ifstream finFeat, finLabel;
	istringstream iss;
	char outfile[256];
	sprintf(outfile, "48_60_0%i.txt", i);
	ofstream fout(outfile);
	float value = 0.0;
	int c = 1;

	input += file;
	finFeat.open(input.c_str());
	input = "genderdata/48_60/T";
	input += file;
	finLabel.open(input.c_str());
	int iter = 0;

	while(iter < 30 && getline(finFeat, curData) && finLabel.get(curLabel, 2)) {
		fout << curLabel;
		finLabel.get(curLabel, 2);
		iss.clear();
		iss.str(curData);
		c = 1;
		while(iss >> value) {
			fout << " " << c << ":" << value;
			c++;
		}
		iter++;
		fout << endl;
	}
	finFeat.close();
	finLabel.close();
	fout.close();
}

void saveTestFile(char* file, int i, string val) {
	string input = "genderdata/48_60/";
	string curData = "";
	char curLabel[2];
	ifstream finFeat, finLabel;
	istringstream iss;
	char outfile[256];
	sprintf(outfile, "48_60_0%i%s_test.txt", i, val.c_str());
	ofstream fout(outfile);
	float value = 0.0;
	int c = 1;

	input += file;
	finFeat.open(input.c_str());
	input = "genderdata/48_60/T";
	input += file;
	finLabel.open(input.c_str());
	int iter = 0;

	while(getline(finFeat, curData) && finLabel.get(curLabel, 2)) {
		fout << curLabel;
		finLabel.get(curLabel, 2);
		iss.clear();
		iss.str(curData);
		c = 1;
		while(iss >> value) {
			fout << " " << c << ":" << value;
			c++;
		}
		fout << endl;
	}
	finFeat.close();
	finLabel.close();
	fout.close();
}
