#include <Eigen>
#include <iostream>
#include <fstream>

#include "image.h"

using namespace Eigen;
using namespace std;

/* external functions */
int readImage(char fname[], ImageType& image);
int readImageHeader(char fname[], int& N, int& M, int& Q, bool& type);

int main() {
	char[] name = "test.pgm";
	int rows, cols, levels;
	bool type;

	readImageHeader(name, rows, cols, levels, type);
	image = ImageType(rows, cols, levels);

	readImage(name, image);
}
