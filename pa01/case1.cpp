#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <boxmuller.h>
#include <commonFunction.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <numeric>
using namespace std;


int* genRandomNums();
int case3(vector<float> x, int label, float mu1[2][1], float mu2[2][1], float e1[2][2], float e2[2][2], float prior1, float prior2, int& incorrect1, int& incorrect2);
void calcInverse(float ein[][2], float e[2][2], float eDet );
float calcDeterminant(float e[2][2]);
void calcW(float ein[2][2], float einHalved[][2]);
void calcw(float ein[2][2], float mu[2][1], float combined[][1]);
float calcwi0(float ein[2][2], float mu[2][1], float e[2][2], float prior);
vector<float> calcCovariance(vector<float> sampleMean, vector< vector<float> > data);
vector<float> calcMean(vector< vector<float> > data);

int main() {
  int mx1 = 1;
  int my1 = 1;
  int mx2 = 4;
  int my2 = 4;
  int sdx1 = 1;
  int sdy1 = 1;
  int sdx2 = 1;
  int sdy2 = 1;

  vector<vector<float> > samples = genSamples(mx1, my1, mx2, my2, sdx1, sdy1, sdx2, sdy2); //generates gaussian
  vector<float> v = samples.front(); //takes first element of vector

  vector< vector<float> > samples1;
  vector< vector<float> > samples2;

  // separate samples into two separate vectors
  for (vector< vector<float> >::iterator row = samples.begin(); row != samples.end(); row++) {
    samples1.push_back(*row);
    row++;//data point, label, mu1, mu2, sd1, sd2, prior1, prior2
    samples2.push_back(*row);
  }

  vector<float> means1 = calcMean(samples1);
  vector<float> means2 = calcMean(samples2);
  vector<float> cov1 = calcCovariance(means1, samples1);
  vector<float> cov2 = calcCovariance(means2, samples2);

  float e1[2][2] = {//row by col
    {cov1[0], 0},
    {0, cov1[1]}
  };

  float e2[2][2] = {
    {cov2[0], 0},
    {0, cov2[1]}
  };

  float mu1[2][1] = { //row by col
    {means1[0]},
    {means1[1]}
  };

  float mu2[2][1] = {
    {means2[0]},
    {means2[0]}
  };

  float meanx1 = mu1[0][0];
  float meany1 = mu1[1][0];
  float meanx2 = mu2[0][0];
  float meany2 = mu2[1][0];
  float stdx1 = sqrt(e1[0][0]);
  float stdy1 = sqrt(e1[1][1]);
  float stdx2 = sqrt(e2[0][0]);
  float stdy2 = sqrt(e2[1][1]);
  int incorrect1 = 0;
  int incorrect2 = 0;

  for (vector< vector<float> >::iterator row = samples.begin(); row != samples.end(); row++)
  {
    case3(*row, 1, mu1, mu2, e1, e2, .5, .5, incorrect1, incorrect2); //error
    row++;
    case3(*row, 2, mu1, mu2, e1, e2, .5, .5, incorrect1, incorrect2);
  }

  cout << "=======================================================================\n";
  cout << "EQUAL PROBS MISCLASSIFICATIONS (P(w1) = 0.5, P(w2) = 0.5): " << endl;
  cout << "w1: " << incorrect1 << endl;
  cout << "w2: " << incorrect2 << endl;
  cout << "TOTAL: " << incorrect1+incorrect2 << endl;
  cout << "=======================================================================\n";

  /****************************************************************************************/
  // REPEAT WITH 1/100 of SAMPLES

  int *randNums;
  randNums = genRandomNums();

  vector< vector<float> > randSamples1;
  vector< vector<float> > randSamples2;
  std::vector<int>::size_type j;
  vector<float> row1, row2;

  for(int i = 0; i < 1000; i++) {
    j = (std::vector<int>::size_type)*(randNums+i);
    row1 = samples1[j];
    randSamples1.push_back(row1);
    row2 = samples2[j];
    randSamples2.push_back(row2);
  }

  means1 = calcMean(randSamples1);
  means2 = calcMean(randSamples2);
  cov1 = calcCovariance(means1, randSamples1);
  cov2 = calcCovariance(means2, randSamples2);

  float e1_2[2][2] = {//row by col
    {cov1[0], 0},
    {0, cov1[1]}
  };

  float e2_2[2][2] = {
    {cov2[0], 0},
    {0, cov2[1]}
  };

  float mu1_2[2][1] = { //row by col
    {means1[0]},
    {means1[1]}
  };

  float mu2_2[2][1] = {
    {means2[0]},
    {means2[0]}
  };

  meanx1 = mu1_2[0][0];
  meany1 = mu1_2[1][0];
  meanx2 = mu2_2[0][0];
  meany2 = mu2_2[1][0];
  stdx1 = sqrt(e1_2[0][0]);
  stdy1 = sqrt(e1_2[1][1]);
  stdx2 = sqrt(e2_2[0][0]);
  stdy2 = sqrt(e2_2[1][1]);
  incorrect1 = 0;
  incorrect2 = 0;

  for (vector< vector<float> >::iterator row = samples.begin(); row != samples.end(); row++)
  {
      case3(*row, 1, mu1_2, mu2_2, e1_2, e2_2, .5, .5, incorrect1, incorrect2); //error
      row++;
      case3(*row, 2, mu1_2, mu2_2, e1_2, e2_2, .5, .5, incorrect1, incorrect2);
  }

  cout << "=======================================================================\n";
  cout << "EQUAL PROBS MISCLASSIFICATIONS w/ 1/100 SAMPLES (P(w1) = 0.5, P(w2) = 0.5): " << endl;
  cout << "w1: " << incorrect1 << endl;
  cout << "w2: " << incorrect2 << endl;
  cout << "TOTAL: " << incorrect1+incorrect2 << endl;
  cout << "=======================================================================\n";

  // /****************************************************************************************/
  // /* DIFFERENT SD's CASE */
  // /****************************************************************************************/

  mx1 = 1;
  my1 = 1;
  mx2 = 4;
  my2 = 4;
  sdx1 = 1;
  sdy1 = 1;
  sdx2 = 4;
  sdy2 = 8;
  samples = genSamples(mx1, my1, mx2, my2, sdx1, sdy1, sdx2, sdy2);

  samples1.clear();
  samples2.clear();

  // separate samples into two separate vectors
  for (vector< vector<float> >::iterator row = samples.begin(); row != samples.end(); row++) {
    samples1.push_back(*row);
    row++;//data point, label, mu1, mu2, sd1, sd2, prior1, prior2
    samples2.push_back(*row);
  }

  means1 = calcMean(samples1);
  means2 = calcMean(samples2);
  cov1 = calcCovariance(means1, samples1);
  cov2 = calcCovariance(means2, samples2);

  float e1_3[2][2] = {//row by col
    {cov1[0], 0},
    {0, cov1[1]}
  };

  float e2_3[2][2] = {
    {cov2[0], 0},
    {0, cov2[1]}
  };

  float mu1_3[2][1] = { //row by col
    {means1[0]},
    {means1[1]}
  };

  float mu2_3[2][1] = {
    {means2[0]},
    {means2[0]}
  };

  meanx1 = mu1_3[0][0];
  meany1 = mu1_3[1][0];
  meanx2 = mu2_3[0][0];
  meany2 = mu2_3[1][0];
  stdx1 = sqrt(e1_3[0][0]);
  stdy1 = sqrt(e1_3[1][1]);
  stdx2 = sqrt(e2_3[0][0]);
  stdy2 = sqrt(e2_3[1][1]);
  incorrect1 = 0;
  incorrect2 = 0;

  for (vector< vector<float> >::iterator row = samples.begin(); row != samples.end(); row++)
  {
    case3(*row, 1, mu1_3, mu2_3, e1_3, e2_3, .5, .5, incorrect1, incorrect2); //error
    row++;
    case3(*row, 2, mu1_3, mu2_3, e1_3, e2_3, .5, .5, incorrect1, incorrect2);
  }

  cout << "=======================================================================\n";
  cout << "EQUAL PROBS MISCLASSIFICATIONS (P(w1) = 0.5, P(w2) = 0.5): " << endl;
  cout << "w1: " << incorrect1 << endl;
  cout << "w2: " << incorrect2 << endl;
  cout << "TOTAL: " << incorrect1+incorrect2 << endl;
  cout << "=======================================================================\n";

  /****************************************************************************************/
  // REPEAT WITH 1/100 of SAMPLES

  randNums = genRandomNums();

  randSamples1.clear();
  randSamples2.clear();

  for(int i = 0; i < 1000; i++) {
    j = (std::vector<int>::size_type)*(randNums+i);
    row1 = samples1[j];
    randSamples1.push_back(row1);
    row2 = samples2[j];
    randSamples2.push_back(row2);
  }

  means1 = calcMean(randSamples1);
  means2 = calcMean(randSamples2);
  cov1 = calcCovariance(means1, randSamples1);
  cov2 = calcCovariance(means2, randSamples2);

  float e1_4[2][2] = {//row by col
    {cov1[0], 0},
    {0, cov1[1]}
  };

  float e2_4[2][2] = {
    {cov2[0], 0},
    {0, cov2[1]}
  };

  float mu1_4[2][1] = { //row by col
    {means1[0]},
    {means1[1]}
  };

  float mu2_4[2][1] = {
    {means2[0]},
    {means2[0]}
  };

  meanx1 = mu1_4[0][0];
  meany1 = mu1_4[1][0];
  meanx2 = mu2_4[0][0];
  meany2 = mu2_4[1][0];
  stdx1 = sqrt(e1_4[0][0]);
  stdy1 = sqrt(e1_4[1][1]);
  stdx2 = sqrt(e2_4[0][0]);
  stdy2 = sqrt(e2_4[1][1]);
  incorrect1 = 0;
  incorrect2 = 0;

  for (vector< vector<float> >::iterator row = samples.begin(); row != samples.end(); row++)
  {
    case3(*row, 1, mu1_4, mu2_4, e1_4, e2_4, .5, .5, incorrect1, incorrect2); //error
    row++;
    case3(*row, 2, mu1_4, mu2_4, e1_4, e2_4, .5, .5, incorrect1, incorrect2);
  }

  cout << "=======================================================================\n";
  cout << "EQUAL PROBS MISCLASSIFICATIONS w/ 1/100 SAMPLES (P(w1) = 0.5, P(w2) = 0.5): " << endl;
  cout << "w1: " << incorrect1 << endl;
  cout << "w2: " << incorrect2 << endl;
  cout << "TOTAL: " << incorrect1+incorrect2 << endl;
  cout << "=======================================================================\n";

}

int* genRandomNums() {
  const std::size_t SZ = 100000;
  const std::size_t N = 1000;

  int KenoArray[SZ] ;
  std::iota( KenoArray, KenoArray+SZ, 1 ) ; // fill with numbers 1 2 ... SZ

  std::srand( std::time(0) ) ; // seed the rng

  // randomly shuffle elements in the array
  std::random_shuffle( KenoArray, KenoArray+SZ ) ;
  int randNums[N];

  // pick the first N elements in the array for N unique random numbers in [1,SZ]
  for( std::size_t i = 0 ; i < N ; ++i ){
    randNums[i] = KenoArray[i];
  }
  return randNums;
}

int case3(vector<float> x, int label, float mu1[2][1], float mu2[2][1], float e1[2][2], float e2[2][2], float prior1, float prior2, int& incorrect1, int& incorrect2)
{
	float ein1[2][2];
	float einHalved1[2][2];
	float combined1[2][1];
	float ein2[2][2];
	float einHalved2[2][2];
	float combined2[2][1];
	float x1 = x[0];
	float x2 = x[1];


	float eDet1 = calcDeterminant(e1);
	float eDet2 = calcDeterminant(e2);
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

vector<float> calcCovariance(vector<float> sampleMean, vector< vector<float> > data) {
  std::vector<int>::size_type i;
  float sumCovX = 0.00;
  float sumCovY = 0.00;
  vector<float> cov;

  for(i = 0; i != data.size(); i++) {
    sumCovX += pow((data[i][0] - sampleMean[0]), 2);
    sumCovY += pow((data[i][1] - sampleMean[1]), 2);
  }
  cov.push_back(sumCovX / float(i));
  cov.push_back(sumCovY / float(i));
  return cov;
}

vector<float> calcMean(vector< vector<float> > data) {
  float sumX = 0.00;
  float sumY = 0.00;
  std::vector<int>::size_type i;

  for(i = 0; i != data.size(); i++) {
      sumX += data[i][0];
      sumY += data[i][1];
  }

  vector<float> means;
  means.push_back(sumX / float(i));
  means.push_back(sumY / float(i));
  //cout << "MEAN: " << means[0] << " " << means[1];
  return means;
}

float calcDeterminant(float e[2][2])
{
	float a = e[0][0];
	float b = e[0][1];
	float c = e[1][0];
	float d = e[1][1];

	float determinant = (a*d)-(b*c);

	return determinant;
}

void calcInverse(float ein[][2], float e[2][2], float eDet )
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

void calcw(float ein[2][2], float mu[2][1], float combined[][1])
{
	combined[0][0] = (ein[0][0] * mu[0][0]) + (ein[0][1] * mu[1][0]);
	combined[1][0] = (ein[1][0] * mu[0][0]) + (ein[1][1] * mu[1][0]);
	//cout << combined[1][0] << endl;
}

float calcwi0(float ein[2][2], float mu[2][1], float e[2][2], float prior)
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
