#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <boxmuller.h>
using namespace std;


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
