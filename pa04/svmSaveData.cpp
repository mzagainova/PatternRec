#include <iostream>
#include <vector>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void saveNewTrainingFile(char* fileName, int i) {
	string inFile = "genderdata/48_60/";
	string curData = "";
	char curLabel[2];
	ifstream finData, finLabel;
	istringstream iss;
	char outfileName[256];
	sprintf(outfileName, "48_60_0%i.txt", i);
	ofstream fout(outfileName);
	float curFloat = 0.0;
	int counter = 1, numEig = 0;

	inFile += fileName;
	cout << inFile << endl;
	finData.open(inFile.c_str());
	inFile = "genderdata/48_60/T";
	inFile += fileName;
	finLabel.open(inFile.c_str());

	cout << getline(finData, curData) << endl;

	while(numEig < 30 && getline(finData, curData) && finLabel.get(curLabel, 2)) {
		cout << "currLabel " << curLabel << "..." << endl;
		fout << curLabel;
		finLabel.get(curLabel, 2);
		iss.clear();
		iss.str(curData);
		counter = 1;
		while(iss >> curFloat) {
			fout << " " << counter << ":" << curFloat;
			cout << " " << counter << ":" << curFloat;
			counter++;
		}
		fout << endl;
		cout << "\n";
		numEig++;
	}
	finData.close();
	finLabel.close();
	fout.close();
}

void saveNewTestFile(char* fileName, int i, string val) {
	string inFile = "genderdata/48_60/";
	string curData = "";
	char curLabel[2];
	ifstream finData, finLabel;
	istringstream iss;
	char outfileName[256];
	sprintf(outfileName, "48_60_0%i%s_test.txt", i, val.c_str());
	ofstream fout(outfileName);
	float curFloat = 0.0;
	int counter = 1;

	inFile += fileName;
	finData.open(inFile.c_str());
	inFile = "genderdata/48_60/T";
	inFile += fileName;
	finLabel.open(inFile.c_str());

	while(getline(finData, curData) && finLabel.get(curLabel, 2)) {
		fout << curLabel;
		finLabel.get(curLabel, 2);
		iss.clear();
		iss.str(curData);
		counter = 1;
		while(iss >> curFloat) {
			fout << " " << counter << ":" << curFloat;
			counter++;
		}
		fout << endl;
	}
	finData.close();
	finLabel.close();
	fout.close();

}

int main()
{
	char fileName[256];
	string v = "val";
	string ts = "";
	for(int i = 1; i <= 3; i++) {
		sprintf(fileName, "trPCA_0%i.txt", i);
		cout << fileName << endl;
		saveNewTrainingFile(fileName, i);
		sprintf(fileName, "tsPCA_0%i.txt", i);
		saveNewTestFile(fileName, i, ts);
		sprintf(fileName, "valPCA_0%i.txt", i);
		saveNewTestFile(fileName, i, v);
	}
	return 0;
}
