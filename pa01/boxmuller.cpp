/* boxmuller.c           Implements the Polar form of the Box-Muller
                         Transformation

                      (c) Copyright 1994, Everett F. Carter Jr.
                          Permission is granted by the author to use
			  this software for any application provided this
			  copyright notice is preserved.

*/
#include "boxmuller.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
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

  // fstream fout;
  // fout.open("data.csv", ios::out | ios::app);

	for(int i = 0; i < 100000; i++) {
		s.push_back(box_muller(1, 1));
    // fout << s[0] << ",";
		s.push_back(box_muller(1, 1));
    // fout << s[1] << ",";
    samples.push_back(s);
    s.clear();
    s.push_back(box_muller(4, 1));
    // fout << s[0] << ",";
    s.push_back(box_muller(4, 1));
    // fout << s[1] << "\n";
		samples.push_back(s);
    s.clear();
	}
  // fout.close();
  return samples;
}

// int main() {
//   vector<vector<float> > samples = genSamples();
//   vector< vector<float> >::iterator row;
//   vector<float>::iterator col;
//   for (row = samples.begin(); row != samples.end(); row++) {
//     for (col = row->begin(); col != row->end(); col++) {
//         cout << *col << " ";
//     }
//     cout << endl;
//   }
// }
