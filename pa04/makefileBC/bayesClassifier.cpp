#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <math.h>

using namespace std;


void readFiles(ifstream&, ifstream&, ifstream&, ifstream&, ifstream&, ifstream&, vector<float>& , vector<float>&, vector<float>&, vector<float>&);
void calculateMean(vector<float>, vector<float>&);
void calculateCovariance(vector<float>, vector<float>, vector<float>&);
double calculateFrac(vector<float>);
void calculateInverse(vector<float>& , vector<float>);
float calculateGX(vector<float> , vector<float> , vector<float> , double);


int main()
{
	ifstream train1;
	train1.open("16_20/trPCA_01.txt");
	ifstream train1label;
	train1label.open("16_20/TtrPCA_01.txt");
	ifstream val1;
	val1.open("16_20/valPCA_01.txt");
	ifstream val1label;
	val1label.open("16_20/TvalPCA_01.txt");
	ifstream test1;
	test1.open("16_20/tsPCA_01.txt");
	ifstream test1label;
	test1label.open("16_20/TtsPCA_01.txt");
	vector<float> male;
	vector<float> female;
	vector<float> maleT;
	vector<float> femaleT;
	vector<float> meanM;
	vector<float> meanF;
	vector<float> covarM;
	vector<float> covarF;
	vector<float> invCovarM;
	vector<float> invCovarF;
	vector<float> femaleAcc;
	vector<float> maleAcc;
	
	readFiles(train1, train1label, val1, val1label, test1, test1label, male, female, maleT, femaleT);
	calculateMean(male, meanM);
	calculateCovariance(male, meanM, covarM);
	double fracM = calculateFrac(covarM);
	calculateInverse(invCovarM, covarM);
	calculateMean(female, meanF);
	calculateCovariance(female, meanF, covarF);
	double fracF = calculateFrac(covarF);
	calculateInverse(invCovarF, covarF);


	int c = 0;
	vector<float> tempM;
	int counter = 0;
	for (int i = 0; i < maleT.size(); i++)
	{
		
		tempM.push_back(maleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempM, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempM, meanF, invCovarF, fracF);
			
			if (gxM > gxF)
			{
				counter ++;
			}
			tempM.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy male 16 x 20 " << float(float(counter) / float(maleT.size()/30)) * 100.00 << " Size " << maleT.size()/30 << endl << endl;
	maleAcc.push_back( float(float(counter) / float(maleT.size()/30)) * 100.00);

	c = 0;
	vector<float> tempF;
	counter = 0;
	for (int i = 0; i < femaleT.size(); i++)
	{
		
		tempF.push_back(femaleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempF, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempF, meanF, invCovarF, fracF);
			
			if (gxF > gxM)
			{
				counter ++;
			}
			tempF.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy female 16 x 20 " << float(float(counter) / float(femaleT.size()/30)) * 100.00 << " Size " << femaleT.size()/30 << endl << endl;
	femaleAcc.push_back(float(float(counter) / float(femaleT.size()/30)) * 100.00);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ifstream train2;
	train2.open("16_20/trPCA_02.txt");
	ifstream train2label;
	train2label.open("16_20/TtrPCA_02.txt");
	ifstream val2;
	val2.open("16_20/valPCA_02.txt");
	ifstream val2label;
	val2label.open("16_20/TvalPCA_02.txt");
	ifstream test2;
	test2.open("16_20/tsPCA_02.txt");
	ifstream test2label;
	test2label.open("16_20/TtsPCA_02.txt");
	
	 male.clear();
	 female.clear();
	 maleT.clear();
	 femaleT.clear();
	 meanM.clear();
	 meanF.clear();
	 covarM.clear();
	 covarF.clear();
	 invCovarM.clear();
	 invCovarF.clear();
	

	readFiles(train2, train2label, val2, val2label, test2, test2label, male, female, maleT, femaleT);
	
	
	
	calculateMean(male, meanM);
	calculateCovariance(male, meanM, covarM);
	fracM = calculateFrac(covarM);
	calculateInverse(invCovarM, covarM);
	calculateMean(female, meanF);
	calculateCovariance(female, meanF, covarF);
	fracF = calculateFrac(covarF);
	calculateInverse(invCovarF, covarF);


	c = 0;
	tempM.clear();
	counter = 0;
	for (int i = 0; i < maleT.size(); i++)
	{
		
		tempM.push_back(maleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempM, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempM, meanF, invCovarF, fracF);
		
			if (gxM > gxF)
			{
				counter ++;
			}
			tempM.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy male 16 x 20 " << float(float(counter) / float(maleT.size()/30)) * 100.00 << " Size " << maleT.size()/30 << endl << endl;
	maleAcc.push_back(float(float(counter) / float(maleT.size()/30)) * 100.00);
	c = 0;
	tempF.clear();
	counter = 0;
	for (int i = 0; i < femaleT.size(); i++)
	{
		
		tempF.push_back(femaleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempF, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempF, meanF, invCovarF, fracF);
			//cout  << "male score: " << gxM << " female score " << gxF << endl;
			if (gxF > gxM)
			{
				counter ++;
			}
			tempF.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy female 16 x 20 " << float(float(counter) / float(femaleT.size()/30)) * 100.00 << " Size " << femaleT.size()/30 << endl << endl;
	femaleAcc.push_back(float(float(counter) / float(femaleT.size()/30)) * 100.00);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	ifstream train3;
	train3.open("16_20/trPCA_03.txt");
	ifstream train3label;
	train3label.open("16_20/TtrPCA_03.txt");
	ifstream val3;
	val3.open("16_20/valPCA_03.txt");
	ifstream val3label;
	val3label.open("16_20/TvalPCA_03.txt");
	ifstream test3;
	test3.open("16_20/tsPCA_03.txt");
	ifstream test3label;
	test1label.open("16_20/TtsPCA_03.txt");

	 male.clear();
	 female.clear();
	 maleT.clear();
	 femaleT.clear();
	 meanM.clear();
	 meanF.clear();
	 covarM.clear();
	 covarF.clear();
	 invCovarM.clear();
	 invCovarF.clear();
	readFiles(train3, train3label, val3, val3label, test3, test3label, male, female, maleT, femaleT);
	
	
	
	calculateMean(male, meanM);
	calculateCovariance(male, meanM, covarM);
	fracM = calculateFrac(covarM);
	calculateInverse(invCovarM, covarM);
	calculateMean(female, meanF);
	calculateCovariance(female, meanF, covarF);
	fracF = calculateFrac(covarF);
	calculateInverse(invCovarF, covarF);


	 c = 0;
	tempM.clear();
	counter = 0;
	for (int i = 0; i < maleT.size(); i++)
	{
		
		tempM.push_back(maleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempM, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempM, meanF, invCovarF, fracF);
			
			if (gxM > gxF)
			{
				counter ++;
			}
			tempM.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy male 16 x 20 " << float(float(counter) / float(maleT.size()/30)) * 100.00 << " Size " << maleT.size()/30 << endl << endl;
	maleAcc.push_back(float(float(counter) / float(maleT.size()/30)) * 100.00);
	c = 0;
	tempF.clear();
	counter = 0;
	for (int i = 0; i < femaleT.size(); i++)
	{
		
		tempF.push_back(femaleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempF, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempF, meanF, invCovarF, fracF);
			//cout  << "male score: " << gxM << " female score " << gxF << endl;
			if (gxF > gxM)
			{
				counter ++;
			}
			tempF.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy female 16 x 20 " << float(float(counter) / float(femaleT.size()/30)) * 100.00 << " Size " << femaleT.size()/30 << endl << endl;
	femaleAcc.push_back(float(float(counter) / float(femaleT.size()/30)) * 100.00);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float mSum = 0;
	float fSum = 0;
	for (int j = 0; j < maleAcc.size(); j++)
	{
		mSum += maleAcc.at(j);
		fSum += femaleAcc.at(j);
	}

	float fAcc = float(fSum)/ float(femaleAcc.size());
	float mAcc = float(mSum) / float(maleAcc.size());
	cout << "3-fold male accuracy " << mAcc << " 3-fold female accuracy " << fAcc << endl;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ifstream train1b;
	train1b.open("48_60/trPCA_01.txt");
	ifstream train1labelb;
	train1labelb.open("48_60/TtrPCA_01.txt");
	ifstream val1b;
	val1b.open("48_60/valPCA_01.txt");
	ifstream val1labelb;
	val1labelb.open("48_60/TvalPCA_01.txt");
	ifstream test1b;
	test1b.open("48_60/tsPCA_01.txt");
	ifstream test1labelb;
	test1labelb.open("48_60/TtsPCA_01.txt");
	 male.clear();
	 female.clear();
	 maleT.clear();
	 femaleT.clear();
	 meanM.clear();
	 meanF.clear();
	 covarM.clear();
	 covarF.clear();
	 invCovarM.clear();
	 invCovarF.clear();
	
	readFiles(train1b, train1labelb, val1b, val1labelb, test1b, test1labelb, male, female, maleT, femaleT);
	calculateMean(male, meanM);
	calculateCovariance(male, meanM, covarM);
	fracM = calculateFrac(covarM);
	calculateInverse(invCovarM, covarM);
	calculateMean(female, meanF);
	calculateCovariance(female, meanF, covarF);
	fracF = calculateFrac(covarF);
	calculateInverse(invCovarF, covarF);


	c = 0;
	 tempM;
	counter = 0;
	for (int i = 0; i < maleT.size(); i++)
	{
		
		tempM.push_back(maleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempM, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempM, meanF, invCovarF, fracF);
			
			if (gxM > gxF)
			{
				counter ++;
			}
			tempM.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy male 48_60 " << float(float(counter) / float(maleT.size()/30)) * 100.00 << " Size " << maleT.size()/30 << endl << endl;
	maleAcc.push_back( float(float(counter) / float(maleT.size()/30)) * 100.00);

	c = 0;
	tempF;
	counter = 0;
	for (int i = 0; i < femaleT.size(); i++)
	{
		
		tempF.push_back(femaleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempF, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempF, meanF, invCovarF, fracF);
			
			if (gxF > gxM)
			{
				counter ++;
			}
			tempF.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy female 48_60 " << float(float(counter) / float(femaleT.size()/30)) * 100.00 << " Size " << femaleT.size()/30 << endl << endl;
	femaleAcc.push_back(float(float(counter) / float(femaleT.size()/30)) * 100.00);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ifstream train2b;
	train2b.open("48_60/trPCA_02.txt");
	ifstream train2labelb;
	train2labelb.open("48_60/TtrPCA_02.txt");
	ifstream val2b;
	val2b.open("48_60/valPCA_02.txt");
	ifstream val2labelb;
	val2labelb.open("48_60/TvalPCA_02.txt");
	ifstream test2b;
	test2b.open("48_60/tsPCA_02.txt");
	ifstream test2labelb;
	test2labelb.open("48_60/TtsPCA_02.txt");
	
	 male.clear();
	 female.clear();
	 maleT.clear();
	 femaleT.clear();
	 meanM.clear();
	 meanF.clear();
	 covarM.clear();
	 covarF.clear();
	 invCovarM.clear();
	 invCovarF.clear();
	

	readFiles(train2b, train2labelb, val2b, val2labelb, test2b, test2labelb, male, female, maleT, femaleT);
	
	
	
	calculateMean(male, meanM);
	calculateCovariance(male, meanM, covarM);
	fracM = calculateFrac(covarM);
	calculateInverse(invCovarM, covarM);
	calculateMean(female, meanF);
	calculateCovariance(female, meanF, covarF);
	fracF = calculateFrac(covarF);
	calculateInverse(invCovarF, covarF);


	c = 0;
	tempM.clear();
	counter = 0;
	for (int i = 0; i < maleT.size(); i++)
	{
		
		tempM.push_back(maleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempM, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempM, meanF, invCovarF, fracF);
		
			if (gxM > gxF)
			{
				counter ++;
			}
			tempM.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy male 48_60 " << float(float(counter) / float(maleT.size()/30)) * 100.00 << " Size " << maleT.size()/30 << endl << endl;
	maleAcc.push_back(float(float(counter) / float(maleT.size()/30)) * 100.00);
	c = 0;
	tempF.clear();
	counter = 0;
	for (int i = 0; i < femaleT.size(); i++)
	{
		
		tempF.push_back(femaleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempF, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempF, meanF, invCovarF, fracF);
			//cout  << "male score: " << gxM << " female score " << gxF << endl;
			if (gxF > gxM)
			{
				counter ++;
			}
			tempF.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy female 48_60 " << float(float(counter) / float(femaleT.size()/30)) * 100.00 << " Size " << femaleT.size()/30 << endl << endl;
	femaleAcc.push_back(float(float(counter) / float(femaleT.size()/30)) * 100.00);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	ifstream train3b;
	train3b.open("48_60/trPCA_03.txt");
	ifstream train3labelb;
	train3labelb.open("48_60/TtrPCA_03.txt");
	ifstream val3b;
	val3b.open("48_60/valPCA_03.txt");
	ifstream val3labelb;
	val3labelb.open("48_60/TvalPCA_03.txt");
	ifstream test3b;
	test3b.open("48_60/tsPCA_03.txt");
	ifstream test3labelb;
	test1labelb.open("48_60/TtsPCA_03.txt");
	
	 male.clear();
	 female.clear();
	 maleT.clear();
	 femaleT.clear();
	 meanM.clear();
	 meanF.clear();
	 covarM.clear();
	 covarF.clear();
	 invCovarM.clear();
	 invCovarF.clear();
	readFiles(train3b, train3labelb, val3b, val3labelb, test3b, test3labelb, male, female, maleT, femaleT);
	
	
	
	calculateMean(male, meanM);
	calculateCovariance(male, meanM, covarM);
	fracM = calculateFrac(covarM);
	calculateInverse(invCovarM, covarM);
	calculateMean(female, meanF);
	calculateCovariance(female, meanF, covarF);
	fracF = calculateFrac(covarF);
	calculateInverse(invCovarF, covarF);


	 c = 0;
	tempM.clear();
	counter = 0;
	for (int i = 0; i < maleT.size(); i++)
	{
		
		tempM.push_back(maleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempM, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempM, meanF, invCovarF, fracF);
			
			if (gxM > gxF)
			{
				counter ++;
			}
			tempM.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy male 48_60 " << float(float(counter) / float(maleT.size()/30)) * 100.00 << " Size " << maleT.size()/30 << endl << endl;
	maleAcc.push_back(float(float(counter) / float(maleT.size()/30)) * 100.00);
	c = 0;
	tempF.clear();
	counter = 0;
	for (int i = 0; i < femaleT.size(); i++)
	{
		
		tempF.push_back(femaleT.at(i));
		c++;
		if (c == 30){
			float gxM = calculateGX(tempF, meanM, invCovarM, fracM);
			float gxF = calculateGX(tempF, meanF, invCovarF, fracF);
			//cout  << "male score: " << gxM << " female score " << gxF << endl;
			if (gxF > gxM)
			{
				counter ++;
			}
			tempF.clear();
			c= 0;
		}

		
		
	}
	
	cout << "Accuracy female 48_60 " << float(float(counter) / float(femaleT.size()/30)) * 100.00 << " Size " << femaleT.size()/30 << endl << endl;
	femaleAcc.push_back(float(float(counter) / float(femaleT.size()/30)) * 100.00);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	mSum = 0;
	fSum = 0;
	for (int j = 0; j < maleAcc.size(); j++)
	{
		mSum += maleAcc.at(j);
		fSum += femaleAcc.at(j);
	}

	fAcc = float(fSum)/ float(femaleAcc.size());
	mAcc = float(mSum) / float(maleAcc.size());
	cout << "3-fold male accuracy " << mAcc << " 3-fold female accuracy " << fAcc << endl;

	
}
//2 is female
//This appears to work correctly
void readFiles(ifstream& train, ifstream& trainlabel, ifstream& val, ifstream& vallabel, ifstream& test, ifstream& testlabel, vector<float>& male, vector<float>& female, vector<float>& maleT, vector<float>& femaleT){
	
	
	float x;
	int trainCount = 0;
	vector<float> trainVec;
	vector<int> trainLabels;
	vector<float> testVec;
	vector<int> testLabels;

	//create train
	while (train >> x)
	{
		if (trainCount < 30)
		{

			trainVec.push_back(x);				
		}
		trainCount ++;
		if (trainCount == 134)
		{
			trainCount = 0;
		}
	}

	while (trainlabel >> x){
		trainLabels.push_back(x);
	}
	
	//create test
	trainCount = 0;
	while (test >> x)
	{
		if(trainCount < 30){
			testVec.push_back(x);
		}
		trainCount++;
		if(trainCount ==134)
		{
			trainCount = 0;
		}
	}

	while(testlabel >> x)
	{
		testLabels.push_back(x);
	}
	trainCount = 0;
	while (val >> x)
	{
		if(trainCount < 30){
			testVec.push_back(x);
		}
		trainCount++;
		if(trainCount ==134)
		{
			trainCount = 0;
		}
	}
	while(vallabel >>x)
	{
		testLabels.push_back(x);
	}


	//populate male and female training vectors
	int num = 0;
	for (int i= 0; i < trainLabels.size(); i++)
	{
		int val = 0;
		if (trainLabels.at(i) ==1)
		{
			while(val < 30)
			{
				male.push_back(trainVec.at(num));
				val++;
				num++;
			}
		}
		if(trainLabels.at(i) ==2)
		{
			while(val < 30)
			{
				female.push_back(trainVec.at(num));
				val++;
				num++;
			}

		}
	}

//populate male and female testing vectors
	num = 0;
	for (int i= 0; i < testLabels.size(); i++)
	{
		int val = 0;
		if (testLabels.at(i) ==1)
		{
			while(val < 30)
			{
				maleT.push_back(testVec.at(num));
				val++;
				num++;
			}
		}
		if(testLabels.at(i) ==2)
		{
			while(val < 30)
			{
				femaleT.push_back(testVec.at(num));
				val++;
				num++;
			}

		}
	}



}

void calculateMean(vector<float> trainVec, vector<float>& mean)
{
	int counter = 0;
	float sum = 0.0;


	vector<float> tempVec(30, 0);
	int c = 0;
	int val = 0;
	
	
	for (int i = 0; i < trainVec.size()/30; i++)
	{
		while (c < 30 )
		{
			int num = (i * 30) + c;
			tempVec.at(c) += trainVec.at(num);
			c++;
		}
		c= 0;
	}

	for (int j = 0; j< tempVec.size(); j++)
	{
		mean.push_back(tempVec.at(j) / (trainVec.size()/30));
	}




}

void calculateCovariance(vector<float> train, vector<float> mean, vector<float>& covar)
{

	vector<float> tempVec(30, 0);
	int c = 0;
	int val = 0;
	vector<float> temp(30, 0);
	
	
	for (int i = 0; i < train.size()/30; i++)
	{
		while (c < 30 )
		{
			int num = (i * 30) + c;
			float diff = train.at(num) - mean.at(c);
			tempVec.at(c) += diff * diff;
			c++;
		}
		c= 0;
	}

	for (int j = 0; j< tempVec.size(); j++)
	{
		float temp = tempVec.at(j) / (train.size()/30);
		covar.push_back(temp);
	}


	
}

double calculateFrac(vector<float> covar)
{
	double pi = 3.1415926;
	double dblPi = pi * 2;
	double diag = 1.0;


	for (int i = 0; i < covar.size(); i++)
	{
		
		diag *= covar.at(i);
		
	}


	return (double(1/(dblPi * pow(diag, 0.5))));
}

void calculateInverse(vector<float>& invCovar, vector<float> covar)
{
	for (int i = 0; i < covar.size(); i++)
	{
		invCovar.push_back(1/covar.at(i));
		
	}
}

float calculateGX(vector<float> test, vector<float> average, vector<float> invCovar, double frac)
{
	vector<float> temp;
	float val = 0;
	float mult = 0;
	float sum = 0;
	float pow = 0;
	float e = 0;
	float gx = 0;
	
	for (int i = 0; i < test.size(); i++)
	{
		val = test.at(i) - average.at(i);
		mult = val * invCovar.at(i); 
		mult = mult * val;
		sum += mult;

	}
	pow = -0.5 * sum; 
	e = exp(pow);
	gx = frac * e;
	
	return gx;

}
