/* boxmuller.c           Implements the Polar form of the Box-Muller
                         Transformation

                      (c) Copyright 1994, Everett F. Carter Jr.
                          Permission is granted by the author to use
			  this software for any application provided this
			  copyright notice is preserved.

*/
#ifndef BOXMULLER
#define BOXMULLER

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
using namespace std;

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

int main() {
  vector<vector<float> > samples = genSamples();
}

#endif
