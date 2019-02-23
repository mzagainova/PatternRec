#include <vector>
using namespace std;

float errorBound(float B, float mu1, float mu2, float sd1, float sd2);
vector<float> calcChernoff(float mu1, float mu2, float sd1, float sd2);
float calcBhattach(float mu1, float mu2, float sd1, float sd2);
