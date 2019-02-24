#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <boxmuller.h>
#include <commonFunction.h>
//#include <case1.cpp>
using namespace std;

int case3(vector<float> x, int label, int mu1[2][1], int mu2[2][1], int e1[2][2], int e2[2][2], float prior1, float prior2, int& incorrect1, int& incorrect2);

void calcInverse(float ein[][2], int e[2][2], int eDet );

int calcDeterminant(int e[2][2]);

void calcW(float ein[2][2], float einHalved[][2]);

void calcw(float ein[2][2], int mu[2][1], float combined[][1]);

float calcwi0(float ein[2][2], int mu[2][1], int e[2][2], float prior);

int main()
{

	int e1[2][2] = {//row by col
		{1, 0},
		{0, 1}
	};

	int e2[2][2] = { 
		{4, 0},
		{0, 8}
	};

	int mu1[2][1] = { //row by col
		{1},
		{1}
	};

	int mu2[2][1] = {
		{4},
		{4}
	};

  int meanx1 = mu1[0][0];
  int meany1 = mu1[1][0];
  int meanx2 = mu2[0][0];
  int meany2 = mu2[1][0];
  float stdx1 = sqrt(e1[0][0]);
  float stdy1 = sqrt(e1[1][1]);
  float stdx2 = sqrt(e2[0][0]);
  float stdy2 = sqrt(e2[1][1]);
  int incorrect1 = 0;
  int incorrect2 = 0;

  	

	vector<vector<float>> samples = genSamples(meanx1, meany1, meanx2, meany2, stdx1, stdy1, stdx2, stdy2); //meed to change mean and std
	vector<float> v = samples.front();

	//cout << mu1[1][0] << endl;


	vector<vector<float>>::iterator row;
	for (row = samples.begin(); row != samples.end(); row++)
	{
		case3(*row, 1, mu1, mu2, e1, e2, .5, .5, incorrect1, incorrect2); //error
		row++;
		case3(*row, 2, mu1, mu2, e1, e2, .5, .5, incorrect1, incorrect2);

		//order is x, class, mu1, mu2, sd1a, sd1b, sd2a, sd2b, prior1, prior2
	}

  cout << "=======================================================================\n";
  cout << "EQUAL PROBS MISCLASSIFICATIONS (P(w1) = 0.5, P(w2) = 0.5): " << endl;
  cout << "w1: " << incorrect1 << endl;
  cout << "w2: " << incorrect2 << endl;
  cout << "TOTAL: " << incorrect1+incorrect2 << endl;
  cout << "=======================================================================\n";



  	incorrect1 = 0;
    incorrect2 = 0;
  	for (row = samples.begin(); row != samples.end(); row++)
	{
		case3(*row, 1, mu1, mu2, e1, e2, .2, .8, incorrect1, incorrect2); //error
		row++;
		case3(*row, 2, mu1, mu2, e1, e2, .2, .8, incorrect1, incorrect2);

		//order is x, class, mu1, mu2, sd1a, sd1b, sd2a, sd2b, prior1, prior2
	}

  cout << "=======================================================================\n";
  cout << "DIFF PROBS MISCLASSIFICATIONS (P(w1) = 0.2, P(w2) = 0.8): " << endl;
  cout << "w1: " << incorrect1 << endl;
  cout << "w2: " << incorrect2 << endl;
  cout << "TOTAL: " << incorrect1+incorrect2 << endl;
  cout << "=======================================================================\n";

  vector<float> cherBound = calcChernoff(1, 4, 1, 6);
  cout << "Cher Bound beta: " << cherBound[0] << endl;
  cout << "Cher Bound CherBound: " << cherBound[1] << endl;
  cout << "Bhattacharyya bound: " << calcBhattach(1, 4, 1, 6) << endl;
  cout << "=======================================================================\n";


}

int case3(vector<float> x, int label, int mu1[2][1], int mu2[2][1], int e1[2][2], int e2[2][2], float prior1, float prior2, int& incorrect1, int& incorrect2)
{
	float ein1[2][2];
	float einHalved1[2][2];
	float combined1[2][1];
	float ein2[2][2];
	float einHalved2[2][2];
	float combined2[2][1];
	float x1 = x[0];
	float x2 = x[1];



	int eDet1 = calcDeterminant(e1);
	int eDet2 = calcDeterminant(e2);
	calcInverse(ein1, e1, eDet1);
	calcInverse(ein2, e2, eDet2);

	calcW(ein1, einHalved1); //gives Wi (first term)
	calcW(ein2, einHalved2); //gives Wi (first term)

	calcw(ein1, mu1, combined1); // gives wi (second term)
	calcw(ein2, mu2, combined2); // gives wi (second term)

	float value1 = calcwi0(ein1, mu1, e1, prior1);
	float value2 = calcwi0(ein2, mu2, e2, prior2);



	float class1 = (((einHalved1[0][0] * x1 * x1)  + (einHalved1[1][1] * x2 * x2) ) + ((combined1[0][0] * x1) + (combined1[1][0] * x2))  + value1);
	float class2 = (((einHalved2[0][0] * x1 * x1)  + (einHalved2[1][1] * x2 * x2) ) + ((combined2[0][0] * x1) + (combined2[1][0] * x2))  + value2);

	if (class1 > class2)
	{
		int val = 1;
		
		if (val != label)
		{
			incorrect1+=1;
			
		}
		return 1;
	}
	if (class1 < class2)
	{
		int val= 2;
		if (val != label)
		{
			incorrect2+=1;
		}

		return 2;
	}


}

int calcDeterminant(int e[2][2])
{
	int a = e[0][0];
	int b = e[0][1];
	int c = e[1][0];
	int d = e[1][1];

	int determinant = (a*d)-(b*c);

	return determinant;
}

void calcInverse(float ein[][2], int e[2][2], int eDet )
{
	float frac = (float)1/(float)eDet;
	//cout << "frac" << frac << endl;

	ein[0][0] = e[1][1] * frac;
	ein[0][1] = e[1][0] * (-1) * frac;
	ein[1][0] = e[0][1] * (-1) * frac;
	ein[1][1] = e[0][0] * frac;
}

void calcW(float ein[2][2], float einHalved[][2])
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			einHalved[i][j] = ein[i][j] * (-.5);
		}
	}
}

void calcw(float ein[2][2], int mu[2][1], float combined[][1])
{
	combined[0][0] = (ein[0][0] * mu[0][0]) + (ein[0][1] * mu[1][0]);
	combined[1][0] = (ein[1][0] * mu[0][0]) + (ein[1][1] * mu[1][0]);
	//cout << combined[1][0] << endl;
}

float calcwi0(float ein[2][2], int mu[2][1], int e[2][2], float prior)
{
	float combomuTEin[1][2];
	float comboAll;
	float muT[1][2] = {
		{mu[0][0], mu[1][0]}
	};

	combomuTEin[0][0] = (muT[0][0] * ein[0][0]) +(muT[0][1] * ein[1][0]);
	combomuTEin[0][1] = (muT[0][0] * ein[0][1]) + (muT[0][1] * ein[1][1]);

	comboAll = (combomuTEin[0][0] * mu[0][0]) + (combomuTEin[0][1] * mu[1][0]);

	float comboAllhalf = (-.5) * comboAll;

	float natDet = (-.5) * log(calcDeterminant(e));

	float natPrior = log(prior);

	return (comboAllhalf + natDet + natPrior);


}