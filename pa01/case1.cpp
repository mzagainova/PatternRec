#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <boxmuller.h>
using namespace std;

int case1(vector<float> x, int label, float mu1, float mu2,
  float sd1, float sd2, float prior1, float prior2)
{
  float w_i = ((1.0/pow(sd1,2)*mu1) * x[0]) + ((1.0/pow(sd1,2)*mu1) * x[1]);
  float w_io = ((-1.0/2.0*pow(sd1,2)) * ((mu1*mu1)*2.0)) + log(prior1);
  float g1 = w_i + w_io;
  w_i = ((1.0/pow(sd2,2)*mu2) * x[0]) + ((1.0/pow(sd2,2)*mu2) * x[1]);
  w_io = ((-1.0/2.0*pow(sd2,2)) * ((mu2*mu2)*2.0)) + log(prior2);
  float g2 = w_i + w_io;

  if((g1 > g2 && label == 1) || (g2 > g1 && label == 2)) {
    return 0;
  }
  else {
    return 1;
  }
}

float errorBound(float B, float mu1, float mu2, float sd1, float sd2) {
  float kb = (B*(1-B))/2.0;
	kb *= (mu1 - mu2) * (1.0/((1-B)*sd1 + (B)*sd2)) * (mu1-mu2);
	kb += 0.5 * log( ((1-B)*sd1 + (B)*sd2) / (pow(sd1, 1-B) * pow(sd2, B)));

  return exp(-1.0*kb);
}

vector<float> calcChernoff(float mu1, float mu2, float sd1, float sd2) {
  float beta = 0.0;
  float cherBound = errorBound(0.0, mu1, mu2, sd1, sd2);
  float cherBoundNew = 0.0;

  fstream fout;
  fout.open("cherBound.csv", ios::out | ios::app);

  for(float i = 0.000001; i <= 1.0; i += 0.00001) {
    cherBoundNew = errorBound(i, mu1, mu2, sd1, sd2);
    fout << i << ",";
    fout << cherBoundNew << "\n";
    if(cherBoundNew < cherBound) {
      beta = i;
      cherBound = cherBoundNew;
    }
  }
  vector<float> pair;
  pair.push_back(beta);
  pair.push_back(cherBound);
  fout.close();
  return pair;
}

float calcBhattach(float mu1, float mu2, float sd1, float sd2) {
  return errorBound(0.5, mu1, mu2, sd1, sd2);
}

int main() {
  vector<vector<float> > samples = genSamples();
  vector<float> v = samples.front();

  int pred1 = 0;
  int pred2 = 0;

  vector< vector<float> >::iterator row;
  for (row = samples.begin(); row != samples.end(); row++) {
    pred1 += case1(*row, 1, 1, 4, 1, 1, 0.5, 0.5);
    row++;
    pred2 += case1(*row, 2, 1, 4, 1, 1, 0.5, 0.5);
  }
  cout << "=======================================================================\n";
  cout << "EQUAL PROBS MISCLASSIFICATIONS (P(w1) = 0.5, P(w2) = 0.5): " << endl;
  cout << "w1: " << pred1 << endl;
  cout << "w2: " << pred2 << endl;
  cout << "TOTAL: " << pred1+pred2 << endl;
  cout << "=======================================================================\n";

  pred1, pred2 = 0;
  for (row = samples.begin(); row != samples.end(); row++) {
    pred1 += case1(*row, 1, 1, 4, 1, 1, 0.2, 0.8);
    row++;
    pred2 += case1(*row, 2, 1, 4, 1, 1, 0.2, 0.8);
  }
  cout << "DIFF PROBS MISCLASSIFICATIONS (P(w1) = 0.2, P(w2) = 0.8): " << endl;
  cout << "w1: " << pred1 << endl;
  cout << "w2: " << pred2 << endl;
  cout << "TOTAL: " << pred1+pred2 << endl;
  cout << "=======================================================================\n";

  vector<float> cherBound = calcChernoff(1, 4, 1, 1);
  cout << "Cher Bound beta: " << cherBound[0] << endl;
  cout << "Cher Bound CherBound: " << cherBound[1] << endl;
  cout << "Bhattacharyya bound: " << calcBhattach(1, 4, 1, 1) << endl;
  cout << "=======================================================================\n";

}
