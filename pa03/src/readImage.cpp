#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "image.h"
using namespace std;

int readImage(char fname[], ImageType& image,int N, int M, int Q)
{
  int i, j;
  //int N, M, Q;
  unsigned char *charImage;
  char header [100], *ptr;
  ifstream ifp;

  ifp.open(fname, ios::in | ios::binary);

  if (!ifp) {
    cout << "Can't read image: " << fname << endl;
    exit(1);
  }

  // read header

  ifp.getline(header,100,'\n');
  if ( (header[0]!=80) ||    /* 'P' */
       (header[1]!=53) ) {   /* '5' */
       cout << "Image " << fname << " is not PGM" << endl;
       exit(1);
  }

  while(header[0]=='#')
    ifp.getline(header,100,'\n');

  M=strtol(header+3,&ptr,0);
  N=strtol(ptr, &ptr, 0);
  Q=strtol(ptr, &ptr, 0);

  charImage = (unsigned char *) new unsigned char [M*N];

  ifp.read( reinterpret_cast<char *>(charImage), (M*N)*sizeof(unsigned char));

  if (ifp.fail()) {
    cout << "Image " << fname << " has wrong size" << endl;
    exit(1);
  }

  ifp.close();

  //
  // Convert the unsigned characters to integers
  //

  int val;

  for(i=0; i<N; i++)
    for(j=0; j<M; j++) {
      val = (int)charImage[i*M+j];
      image.setPixelVal(i, j, val);
    }

 delete [] charImage;

}
