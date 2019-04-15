#include <eigen3/Eigen/Dense>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "image.h"
#include <sys/types.h>
#include <dirent.h>
#include <iterator>

using namespace Eigen;
using namespace std;

// change for low/high definition images
int M = 48;
int N = 60;
int Q = 255;

float PERCENT_PRES = 0.85;

typedef std::vector<std::string> stringvec;

/* external functions */
int readImage(char[], ImageType&, int, int, int);
int writeImage(char[], ImageType&);
int readImageHeader(char[], int&, int&, int&, bool& );

/* internal functions */
void read_directory(const std::string&, stringvec&);
void readFaceImgs(string, vector<pair<string, VectorXf> > &);
bool fileExists(const char*);
void writeFace(VectorXf , char[], char);
void calcEigenFaces(vector<pair<string, VectorXf> >, VectorXf&, MatrixXf&, VectorXf&, const char*);
bool readSavedFaces(VectorXf &, MatrixXf &, VectorXf &,const char*);
VectorXf projectFace(VectorXf, VectorXf, MatrixXf);
bool compare(pair<string, float>, pair<string, float>);
bool classify(pair<string, VectorXf>, vector<pair<string, VectorXf>>, vector<pair<string, VectorXf>>);

#include <sys/types.h>
#include <dirent.h>

// writing/reading binary -- from Stack overflow
namespace Eigen
{
    template<class Matrix>
    void write_bin(const char* filename, const Matrix& matrix)
    {
        std::ofstream out(filename,ios::out | ios::binary | ios::trunc);
        typename Matrix::Index rows=matrix.rows(), cols=matrix.cols();
        out.write((char*) (&rows), sizeof(typename Matrix::Index));
        out.write((char*) (&cols), sizeof(typename Matrix::Index));
        out.write((char*) matrix.data(), rows*cols*sizeof(typename Matrix::Scalar) );
        out.close();
    }

    template<class Matrix>
    void read_bin(const char* filename, Matrix& matrix)
    {
        std::ifstream in(filename,ios::in | std::ios::binary);
        typename Matrix::Index rows=0, cols=0;
        in.read((char*) (&rows),sizeof(typename Matrix::Index));
        in.read((char*) (&cols),sizeof(typename Matrix::Index));
        matrix.resize(rows, cols);
        in.read( (char *) matrix.data() , rows*cols*sizeof(typename Matrix::Scalar) );
        in.close();
    }
}

// run with train or test for argv[1]
int main(int argc, char* argv[]) {
	// set train/test image directories
	const char* train_dir = "train_images/fa_H";
	const char* eigen_dir = "eigenvalues/";
	const char* test_dir = "test_images/fb_H";
	vector<pair<string, VectorXf> > trainFaces;
	vector<pair<string, VectorXf> > testFaces;
	VectorXf averageFace;
	MatrixXf eigenFaces;
	VectorXf eigenVals;
	vector<pair<float, VectorXf>> eigenPairs;

	// TRAINING MODE
	cout << argv[1];
	if(string(argv[1]) == "train") {
		// read in training face images
		readFaceImgs(train_dir, trainFaces);

		// calculate eigen values and average face
		cout << "Training mode. Computing average face and eigenfaces." << endl;
		if(!readSavedFaces(averageFace, eigenFaces, eigenVals, eigen_dir)) {
			cout << "No saved eigen faces. Will now compute and save.\n" << endl;
			calcEigenFaces(trainFaces, averageFace, eigenFaces, eigenVals, eigen_dir);
		}
		else {
			cout << "Eigenfaces already computed.\n" << endl;
		}
	}
	// TESTING MODE
	else if(string(argv[1]) == "test") {
		// read in training face images
		readFaceImgs(train_dir, trainFaces);
		// read in test data
		readFaceImgs(test_dir, testFaces);

		// calculate eigen values and average face
		cout << "Testing mode. Reading in average face and eigenfaces." << endl;
		if(!readSavedFaces(averageFace, eigenFaces, eigenVals, eigen_dir)) {
			cout << "No saved eigen faces. Will now compute and save.\n" << endl;
			calcEigenFaces(trainFaces, averageFace, eigenFaces, eigenVals, eigen_dir);
		}

		// normalize eigen faces
		for(int i = 0; i < eigenFaces.cols(); i++) {
			eigenFaces.col(i).normalize();
		}

		// print out average face image
		char average_dir[] = "average/averageFace.pgm";
		// change def to 'l' if using with low def images
		char def = 'h';
		cout << "Writing out average face to average/ directory." << endl;
		writeFace(averageFace, average_dir, def);

		// Find k
		float eigenSum = eigenVals.sum();
		float currTotal = 0.00;
		int k;
		for(k = 0; currTotal / eigenSum < PERCENT_PRES && k < eigenVals.rows(); k++) {
			currTotal += eigenVals.row(k)(0);
		}
		cout << "To preserve " << PERCENT_PRES << " percent of the data, using k = " << k << endl;

		// reduce eigen faces to k
		MatrixXf kEigenFaces(averageFace.rows(), k);
		kEigenFaces = eigenFaces.block(0,0,averageFace.rows(),k);

		// project all training faces onto eigenspace
		vector<pair<string, VectorXf> > projTrainFaces;
		for(unsigned int i = 0; i < trainFaces.size(); i++) {
			pair<string, VectorXf> proj(trainFaces[i].first, projectFace(trainFaces[i].second, averageFace, kEigenFaces));
			projTrainFaces.push_back(proj);
		}

		// project all test faces onto eigenspace
		vector<pair<string, VectorXf> > projTestFaces;
		for(unsigned int i = 0; i < testFaces.size(); i++) {
			cout << "Projecting onto i: " << i << endl;
			pair<string, VectorXf> proj(testFaces[i].first, projectFace(testFaces[i].second, averageFace, kEigenFaces));
			projTestFaces.push_back(proj);
		}

		int correct = 0;
		int incorrect = 0;

		for(int k = 0; k < projTestFaces.size(); k++) {
			if(classify(projTestFaces[k], projTrainFaces, trainFaces)) {
				correct++;
			}
			else {
				incorrect++;
			}
		}
		cout << "correct: " << correct << endl;
		cout << "incorrect: " << incorrect << endl;
	}

}

/*********************** FUNCTION DEFINITIONS **********************/

bool fileExists(const char* fileName) {
	ifstream ifile(fileName);
	return ifile;
}


void read_directory(const std::string& name, stringvec& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}


bool readSavedFaces(VectorXf &averageFace, MatrixXf &eigenfaces, VectorXf &eigenvalues,const char* path) {
    char fileName[100];

    sprintf(fileName, "%s-binary.dat", path);
    if(fileExists(fileName)) {
        Eigen::read_bin(fileName, eigenfaces);
    }
    else {
        return false;
    }

    sprintf(fileName, "%s-values-binary.dat", path);
    if(fileExists(fileName)) {
        Eigen::read_bin(fileName, eigenvalues);
    }
    else {
        return false;
    }

    sprintf(fileName, "%s-avg-binary.dat", path);
    if(fileExists(fileName)){
        Eigen::read_bin(fileName, averageFace);
    }
    else{
        return false;
    }
    return true;
}


void readFaceImgs(string dir, vector<pair<string, VectorXf> > &faces) {
	ImageType image(N, M, Q);
	int n;
	string s;
	stringvec v;
  read_directory(dir, v);

	for (vector<string>::const_iterator i = v.begin(); i != v.end(); i++) {
		if( *i != "." && *i != "..") {
			cout << "FILE: " << *i << endl;
			s = dir + string(*i);
			n = s.length();
			char filename[n + 1];
			strcpy(filename, s.c_str());
			readImage(filename, image, N, M, Q);

			VectorXf currFace;
			currFace = VectorXf(N*M);
	    for(int i = 0; i < N; i++) {
	        for(int j = 0; j < M; j++) {
	            int t = 0;
	            image.getPixelVal(i, j, t);
	            currFace[i*M + j] = t;
	        }
	    }
			faces.push_back(pair<string, VectorXf>(string(*i, 0, 5), currFace));
		}
	}
}


void writeFace(VectorXf face, char file[], char def) {
	int rows, cols, levels;

	if(def == 'h') {
		rows = 60;
		cols = 48;
	}
	else {
		rows = 20;
		cols = 16;
	}

	levels = 255;
	ImageType image(rows, cols, levels);
	float min, max, temp;
	min = face.minCoeff();
	max = face.maxCoeff();

	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			temp = (face[i*cols+j] - min) / (max - min);
			image.setPixelVal(i, j, temp*255);
		}
	}
	writeImage(file, image);
}


void calcEigenFaces(vector<pair<string, VectorXf> > faces, VectorXf &averageFace, MatrixXf &eigenFaces,
	VectorXf &eigenVals, const char* path) {
	char fileName[100];
	ofstream output;
	EigenSolver<MatrixXf> solver;

	// calculate average face
	averageFace = VectorXf(faces[0].second.rows());
  averageFace.fill(0);
  for(auto i = faces.begin(); i != faces.end(); i++)
  {
      averageFace += i->second;
  }
	averageFace /= faces.size();

	sprintf(fileName, "%s-avg-binary.dat", path);
	Eigen::write_bin(fileName, averageFace);

	// subtract average face from training faces
	MatrixXf norm;
	norm = MatrixXf(averageFace.rows(), faces.size());
  for(vector<VectorXf>::size_type i = 0; i < faces.size(); i++) {
      norm.col(i) = faces[i].second - averageFace;
	}

	// calculate eigenvectors using AT*A trick
	MatrixXf eigenVectors = MatrixXf(faces.size(), faces.size());
	eigenVectors = norm.transpose()*norm;
	solver.compute(eigenVectors, true);

	// extract eigenfaces and eigenvalues from solved
	eigenFaces = MatrixXf(averageFace.rows(), faces.size());
	eigenFaces = norm * solver.eigenvectors().real();
	eigenVals = VectorXf(eigenFaces.cols());
	eigenVals = solver.eigenvalues().real();

	// write and save to file
	sprintf(fileName, "%s-binary.dat", path);
	Eigen::write_bin(fileName, eigenFaces);
	Eigen::read_bin(fileName,eigenFaces);

	sprintf(fileName, "%s-values-binary.dat", path);
	Eigen::write_bin(fileName, eigenVals);

	sprintf(fileName, "%s-EigenVectors.txt", path);
	output.open(fileName);
	output << eigenFaces;
	output.close();
}


VectorXf projectFace(VectorXf face, VectorXf avFace, MatrixXf eigenFaces) {
    vector<float> coeffs;
    VectorXf normFace = face - avFace;
    VectorXf projFace(avFace.rows());
    projFace.fill(0);
    for(int i = 0; i < eigenFaces.cols(); i++) {
        float a = (eigenFaces.col(i).transpose() * normFace)(0,0);
        coeffs.push_back(a);
        projFace += (coeffs[i] * eigenFaces.col(i));
    }
    return projFace + avFace;
}

bool compare(pair<string, float> a, pair<string, float> b) {
    return a.second < b.second;
}

// input: projected test face you want to test, vector of all projected training faces, and original triaing faces
bool classify(pair<string, VectorXf> projTestFace, vector<pair<string, VectorXf>> projTrainFaces,
  vector<pair<string, VectorXf> > trainFaces) {

  vector< pair<string, float> > testPairs;
  float dist = 0.0;

  // find all distances
  for(unsigned int t = 0; t < trainFaces.size(); t++) {
    dist = (projTestFace.second - projTrainFaces[t].second).norm();
    pair<string, float> temp(trainFaces[t].first, dist);
    testPairs.push_back(temp);
  }

  // Sort distances
  sort(testPairs.begin(), testPairs.end(), compare);

  // returns true if ID correctly matches closest one
	for(int i = 0; i < 50; i++) {
		if(projTestFace.first == testPairs[i].first) {
			return true;
		}
	}
	return false;
}
