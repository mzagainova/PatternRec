#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <boxmuller.h>
#include <commonFunction.h>
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



int main() {
  int meanx1 = 1;
  int meany1 = 1;
  int meanx2 = 4;
  int meany2 = 4;
  int stdx1 = 1;
  int stdy1 = 1;
  int stdx2 = 1;
  int stdy2 = 1;

  vector<vector<float> > samples = genSamples(meanx1, meany1, meanx2, meany2, stdx1, stdy1, stdx2, stdy2); //generates gaussian
  vector<float> v = samples.front(); //takes first element of vector

  int pred1 = 0;
  int pred2 = 0;

  vector< vector<float> >::iterator row;
  for (row = samples.begin(); row != samples.end(); row++) {
    pred1 += case1(*row, 1, 1, 4, 1, 1, 0.5, 0.5); // send distributions and prob
    row++;//data point, label, mu1, mu2, sd1, sd2, prior1, prior2
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
