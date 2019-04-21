#include "eigen3/Eigen/Dense"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "image.h"
#include <sys/types.h>
#include <dirent.h>
#include <iterator>
#include <algorithm>

using namespace Eigen;
using namespace std;

// CHANGE M AND N IF CHANGING RES
int M = 48;  
int N = 60;
int Q = 255;



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
int classify(pair<string, VectorXf>, vector<pair<string, VectorXf>>, vector<pair<string, VectorXf>>, int, string&, float);
void calc_N_Vals(vector<pair<string, VectorXf> >, vector<pair<string, VectorXf> > ,  vector<pair<string, VectorXf> >, int , int& , int& , vector<string>&, vector<string>&, float);
float findMax(vector<pair<string, VectorXf>> , vector<pair<string, VectorXf>>, vector<pair<string, VectorXf>> , vector<pair<string, VectorXf>> );


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
	const char* train_dir = "train_images/fa_H/"; //change to fa_L for grad portion
	const char* eigen_dir = "eigenvalues/";
	const char* test_dir = "test_images/fb_H/"; //change to fb_L for grad portion
	vector<pair<string, VectorXf> > trainFaces;
	vector<pair<string, VectorXf> > testFaces;
	VectorXf averageFace;
	MatrixXf eigenFaces;
	VectorXf eigenVals;
	vector<pair<float, VectorXf>> eigenPairs;

	// TRAINING MODE

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
	else if(string(argv[1]) == "test") 
	{
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
		//char average_dir[] = "average/averageFace.pgm";
		// change def to 'l' if using with low def images
		
		char def = 'h';
		/*
		cout << "Writing out average face to average/ directory." << endl;
		writeFace(averageFace, average_dir, def);
	
		int endVal = eigenFaces.cols() -1;
		
		//compute top 10 eigen		
		char dir [] = "average/1.pgm";
		writeFace(VectorXf(eigenFaces.col(0)), dir, def);
		char dir1 [] = "average/2.pgm";
		writeFace(VectorXf(eigenFaces.col(1)), dir1, def);
		char dir2 [] = "average/3.pgm";
		writeFace(VectorXf(eigenFaces.col(2)), dir2, def);
		char dir3 [] = "average/4.pgm";
		writeFace(VectorXf(eigenFaces.col(3)), dir3, def);
		char dir4 [] = "average/5.pgm";
		writeFace(VectorXf(eigenFaces.col(4)), dir4, def);
		char dir5 [] = "average/6.pgm";
		writeFace(VectorXf(eigenFaces.col(5)), dir5, def);
		char dir6 [] = "average/7.pgm";
		writeFace(VectorXf(eigenFaces.col(6)), dir6, def);
		char dir7 [] = "average/8.pgm";
		writeFace(VectorXf(eigenFaces.col(7)), dir7, def);
		char dir8 [] = "average/9.pgm";
		writeFace(VectorXf(eigenFaces.col(8)), dir8, def);
		char dir9 [] = "average/10.pgm";
		writeFace(VectorXf(eigenFaces.col(9)), dir9, def);
		

		//compute bottom 10 eigen
		char dir10 [] = "average/ev.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal)), dir10, def);
		char dir11 [] = "average/ev1.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-1)), dir11, def);
		char dir12 [] = "average/ev2.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-2)), dir12, def);
		char dir13 [] = "average/ev3.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-3)), dir13, def);
		char dir14 [] = "average/ev4.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-4)), dir14, def);
		char dir15 [] = "average/ev5.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-5)), dir15, def);
		char dir16 [] = "average/ev6.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-6)), dir16, def);
		char dir17 [] = "average/ev7.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-7)), dir17, def);
		char dir18 [] = "average/ev8.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-8)), dir18, def);
		char dir19 [] = "average/ev9.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-9)), dir19, def);


		vector<pair<string, VectorXf> > projTrainFaces;
		*/
		int k = 0;
		

		// Find k
		
		float eigenSum = eigenVals.sum();
		float currTotal = 0.00;
		/*

		vector<float> PERCENT_PRES{ 0.80, 0.90, 0.95};
		ofstream ofile;
		ofile.open("test_cases.txt");
		
		for (int i = 0; i< PERCENT_PRES.size(); i++)
		{
			projTrainFaces.clear();
		

			for(k = 0; currTotal / eigenSum < PERCENT_PRES.at(i) && k < eigenVals.rows(); k++) {
				currTotal += eigenVals.row(k)(0);
			}
			cout << "To preserve " << PERCENT_PRES.at(i) << " percent of the data, using k = " << k << endl;
			ofile << "To preserve " << PERCENT_PRES.at(i) << " percent of the data, using k = " << k << endl;

			// reduce eigen faces to k
			MatrixXf kEigenFaces(averageFace.rows(), k);
			kEigenFaces = eigenFaces.block(0,0,averageFace.rows(),k);

			// project all training faces onto eigenspace
			
			for(unsigned int i = 0; i < trainFaces.size(); i++) {
				pair<string, VectorXf> proj(trainFaces[i].first, projectFace(trainFaces[i].second, averageFace, kEigenFaces));
				projTrainFaces.push_back(proj);
			}
			vector<pair<string, VectorXf> > projTestFaces;

			// project all test faces onto eigenspace
		
			for(unsigned int i = 0; i < testFaces.size(); i++) {
				
				pair<string, VectorXf> proj(testFaces[i].first, projectFace(testFaces[i].second, averageFace, kEigenFaces));
				projTestFaces.push_back(proj);
			}

			int correct = 0;
			int incorrect = 0;
			vector<string> cor;
			vector<string> inc;

			
			for (int N = 1; N <= 50; N++){
				calc_N_Vals(projTestFaces, projTrainFaces, trainFaces, N, correct, incorrect, cor, inc, PERCENT_PRES.at(i));
				cout << N << endl;
				cout << "correct: " << correct << endl;
				cout << "incorrect: " << incorrect << endl;
				cout << "percent correct: " << float(correct/(correct+incorrect)) << endl;
				ofile << N << endl;
				ofile << "correct: " << correct << endl;
				ofile << "incorrect: " << incorrect << endl;
				ofile << "percent correct: " << float(correct/(correct+incorrect)) << endl;
			
				ofile << endl << endl;
				ofile << endl << endl;
			}
			ofile.close();
			
			
		}
		*/
/////////////////////////////////////////////////////////////////////////////////PART B////////////////////////////////////////////////////////////////////////////////////////////		
		const char* train_dir2 = "train_images/fa2_H/";
		const char* eigen_dir2 = "eigenvalues2/";
		vector<pair<string, VectorXf> > trainFaces2;
		vector<pair<string, VectorXf> > projTestFaces2;
		vector<pair<string, VectorXf> > testFaces2;
		VectorXf averageFace2;
		MatrixXf eigenFaces2;
		VectorXf eigenVals2;
		vector<pair<float, VectorXf>> eigenPairs2;
		vector<pair<string, VectorXf> > projTrainFaces2;
		
	

		readFaceImgs(train_dir2, trainFaces2);

		// calculate eigen values and average face
		cout << "Training mode. Computing average face and eigenfaces." << endl;
		if(!readSavedFaces(averageFace2, eigenFaces2, eigenVals2, eigen_dir2)) {
			cout << "No saved eigen faces. Will now compute and save.\n" << endl;
			calcEigenFaces(trainFaces2, averageFace2, eigenFaces2, eigenVals2, eigen_dir2);
		}
		else {
			cout << "Eigenfaces already computed.\n" << endl;
		}

		readFaceImgs(test_dir, testFaces2);
		for(int i = 0; i < eigenFaces2.cols(); i++) {
			eigenFaces2.col(i).normalize();
		}

		char average_dir20[] = "average/averageFace2.pgm";
		// change def to 'l' if using with low def images
		cout << "Writing out average face to average/ directory." << endl;
		writeFace(averageFace2, average_dir20, def);

		eigenSum= eigenVals2.sum();
		currTotal = 0.00;

		for(k = 0; currTotal / eigenSum <0.95 && k < eigenVals2.rows(); k++) {
				currTotal += eigenVals2.row(k)(0);
			}
		MatrixXf kEigenFaces2(averageFace2.rows(), k);
		kEigenFaces2 = eigenFaces2.block(0,0,averageFace2.rows(),k);

		//create vector of train faces
		for(unsigned int i = 0; i < trainFaces2.size(); i++) {
				pair<string, VectorXf> proj2(trainFaces2[i].first, projectFace(trainFaces2[i].second, averageFace2, kEigenFaces2));
				projTrainFaces2.push_back(proj2);
			}
		
		//create vector of test faces
		for(unsigned int i = 0; i < testFaces2.size(); i++) {
				
			pair<string, VectorXf> proj2(testFaces2[i].first, projectFace(testFaces2[i].second, averageFace2, kEigenFaces2));
			projTestFaces2.push_back(proj2);
		}

		float max =  findMax(projTestFaces2,  projTrainFaces2,  trainFaces2, testFaces2);
		
		cout << "MAX " << max << endl;


		
		float thresh;
		ofstream ofile2;
		ofile2.open("thresholding.txt");


		string first;
		float partition = 0.0;

		for (float a = 0.0; a <= 100.00; a++)
		{
			int fp = 0;
			int tp = 0;
			int check = 0;
			int nonIntruder = 1105;
			int intruder = 91;

			partition = max/100.00;
			thresh = partition * a;
			thresh = a;
			cout << "T= " << thresh << endl;
			ofile2 << "T= " << thresh << endl;
			
			for (unsigned int l = 0; l < testFaces2.size(); l++)
			{
				
				int test = classify(projTestFaces2[l], projTrainFaces2, trainFaces2, 1, first, partition*50); //0.5 should be thresh

				if (test == 1)
				{
					tp++;
				}
				if (test == 2)
				{
					fp++;
				}
				if(test == 3){
					check ++;
				}
			}
			//cout << "CHECK = " << check << endl;
			cout << "FP= " << fp << " Rate= " << float(float(fp)/float(intruder)) << " TP= " << tp << " Rate= " << float(float(tp)/float(nonIntruder)) << " Precision= " << float(float(tp) / float((tp+fp))) << " Recall= "<< float(float(tp)/(float(nonIntruder - tp))) << endl;
			ofile2 <<  "FP= " << fp << " Rate= " << float(float(fp)/float(intruder)) << " TP= " << tp << " Rate= " << float(float(tp)/float(nonIntruder)) << " Precision= " << float(float(tp) / float((tp+fp))) << " Recall= "<< float(float(tp)/(float(nonIntruder - tp))) << endl;
			ofile2 << endl;

			

			}
			/*
		
		
			int fp = 0;
			int tp = 0;
			int check = 0;
			int nonIntruder = 1105;
			int intruder = 91;
			string first;
			//for (int a = 0; a <= 100; a++){
				for (unsigned int l = 0; l < 10; l++) //For every test face
				{
					float max = findMax(projTestFaces2[l], projTrainFaces2, trainFaces2, testFaces2); //calculate max distance
					float partition = max/100.00;	 //partition it
					//float thresh = partition * a;
					int test = classify(projTestFaces2[l], projTrainFaces2, trainFaces2, 1, first, thresh);
					if (test == 1)
					{
						tp++;
					}
					if (test == 2)
					{
						fp++;
					}
					if(test == 3){
						check ++;
					}
				
				}
			

			cout << "CHECK = " << check << endl;
			cout << "FP= " << fp << " Rate= " << float(float(fp)/float(intruder)) << " TP= " << tp << " Rate= " << float(float(tp)/float(nonIntruder)) << " Precision= " << float(float(tp) / float((tp+fp))) << " Recall= "<< float(float(tp)/(float(nonIntruder - tp))) << endl;
			//ofile2 <<  "FP= " << fp << " Rate= " << float(float(fp)/float(intruder)) << " TP= " << tp << " Rate= " << float(float(tp)/float(nonIntruder)) << " Precision= " << float(float(tp) / float((tp+fp))) << " Recall= "<< float(float(tp)/(float(nonIntruder - tp))) << endl;
			//ofile2 << endl;
			//}
			
			
			cout << "CHECK = " << check << endl;
			cout << "FP= " << fp << " Rate= " << float(float(fp)/float(intruder)) << " TP= " << tp << " Rate= " << float(float(tp)/float(nonIntruder)) << " Precision= " << float(float(tp) / float((tp+fp))) << " Recall= "<< float(float(tp)/(float(nonIntruder - tp))) << endl;
			ofile2 <<  "FP= " << fp << " Rate= " << float(float(fp)/float(intruder)) << " TP= " << tp << " Rate= " << float(float(tp)/float(nonIntruder)) << " Precision= " << float(float(tp) / float((tp+fp))) << " Recall= "<< float(float(tp)/(float(nonIntruder - tp))) << endl;
			ofile2 << endl;
			*/
		

		//ofile2.close();







////////////////////////////////////////////////////////////////////////////////////////////////////////Part C ////////////////////////////////////////////////////////////////
/*

	const char* train_dir21 = "train_images/fa_L/"; //change to fa_L for grad portion
	const char* eigen_dir3 = "eigenvalues3/";
	const char* test_dir3 = "test_images/fb_L/"; //change to fb_L for grad portion
	vector<pair<string, VectorXf> > trainFaces;
	vector<pair<string, VectorXf> > testFaces;
	VectorXf averageFace;
	MatrixXf eigenFaces;
	VectorXf eigenVals;
	vector<pair<float, VectorXf>> eigenPairs;

	// TRAINING MODE

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
	else if(string(argv[1]) == "test") 
	{
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
	
		int endVal = eigenFaces.cols() -1;
		
		//compute top 10 eigen		
		char dir [] = "average/1.pgm";
		writeFace(VectorXf(eigenFaces.col(0)), dir, def);
		char dir1 [] = "average/2.pgm";
		writeFace(VectorXf(eigenFaces.col(1)), dir1, def);
		char dir2 [] = "average/3.pgm";
		writeFace(VectorXf(eigenFaces.col(2)), dir2, def);
		char dir3 [] = "average/4.pgm";
		writeFace(VectorXf(eigenFaces.col(3)), dir3, def);
		char dir4 [] = "average/5.pgm";
		writeFace(VectorXf(eigenFaces.col(4)), dir4, def);
		char dir5 [] = "average/6.pgm";
		writeFace(VectorXf(eigenFaces.col(5)), dir5, def);
		char dir6 [] = "average/7.pgm";
		writeFace(VectorXf(eigenFaces.col(6)), dir6, def);
		char dir7 [] = "average/8.pgm";
		writeFace(VectorXf(eigenFaces.col(7)), dir7, def);
		char dir8 [] = "average/9.pgm";
		writeFace(VectorXf(eigenFaces.col(8)), dir8, def);
		char dir9 [] = "average/10.pgm";
		writeFace(VectorXf(eigenFaces.col(9)), dir9, def);
		

		//compute bottom 10 eigen
		char dir10 [] = "average/ev.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal)), dir10, def);
		char dir11 [] = "average/ev1.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-1)), dir11, def);
		char dir12 [] = "average/ev2.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-2)), dir12, def);
		char dir13 [] = "average/ev3.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-3)), dir13, def);
		char dir14 [] = "average/ev4.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-4)), dir14, def);
		char dir15 [] = "average/ev5.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-5)), dir15, def);
		char dir16 [] = "average/ev6.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-6)), dir16, def);
		char dir17 [] = "average/ev7.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-7)), dir17, def);
		char dir18 [] = "average/ev8.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-8)), dir18, def);
		char dir19 [] = "average/ev9.pgm";
		writeFace(VectorXf(eigenFaces.col(endVal-9)), dir19, def);


		vector<pair<string, VectorXf> > projTrainFaces;
	
		 k = 0;
		

		// Find k
		
		eigenSum = eigenVals.sum();
		currTotal = 0.00;
		

		
		ofstream ofile;
		ofile.open("test_casesL.txt");
		
		for (int i = 0; i< PERCENT_PRES.size(); i++)
		{
			projTrainFaces.clear();
		

			for(k = 0; currTotal / eigenSum < PERCENT_PRES.at(i) && k < eigenVals.rows(); k++) {
				currTotal += eigenVals.row(k)(0);
			}
			cout << "To preserve " << PERCENT_PRES.at(i) << " percent of the data, using k = " << k << endl;
			ofile << "To preserve " << PERCENT_PRES.at(i) << " percent of the data, using k = " << k << endl;

			// reduce eigen faces to k
			MatrixXf kEigenFaces(averageFace.rows(), k);
			kEigenFaces = eigenFaces.block(0,0,averageFace.rows(),k);

			// project all training faces onto eigenspace
			
			for(unsigned int i = 0; i < trainFaces.size(); i++) {
				pair<string, VectorXf> proj(trainFaces[i].first, projectFace(trainFaces[i].second, averageFace, kEigenFaces));
				projTrainFaces.push_back(proj);
			}
			vector<pair<string, VectorXf> > projTestFaces;

			// project all test faces onto eigenspace
		
			for(unsigned int i = 0; i < testFaces.size(); i++) {
				
				pair<string, VectorXf> proj(testFaces[i].first, projectFace(testFaces[i].second, averageFace, kEigenFaces));
				projTestFaces.push_back(proj);
			}

			int correct = 0;
			int incorrect = 0;
			vector<string> cor;
			vector<string> inc;

			
			for (int N = 1; N <= 50; N++){
				calc_N_Vals(projTestFaces, projTrainFaces, trainFaces, N, correct, incorrect, cor, inc, PERCENT_PRES.at(i));
				cout << N << endl;
				cout << "correct: " << correct << endl;
				cout << "incorrect: " << incorrect << endl;
				cout << "percent correct: " << float(correct/(correct+incorrect)) << endl;
				ofile << N << endl;
				ofile << "correct: " << correct << endl;
				ofile << "incorrect: " << incorrect << endl;
				ofile << "percent correct: " << float(correct/(correct+incorrect)) << endl;
			
				ofile << endl << endl;
				ofile << endl << endl;
			}
			ofile.close();
			
			
		}
		







*/


	}
}

/*********************** FUNCTION DEFINITIONS **********************/

bool fileExists(const char* fileName) {
	ifstream ifile(fileName);
	return ifile.good();
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
			//cout << "FILE: " << *i << endl;
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

float findMax(vector<pair<string, VectorXf>> projTestFaces, vector<pair<string, VectorXf>> projTrainFaces, vector<pair<string, VectorXf>> trainFaces, vector<pair<string, VectorXf>> testFaces)
{
	float temp = 0.0;
	float dist = 0.0;
		for (int i = 0; i < testFaces.size(); i++){
			for(unsigned int j = 0; j < trainFaces.size(); j++) {
			   dist = (projTestFaces[i].second - projTrainFaces[j].second).norm();
			   cout << dist << endl;
			   if (dist > temp){
			   		temp = dist;
			   }
		}
	}
	
	return temp;
}

// input: projected test face you want to test, vector of all projected training faces, and original triaing faces
int classify(pair<string, VectorXf> projTestFace, vector<pair<string, VectorXf>> projTrainFaces,
  vector<pair<string, VectorXf> > trainFaces, int N, string& first, float thresh) {


  float dist = 0.0;
  vector< pair<string, float> > testPairs;

  
  for(unsigned int t = 0; t < trainFaces.size(); t++) {
	    dist = (projTestFace.second - projTrainFaces[t].second).norm();
	    pair<string, float> temp(trainFaces[t].first, dist);

	    testPairs.push_back(temp);

	    cout << "a " << dist << endl;
	  }

  
  // Sort distances
  sort(testPairs.begin(), testPairs.end(), compare);
  first = testPairs[0].first;
  float val = testPairs[0].second;
  //cout << "VAL = " << val << " THRESH " << thresh << endl;
  //cout << first << endl;
  vector<string> names;

  if (thresh != 1)
  {
  	if (testPairs[0].second < thresh)
  	{
  		//cout << "Thresh: " << thresh << "pt " << testPairs[0].second << endl;
  		if(projTestFace.first == testPairs[0].first) 
  		{
			return 1;
		}

		else if (projTestFace.first != testPairs[0].first)
			{
				return 2;
			}
		
	
  	}

  	else if (testPairs[0].second >=thresh)
  	{
  		cout << "BAD Thresh: " << thresh << "pt " << testPairs[0].second << endl;
  		return 3;
  	}
  }

  // returns true if ID correctly matches closest one
  //Add an outer loop to check series of answers. 
  else if (thresh == 1){

		for(int i = 0; i < N; i++) { //Check the top 50 and see if they match
			if(projTestFace.first == testPairs[i].first) {
					return 1;
			}

		}
		return 2;
  }
	
}
 
void calc_N_Vals(vector<pair<string, VectorXf> > projTestFaces, vector<pair<string, VectorXf> > projTrainFaces,  vector<pair<string, VectorXf> >trainFaces, int N, int& correct, int& incorrect, vector<string>& cor, vector<string>& inc, float pres)
 {
 	char h = 'h';
 	int val = 1;
 	int ival =1;
 	for(unsigned int k = 0; k < projTestFaces.size(); k++)
 	 {
 	 	string first;
 	 	
 	 	
		if(classify(projTestFaces[k], projTrainFaces, trainFaces, N, first, 1) == true)
		{
			correct++;
			
			if(N == 1 && val<= 3)
			{
				
				for (unsigned int j = 0; j < projTrainFaces.size(); j++)
				{
					
					if (projTrainFaces[j].first == projTestFaces[k].first)
					{
						
						//cout << "CORRECT: " << correct << " PRES: " << pres << endl;
						if (val ==1 && pres == float(0.8))
						{
								cout << "CORRECT: " << correct << " PRES: " << pres << endl;
								char dir1[] = "8_1_tr.pgm";
								char dir2[] = "8_1_te.pgm";
								writeFace(projTrainFaces[j].second, dir1, h);
								writeFace(projTestFaces[k].second, dir2, h);
						}

						if (val == 1 && pres == float(0.9)){
								cout << "CORRECT: " << correct << " PRES: " << pres << endl;
								char dir1[] = "9_1_tr.pgm";
								char dir2[] = "9_1_te.pgm";
								writeFace(projTrainFaces[j].second, dir1, h);
								writeFace(projTestFaces[k].second, dir2,h);
							}

							

						if (val == 1 && pres == float(0.95)){
								cout << "CORRECT: " << correct << " PRES: " << pres << endl;
								char dir1[] = "95_1_tr.pgm";
								char dir2[] = "95_1_te.pgm";
								writeFace(projTrainFaces[j].second, dir1, h);
								writeFace(projTestFaces[k].second, dir2, h);
							}
						if (val == 2 && pres == float(0.8))
						{
							
							cout << "CORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "8_2_tr.pgm";
							char dir2[] = "8_2_te.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);
							
						if (val ==2 && pres == float(0.9))
						{
							cout << "CORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "9_2_tr.pgm";
							char dir2[] = "9_2_te.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);
							}


						if (val == 2 && pres == float(0.95))
						{
							cout << "CORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "95_2_tr.pgm";
							char dir2[] = "95_2_te.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

							}
						}
						if (val ==3 && pres == float(0.8)){
							
							cout << "CORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "8_3_tr.pgm";
							char dir2[] = "8_3_te.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

						}
							
						
						if ( val ==3 && pres == float(0.9))
							{
								cout << "CORRECT: " << correct << " PRES: " << pres << endl;
								char dir1[] = "9_3_tr.pgm";
								char dir2[] = "9_3_te.pgm";
								writeFace(projTrainFaces[j].second, dir1, h);
								writeFace(projTestFaces[k].second, dir2, h);

							}

						

						
						if (val == 3 && pres == float(0.95))
						{
							cout << "CORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "95_3_tr.pgm";
							char dir2[] = "95_3_te.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

						}
					val++;
					
					}

				}
				
			}

				
		}
			//cor.push_back(projTestFaces[k].first);
		
		else 
		{
			
			incorrect++;


			if(N == 1 && ival <=3)
			{

				for (unsigned int j = 0; j < projTrainFaces.size(); j++)
				{
					if (projTrainFaces[j].first == first)
					{
						if (ival ==1 && pres == float(0.8))
						{
							cout << "INCORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "8_1_x.pgm";
							char dir2[] = "8_1_tex.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

						}
						if (ival ==2 && pres == float(0.8))						{
							cout << "INCORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "8_2_x.pgm";
							char dir2[] = "8_2_tex.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

						}
						if (ival ==3 && pres == float(0.8))
						{
							cout << "INCORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "8_3_x.pgm";
							char dir2[] = "8_3_tex.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

						}
						if (ival ==1 && pres == float(0.9))
						{
							cout << "INCORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "9_1_x.pgm";
							char dir2[] = "9_1_txe.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

						}
						if (ival ==2 && pres == float(0.9))
						{
							cout << "INCORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "9_2_x.pgm";
							char dir2[] = "9_2_tex.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

						}
						if (ival ==3 && pres == float(0.9))
						{
							cout << "INCORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "9_3_x.pgm";
							char dir2[] = "9_3_tex.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

						}

						if (ival ==1 && pres == float(0.95))
						{
							cout << "INCORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "95_1_x.pgm";
							char dir2[] = "95_1_tex.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

						}
						if (ival==2 && pres == float(0.95))
						{
							cout << "INCORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "95_2_x.pgm";
							char dir2[] = "95_2_tex.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2, h);

						}
						if (ival ==3 && pres == float(0.95))
						{
							cout << "INCORRECT: " << correct << " PRES: " << pres << endl;
							char dir1[] = "95_3_x.pgm";
							char dir2[] = "95_3_tex.pgm";
							writeFace(projTrainFaces[j].second, dir1, h);
							writeFace(projTestFaces[k].second, dir2,h);

						}
					ival++;
					}
				}


			}

			inc.push_back(projTestFaces[k].first);
		}
	}
 }

