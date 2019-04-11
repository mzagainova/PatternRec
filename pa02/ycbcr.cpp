#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
using namespace std;

void populate_pixel_vectors(vector<float> &cr, vector<float> &y, vector<float> &cb,  int totalPix, vector<float>& vectcr, vector<float>& vectcb, vector<float>& vecty);
void convert_to_ycbcr(int  totalPix, vector <float> vectcr,vector <float>  vectcb,vector <float>  vecty);
void create_2d_array(vector<float>& vectCR, vector<float>& vectY, vector<float>& vectCB, vector<float>& counter, vector<float>& vectBWX, vector<float>& vectBWY, int height, int width);
void regularize_pixels(vector<float>& vectcr, vector<float>& vectcb, vector<float>& vecty);
void get_skin_distribution(vector<float>& crVec, vector<float>& cbVec, vector<float>& yVec, vector<float>& vectcr, vector<float>& vectcb, vector<float>& vecty, vector<float>& counter);
void calculate_sample_averages(vector<float> crVec, vector<float> cbVec, float& sampleAvCR, float& sampleAvCB, int numSamples);
void calculate_sample_covariance(vector<float> crVec, vector<float> cbVec, vector<float>& covarVec, float& sampleAvCR, float& sampleAvCB, int numSamples, int& counter);
void test_thresholds(vector<float> covarVec, float sampleAvCR, float sampleAvCB, int totalPix);
void testImPix(vector<float> covarVec, float sampleAvCR, float sampleAvCB, float thresh, int& FP, int& FN, int totalPix);
float calc_gx(vector<float> covarVec, float sampleAvCR, float sampleAvCB, float crPix, float cbPix);
float calc_frac(vector<float> covarVec);
float calc_determinant(vector<float> covarVec);
double pwr_calc(vector<float> covarVec, float sampleAvCR, float sampleAvCB, float crPix, float cbPix);
void calc_inverse(vector<float> covarVec, vector<float>& covarVecIn);
bool is_bigger(float gx, float thresh);
void print_image(vector<float> CR, vector<float> Y, vector<float> CB, float thresh, string number);
void calculate_ROC (vector<float>CR, vector<float> Y, vector<float> CB, int& FP, int& FN, int totalPix,  vector<float> R, vector<float> G, vector<float> B);

int main(){

	vector<float> crVec;
	vector<float> cbVec;
	vector<float> yVec;
	vector<float> vectBWX;
	vector<float> vectBWY;
	vector<float> vectCR;
	vector<float> vectCB;
	vector<float> vectY;
	vector<float> vectcr;
	vector<float> vectcb;
	vector<float> vecty;
	vector<float> counter;
	vector<float> covarVec;

	int totalPix = 2583552;
	int width = 1856;
	int height = 1392;
	int pix = 0;
	float sampleAvCR = 0;
	float sampleAvCB = 0;




	populate_pixel_vectors(vectCR, vectCB, vectY, totalPix, vectcr, vectcb, vecty);

	convert_to_ycbcr(   totalPix, vectcr, vectcb,  vecty);
	
	create_2d_array( vectCR,  vectCB,  vectY,  counter,  vectBWX, vectBWY, height, width);



	regularize_pixels(vectcr, vectcb, vecty);
	

 
	get_skin_distribution(crVec, cbVec, yVec, vectcr, vectcb, vecty, counter);
	
	int numSamples = crVec.size();
	calculate_sample_averages(crVec, cbVec, sampleAvCR, sampleAvCB, numSamples);
	
	int c = 0;
	calculate_sample_covariance( crVec,  cbVec,  covarVec, sampleAvCR,  sampleAvCB, numSamples, c);




	test_thresholds(covarVec, sampleAvCR, sampleAvCB, totalPix);



}

//uppercase populates the r g and b from bw images
//lowercase populates rgb from rgb image
void populate_pixel_vectors(vector<float> &cr, vector<float> &y, vector<float> &cb,  int totalPix, vector<float>& vectcr, vector<float>& vectcb, vector<float>& vecty)
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
	  
	    if (count <totalPix)
	    {
	        cr.push_back(x);

	    }
	    if (count >= totalPix && count <totalPix*2 )
	    {
	        cb.push_back(x);
	    }
	    if(count >= totalPix*2 )
	    {
	        y.push_back(x);
	    }

	    count ++;
	}

	count =0;
	while (rbgIm >>n){
	    if (count <totalPix)
	    {
	        vectcr.push_back(n);

	    }
	    if (count >= totalPix && count < totalPix*2)
	    {
	        vectcb.push_back(n);
	    }
	    if(count >= totalPix*2)
	    {
	        vecty.push_back(n);
	    }

	    count++;
	 }

	 
}


//convert rgb to ycbcr for colored image
void convert_to_ycbcr(int  totalPix, vector <float> vectcr,vector <float>  vectcb,vector <float>  vecty)

{
	vector <float> r;
	vector < float> b;
	vector <float> g;


	for (int i = 0; i < vectcr.size(); i++)
	{
		r.push_back(vectcr.at(i));
		g.push_back(vectcb.at(i));
		b.push_back(vecty.at(i));

	}

	for (int i = 0; i < vectcr.size(); i++)
	{
		vectcb.at(i) = ( (-.169 * r.at(i)) + (-.332 *g.at(i)) + (.5 * b.at(i)));

		//green vector will contain cr
		vectcr.at(i) = ((.5 * r.at(i)) + (-.419 * g.at(i)) + (-0.081 *b.at(i)));
		//blue vector will contain y

		vecty.at(i) = ((.299 * r.at(i)) + (0.587 * g.at(i)) + (0.114 * b.at(i)));	

	}


}
//kept bw image in rgb space, check if white. if it is find it's index
void create_2d_array(vector<float>& vectCR, vector<float>& vectY, vector<float>& vectCB, vector<float>& counter, vector<float>& vectBWX, vector<float>& vectBWY, int height, int width)
{
	

	int z = 0;
	for (int y = 0; y < width; y++)
	{
	    for (int x=0; x < height; x++)
	    {

	        if (vectCR.at(z) == 255 && vectCB.at(z) ==255 && vectY.at(z) == 255)
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
void regularize_pixels(vector<float>& vectcr, vector<float>& vectcb, vector<float>& vecty)
{
	for (int i = 0; i < vectcb.size(); i++)
	{
	   

	    int pixTot = vectcr.at(i) + vectcb.at(i) + vecty.at(i);
	   
	    if (vectcr.at(i) > 0)
	        vectcr.at(i) = float(vectcr.at(i) / pixTot);
	    if (vectcb.at(i) >0)
	     	vectcb.at(i) = float(vectcb.at(i)/pixTot);
	 	
	}
}

//get skin distribution pixels using the white pixel locations from the bw image, add to red green and blue vectors
void get_skin_distribution(vector<float>& crVec, vector<float>& cbVec, vector<float>& yVec, vector<float>& vectcr, vector<float>& vectcb, vector<float>& vecty, vector<float>& counter)
{
	for (int i = 0; i < counter.size(); i++)
	{
		
	    crVec.push_back(vectcr.at(counter.at(i)));
	    cbVec.push_back(vectcb.at(counter.at(i)));
	    yVec.push_back(vecty.at(counter.at(i)));
	    

	}

	
}

//calculate the sample mean for red and green values in faces
void calculate_sample_averages(vector<float> crVec, vector<float> cbVec, float& sampleAvCR, float& sampleAvCB, int numSamples)
{
	float crTot = 0;
	float cbTot = 0;
	for (int i = 0; i < crVec.size(); i++)
	{
	    crTot += crVec.at(i);
	    cbTot += cbVec.at(i);
	}
	sampleAvCR = crTot/numSamples;
	sampleAvCB = cbTot/numSamples;

	
}

// calculate the sample variance for teh red and green values in faces
void calculate_sample_covariance(vector<float> crVec, vector<float> cbVec, vector<float>& covarVec, float& sampleAvCR, float& sampleAvCB, int numSamples, int& counter)
{
	int sumCR = 0;
	int sumCB = 0;
	for (int i = 0; i < crVec.size(); i++)
	{
		float diffCR = crVec.at(i) - sampleAvCR;
		float diffCB = cbVec.at(i) - sampleAvCB;

		float sqCR = diffCR * diffCR;
		float crcb = diffCR * diffCB;
		float sqCB = diffCB * diffCB;
		//if counter = 0, then this is the first sample and nothing is in the vector. Populate it
		if (counter == 0)
		{
			covarVec.push_back(sqCR);
			covarVec.push_back(crcb);
			covarVec.push_back(crcb);
			covarVec.push_back(sqCB);
		}
		//if counter >0, vector is populated, so sum the values
		if (counter > 0)
		{
			for (int j = 0; j <  covarVec.size(); j++)
			{
				if (j == 0)
					covarVec.at(j) += sqCR;
				if (j == 1)
					covarVec.at(j) += crcb;
				if (j == 2)
					covarVec.at(j) += crcb;
				if(j == 3)
					covarVec.at(j) += sqCB;
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

//test various thresholds
void test_thresholds(vector<float> covarVec, float sampleAvCR, float sampleAvCB, int totalPix)
{
	
	float thresh;
	for (float i = 0.0; i < 100.0; i++)
		{
			int FP = 0;
			int FN = 0;
			thresh =  i / 100; //may need to up to 1000
			cout << "T = " << thresh << endl;
			testImPix(covarVec, sampleAvCR, sampleAvCB, thresh, FP, FN, totalPix);
		}	
}

void testImPix(vector<float> covarVec, float sampleAvCR, float sampleAvCB, float thresh, int& FP, int& FN, int totalPix)
{

	string num = "3";
	
	int x = 0;
	int count = 0;
	vector <float> cr;
	vector<float> cb;
	float crPix = 0;
	float cbPix = 0;
	vector <float>CR;
	vector <float>CB;
	vector <float>Y;
	vector <float>R;
	vector<float> B;
	vector<float> G;
	ifstream ycbcrIm;
	ycbcrIm.open("Training_3.txt");
	ifstream f;
	f.open("ref3.txt");
	int counter = 0;
	while (f >> x)
	{
		if (counter < totalPix)
		{
			R.push_back(x);
		}
		if (counter >=totalPix && counter < totalPix*2)
		{
			G.push_back(x);
		}
		if (counter >= totalPix*2)
		{
			B.push_back(x);
		}

		counter+=1;
	}
;

	vector<float> y;
	

	
	while (ycbcrIm >> x) 
	{
	  	float CRPix = 0;
	  	float CBPix = 0;

	    if (count < totalPix)
	    {

	        cr.push_back(x);

	    }
	    if (count >= totalPix && count < totalPix*2)
	    {
	        cb.push_back(x);
	    }
	    if (count >= totalPix*2)
	    {
	    	y.push_back(x);
	    }




	    count ++;
	}
	
	regularize_pixels(cr, cb, y);


	for (int i = 0; i < cr.size(); i++)
	{
		crPix = cr.at(i);
		cbPix = cb.at(i);
		float gx = calc_gx(covarVec, sampleAvCR, sampleAvCB, crPix, cbPix);
	
		
		if (is_bigger(gx, thresh) == true)
		{
			
			CR.push_back(255);
			CB.push_back(255);
			Y.push_back(255);
		}
		else
		{
			
			CR.push_back(0);
			CB.push_back(0);
			Y.push_back(0);
		}
	}


	print_image(CR, Y, CB, thresh, num);
	
	calculate_ROC(CR, Y, CB, FP, FN, totalPix,  R, G, B);
	cout << "FP= " << FP << endl;
	cout << "FN= " << FN << endl;
	float tot = FP + FN;
	float cor = cr.size() - tot;
	float per = cor/ cr.size();
	cout << "Percent correct= " <<  per * 100 << "% " << endl;
	cout << endl;
	


}

float calc_gx(vector<float> covarVec, float sampleAvCR, float sampleAvCB, float crPix, float cbPix)
{
	float frac = calc_frac(covarVec);
	
	
	double pwr = pwr_calc(covarVec, sampleAvCR, sampleAvCB, crPix, cbPix);
	
	
	float gx = frac * exp(pwr); 
	
	float normgx = gx / frac;
	
	return normgx;
	
}

//calculate the fraction before e
float calc_frac(vector<float> covarVec)
{
	float determinant = calc_determinant(covarVec);
	determinant = sqrt(determinant);
	
	float doublePi = 3.1415926 * 2;

	return (1/(determinant *doublePi));

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
//calculate the power that e is raised to
double pwr_calc(vector<float> covarVec, float sampleAvCR, float sampleAvCB, float crPix, float cbPix)
{
	vector<float> covarVecIn = {0, 0, 0, 0};
	calc_inverse(covarVec, covarVecIn);
	



	float cr = crPix - sampleAvCR;
	float cb = cbPix - sampleAvCB;
	

	float halvedNCR = -0.5 * (crPix-sampleAvCR);
	float halvedNCB = -0.5 * (cbPix-sampleAvCB);


	float dotA = halvedNCR *covarVecIn.at(0) + halvedNCB * covarVecIn.at(2);
	float dotB = halvedNCR *covarVecIn.at(1) + halvedNCB * covarVecIn.at(3);

	float sum = dotA * cr + dotB * cb;
	
	return sum;
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

//check if gx is greater than the threshold
bool is_bigger(float gx, float thresh)
{
	return (gx > thresh);
}

//print the image pixel values to a text file
void print_image(vector<float> CR, vector<float> Y, vector<float> CB, float thresh, string number)
{
	
	ofstream outfile;
	string num = to_string(thresh);
	string filename = number.append("_");
	filename.append(num);
	outfile.open(filename + ".txt");
	for (int i = 0; i < CR.size(); i++)
	{
		outfile << CR.at(i) << '\n';

	}
	for (int i = 0; i < CB.size(); i++)
	{
		outfile << CB.at(i) << '\n';
		
	}
	
	for (int i = 0; i < Y.size(); i++)
	{
		outfile << Y.at(i) << '\n';
		
	}

	outfile.close();
}

//Calculate fp and fn using the ref image data in the r vector to the R, B, G vectors
void calculate_ROC (vector<float>CR, vector<float> Y, vector<float> CB, int& FP, int& FN, int totalPix, vector<float> R, vector<float> G, vector<float> B)
{
	

	
	vector <float> ind;
	int x = 0;
	int count = 0;

	
	for (int i = 0; i < CR.size(); i++)
	{
		
		if (CR.at(i) == 255 && R.at(i) != 255)
		{
			FP ++;
		}
		if (CR.at(i) != 255 && R.at(i) == 255)
		{
			FN ++;
		}
	}

}
