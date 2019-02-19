#include <math.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
// #include "boxmuller"
using namespace std;

float prior1 = 0.5;
float prior2 = 0.5;

float mu1 = 1;
float mu2 = 4;
float sd1 = 1;
float sd2 = 1;

double ranf(double max)
{
  return (((double)rand())/RAND_MAX)*max;
}

float box_muller(float m, float s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * ranf(1) - 1.0;
			x2 = 2.0 * ranf(1) - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}

vector<vector<float> > genSamples() {
	vector<vector<float> > samples;
  vector<float> s;

	for(int i = 0; i < 100000; i++) {
		s.push_back(box_muller(0, 1));
		s.push_back(box_muller(0, 1));
    // for (std::vector<float>::const_iterator i = s.begin(); i != s.end(); ++i)
    //   std::cout << *i << ' ';
    // std::cout << endl;
    s.clear();
    samples.push_back(s);
    s.push_back(box_muller(4, 1));
    s.push_back(box_muller(5, 1));
    // for (std::vector<float>::const_iterator i = s.begin(); i != s.end(); ++i)
    //   std::cout << *i << ' ';
    // std::cout << endl;
		samples.push_back(s);
    s.clear();
	}
  return samples;
}

int case1(vector<float> x, int label) {
  std::cout << x.front() << endl;
  // float w_i = ((1.0/pow(sd1,2)*mu1) * x[0]) + ((1.0/pow(sd1,2)*mu1) * x[1]);
  // float w_io = ((-1.0/2*pow(sd1,2)) * ((mu1*mu1)*2)) + log(prior1);
  // float g1 = w_i + w_io;
  // w_i = ((1.0/pow(sd2,2)*mu2) * x[0]) + ((1.0/pow(sd2,2)*mu2) * x[1]);
  // w_io = ((-1.0/2*pow(sd2,2)) * ((mu2*mu2)*2)) + log(prior2);
  // float g2 = w_i + w_io;
  //
  // if((g1 > g2 && label == 1) || (g2 > g1 && label == 2)) {
  //   return 0;
  // }
  // else {
  //   return 1;
  // }
}

int main() {
  vector<vector<float> > samples = genSamples();
  vector<float> v = samples.front();
  std::cout << v.front();

  int pred1 = 0;
  int pred2 = 0;

  for(int i=0; i < 100000; i++) {
    //std::cout << samples.front().front() << endl;
    // pred1 += case1(samples[i], 1);
    // pred2 += case1(samples[i+1], 2);
  }
}
