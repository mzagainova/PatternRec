#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
using namespace std;


void populate_pixel_vectors(vector<float> &r, vector<float> &b, vector<float> &g,  int totalPix, vector<float>& vectr, vector<float>& vectg, vector<float>& vectb);
void create_2d_array(vector<float>& vectR, vector<float>& vectB, vector<float>& vectG, vector<float>& counter, vector<float>& vectBWX, vector<float>& vectBWY, int height, int width);
void regularize_pixels(vector<float>& vectr, vector<float>& vectg, vector<float>& vectb);
void get_skin_distribution(vector<float>& redVec, vector<float>& greenVec, vector<float>& blueVec, vector<float>& vectr, vector<float>& vectg, vector<float>& vectb, vector<float>& counter);
void calculate_sample_averages(vector<float> redVec, vector<float> greenVec, float& sampleAvRed, float& sampleAvGreen, int numSamples);
void calculate_sample_covariance(vector<float> redVec, vector<float> greenVec, vector<float>& covarVec, float& sampleAvRed, float& sampleAvGreen, int numSamples, int& counter);
float calc_determinant(vector<float> covarVec);
float calc_frac(vector<float> covarVec);
double pwr_calc(vector<float> covarVec, float sampleAvRed, float sampleAvGreen, float redPix, float greenPix);
void calc_inverse(vector<float> covarVec, vector<float>& covarVecIn);
float calc_gx(vector<float> covarVec, float SampleAvRed, float sampleAGreen, float redPix, float greenPix);
void testImPix(vector<float> covarVec, float sampleAvRed, float sampleAvGreen, float thresh, int& FP, int& FN, int totalPix);
void test_thresholds(vector<float> covarVec, float sampleAvRed, float sampleAvGreen, int totalPix);
bool is_bigger(float gx, float thresh);
void print_image(vector<float> R, vector<float> B, vector<float> G, float thresh, string num);
void calculate_ROC (vector<float>R, vector<float> B, vector<float> G, int& FP, int& FN, int totalPix,  ifstream& f);


int main(){
	
	vector<float> redVec;
	vector<float> greenVec;
	vector<float> blueVec;
	vector<float> vectBWX;
	vector<float> vectBWY;
	vector<float> vectR;
	vector<float> vectG;
	vector<float> vectB;
	vector<float> vectr;
	vector<float> vectg;
	vector<float> vectb;
	vector<float> counter;
	vector<float> covarVec;

	int totalPix = 2583552;
	int width = 1856;
	int height = 1392;
	int pix = 0;
	float sampleAvRed = 0;
	float sampleAvGreen = 0;




	populate_pixel_vectors(vectR, vectG, vectB, totalPix, vectr, vectg, vectb);

	create_2d_array( vectR,  vectB,  vectG,  counter,  vectBWX, vectBWY, height, width);


	regularize_pixels(vectr, vectg, vectb);

 
	get_skin_distribution(redVec, greenVec, blueVec, vectr, vectg, vectb, counter);


	int numSamples = redVec.size();
	calculate_sample_averages(redVec, greenVec, sampleAvRed, sampleAvGreen, numSamples);
	
	int c = 0;
	calculate_sample_covariance( redVec,  greenVec,  covarVec, sampleAvRed,  sampleAvGreen, numSamples, c);




	test_thresholds(covarVec, sampleAvRed, sampleAvGreen, totalPix);






}





//uppercase populates the r g and b from bw images
//lowercase populates rgb from rgb image
void populate_pixel_vectors(vector<float> &r, vector<float> &b, vector<float> &g,  int totalPix, vector<float>& vectr, vector<float>& vectg, vector<float>& vectb)
{
	ifstream f;
	f.open("ref1.txt");
	ifstream rbgIm;
	rbgIm.open("Training_1.txt");
	int x = 0;
	int n = 0;
	int count = 0;
	
	while (f >> x) 
	{
	  
	    if (count <=totalPix)
	    {
	        r.push_back(x);

	    }
	    if (count > totalPix && count <=totalPix*2 )
	    {
	        g.push_back(x);
	    }
	    if(count > totalPix*2 )
	    {
	        b.push_back(x);
	    }

	    count ++;
	}

	count =0;
	while (rbgIm >>n){
	    if (count <totalPix)
	    {
	        vectr.push_back(n);

	    }
	    if (count > totalPix && count < totalPix*2 + 1)
	    {
	        vectg.push_back(n);
	    }
	    if(count > totalPix*2 + 1)
	    {
	        vectb.push_back(n);
	    }

	    count++;
	 }
}


//If the pixel value is white in bw image, find its location and add it to counter
void create_2d_array(vector<float>& vectR, vector<float>& vectB, vector<float>& vectG, vector<float>& counter, vector<float>& vectBWX, vector<float>& vectBWY, int height, int width)
{
	
	int z = 0;
	for (int y = 0; y < width; y++)
	{
	    for (int x=0; x < height; x++)
	    {

	        if (vectR.at(z) == 255 && vectG.at(z) ==255 && vectB.at(z) == 255)
	        {
	           
	            vectBWX.push_back(y);
	            vectBWY.push_back(x);
	            
	           
	            counter.push_back(z);


	           
	        }
	        z++;

	    }
	}

}

//regularize rgb values between 0 and 1
void regularize_pixels(vector<float>& vectr, vector<float>& vectg, vector<float>& vectb)
{
	for (int i = 0; i < vectb.size(); i++)
	{
	   

	    int pixTot = vectr.at(i) + vectg.at(i) + vectb.at(i);
	   
	    if (vectr.at(i) > 0)
	        vectr.at(i) = float(vectr.at(i) / pixTot);
	    if (vectg.at(i) >0)
	     vectg.at(i) = float(vectg.at(i)/pixTot);
	}
}

//get skin distribution pixels using the white pixel locations from the bw image, add to red green and blue vectors
void get_skin_distribution(vector<float>& redVec, vector<float>& greenVec, vector<float>& blueVec, vector<float>& vectr, vector<float>& vectg, vector<float>& vectb, vector<float>& counter)
{
	for (int i = 0; i < counter.size(); i++)
	{
		
	    redVec.push_back(vectr.at(counter.at(i)));
	    greenVec.push_back(vectg.at(counter.at(i)));
	    blueVec.push_back(vectb.at(counter.at(i)));
	    

	}

	
}

//calculate the sample mean for red and green values in faces
void calculate_sample_averages(vector<float> redVec, vector<float> greenVec, float& sampleAvRed, float& sampleAvGreen, int numSamples)
{
	float redTot = 0;
	float greenTot = 0;
	for (int i = 0; i < redVec.size(); i++)
	{
	    redTot += redVec.at(i);
	    greenTot += greenVec.at(i);
	}
	sampleAvRed = redTot/numSamples;
	sampleAvGreen = greenTot/numSamples;

	
}

// calculate the sample variance for teh red and green values in faces
void calculate_sample_covariance(vector<float> redVec, vector<float> greenVec, vector<float>& covarVec, float& sampleAvRed, float& sampleAvGreen, int numSamples, int& counter)
{
	int sumRed = 0;
	int sumGreen = 0;
	for (int i = 0; i < redVec.size(); i++)
	{
		float diffR = redVec.at(i) - sampleAvRed;
		float diffG = greenVec.at(i) - sampleAvGreen;

		float sqRed = diffR * diffR;
		float rg = diffR * diffG;
		float sqGreen = diffG * diffG;
		//if counter = 0, then this is the first sample and nothing is in the vector. Populate it
		if (counter == 0)
		{
			covarVec.push_back(sqRed);
			covarVec.push_back(rg);
			covarVec.push_back(rg);
			covarVec.push_back(sqGreen);
		}
		//if counter >0, vector is populated, so sum the values
		if (counter > 0)
		{
			for (int j = 0; j <  covarVec.size(); j++)
			{
				if (j == 0)
					covarVec.at(j) += sqRed;
				if (j == 1)
					covarVec.at(j) += rg;
				if (j == 2)
					covarVec.at(j) += rg;
				if(j == 3)
					covarVec.at(j) += sqGreen;
			}
		}

		counter +=1;
	}
	//divide by num samples
	for(int k = 0; k < covarVec.size(); k++)
	{
		covarVec.at(k) = covarVec.at(k) / numSamples;
	}
}


//calculate the determinant
float calc_determinant(vector<float> covarVec)
{
	float det = 0;
	float ad = 0;
	float bc = 0;

	ad = covarVec.at(0) * covarVec.at(3);
	bc = covarVec.at(1) * covarVec.at(2);

	det = ad - bc;

	return det;
}


//calculate the fraction before e
float calc_frac(vector<float> covarVec)
{
	float determinant = calc_determinant(covarVec);
	determinant = sqrt(determinant);
	
	float doublePi = 3.1415926 * 2;

	return (1/(determinant *doublePi));

}

//calculate the inverse of the covariance matrix
void calc_inverse(vector<float> covarVec, vector<float>& covarVecIn)
{
	float det = calc_determinant(covarVec);
	

	covarVecIn.at(0) = covarVec.at(3) / det;
	covarVecIn.at(3) = covarVec.at(0) / det; 
	covarVecIn.at(1) = -covarVec.at(1) / det;
	covarVecIn.at(2) = -covarVec.at(2) / det;

}


//calculate the power that e is raised to
double pwr_calc(vector<float> covarVec, float sampleAvRed, float sampleAvGreen, float redPix, float greenPix)
{
	vector<float> covarVecIn = {0, 0, 0, 0};
	calc_inverse(covarVec, covarVecIn);
	



	float red = redPix - sampleAvRed;
	float green = greenPix - sampleAvGreen;
	

	float halvedNRed = -0.5 * (redPix-sampleAvRed);
	float halvedNGreen = -0.5 * (greenPix-sampleAvGreen);


	float dotA = halvedNRed *covarVecIn.at(0) + halvedNGreen * covarVecIn.at(2);
	float dotB = halvedNRed *covarVecIn.at(1) + halvedNGreen * covarVecIn.at(3);

	float sum = dotA * red + dotB * green;
	
	return sum;
}

//calculate gx for single pixel rg pair
float calc_gx(vector<float> covarVec, float sampleAvRed, float sampleAvGreen, float redPix, float greenPix)
{
	float frac = calc_frac(covarVec);

	double pwr = pwr_calc(covarVec, sampleAvRed, sampleAvGreen, redPix, greenPix);

	
	float gx = frac * exp(pwr); 
	
	float normgx = gx / frac;
	
	return normgx;
}

//populate test image vectors, send pixel pairs one at a time to gx, print the image, and calculate ROC

void testImPix(vector<float> covarVec, float sampleAvRed, float sampleAvGreen, float thresh, int& FP, int& FN, int totalPix)
{

	string num = "6";
	
	int x = 0;
	int count = 0;
	vector <float> r;
	vector<float> g;
	float redPix = 0;
	float greenPix = 0;
	vector <float>R;
	vector <float>G;
	vector <float>B;
	ifstream rgbIm;
	rgbIm.open("Training_6.txt");
	ifstream f;
	f.open("ref6.txt");
	vector<float> b;
	

	
	while (rgbIm >> x) 
	{
	  	float redPix = 0;
	  	float greenPix = 0;

	    if (count < totalPix)
	    {

	        r.push_back(x);

	    }
	    if (count >= totalPix && count < totalPix*2)
	    {
	        g.push_back(x);
	    }
	    if (count >= totalPix*2)
	    {
	    	b.push_back(x);
	    }




	    count ++;
	}

	regularize_pixels(r, g, b);

	for (int i = 0; i < r.size(); i++)
	{
		redPix = r.at(i);
		greenPix = g.at(i);
		float gx = calc_gx(covarVec, sampleAvRed, sampleAvGreen, redPix, greenPix);
		
		if (is_bigger(gx, thresh) == true)
		{
			
			R.push_back(255);
			B.push_back(255);
			G.push_back(255);
		}
		else
		{
			
			R.push_back(0);
			B.push_back(0);
			G.push_back(0);
		}
	}


	print_image(R, B, G, thresh, num);
	
	calculate_ROC(R, B, G, FP, FN, totalPix, f);
	cout << "FP= " << FP << endl;
	cout << "FN= " << FN << endl;
	float tot = FP + FN;
	float cor = r.size() - tot;
	float per = cor/ r.size();
	cout << "Percent correct= " <<  per * 100 << "% " << endl;
	cout << endl;
	


}

//test various thresholds
void test_thresholds(vector<float> covarVec, float sampleAvRed, float sampleAvGreen, int totalPix)
{
	
	float thresh;
	for (float i = 0.0; i < 100.0; i++)
		{
			int FP = 0;
			int FN = 0;
			thresh =  i / 100; //may need to up to 1000
			cout << "T = " << thresh << endl;
			testImPix(covarVec, sampleAvRed, sampleAvGreen, thresh, FP, FN, totalPix);
		}	
}


//check if gx is greater than the threshold
bool is_bigger(float gx, float thresh)
{
	return (gx > thresh);
}


//print the image pixel values to a text file
void print_image(vector<float> R, vector<float> B, vector<float> G, float thresh, string number)
{
	
	ofstream outfile;
	string num = to_string(thresh);
	string filename = number.append("_");
	filename.append(num);
	outfile.open(filename + ".txt");
	for (int i = 0; i < R.size(); i++)
	{
		outfile << R.at(i) << '\n';

	}
	for (int i = 0; i < G.size(); i++)
	{
		outfile << G.at(i) << '\n';
		
	}
	
	for (int i = 0; i < B.size(); i++)
	{
		outfile << B.at(i) << '\n';
		
	}

	outfile.close();
}


//Calculate fp and fn using the ref image data in the r vector to the R, B, G vectors
void calculate_ROC (vector<float>R, vector<float> B, vector<float> G, int& FP, int& FN, int totalPix, ifstream& f)
{
	

	vector<float> r;
	
	vector <float> ind;
	int x = 0;
	int count = 0;
	while(f >>x)
	{
		
		if (count <= totalPix)
		{
			r.push_back(x);
			
		}
	
	}

	for (int i = 0; i < R.size(); i++)
	{
		
		if (R.at(i) == 255 && r.at(i) != 255)
		{
			FP ++;
		}
		if (R.at(i) != 255 && r.at(i) == 255)
		{
			FN ++;
		}
	}

}
