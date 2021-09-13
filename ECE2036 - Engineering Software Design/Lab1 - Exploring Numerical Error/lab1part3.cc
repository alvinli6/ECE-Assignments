/* Alvin Li
 * ECE2036
 * This file holds the main function that calculates a number of terms in the power series for e^x based on user input
 */

#include <iostream>
#include <cmath>
#include <math.h>
#include <iomanip>

using namespace std;

double e = 2.71828182845904523536028747135266249775724709369995;	//global variable that is used for calculating error

float floatError(float inApprox)					//error function that calculates using float
{
	float error = (100*abs(e-inApprox))/(e);
	return error;	
};
 
void displayFloat(int numTerms, int inPower)				//function that takes in user input to calculate and output the cumulative sums of terms per iteration, until the inputted amount of terms is reached
{	
	if (numTerms > 0)
	{
		cout << "---------------------" << endl;
		cout << "#terms = 1" << endl;
		cout << "approx = 1" << endl;
		cout << "%error = " << floatError(1) << endl;		//this segment will always remain constant and is therefore omitted from the for loop below. The first term of the power series does not follow a pattern
		float accumulatedValue = 1;
		float currentTerm = 1;			//this variable is what multiplies with the previous term to produce a new term that can be added to the cumulative sum
		for (int i = 1; i < numTerms; i++)
		{
			cout << "---------------------" << endl;
			
			cout << "#terms = " << i+1 << endl;		
			
			currentTerm *= ((float)inPower/i);		//by mutiplying previous term by x/(new denominator), the new term is calculated
			accumulatedValue += currentTerm;

			cout << "approx = " << fixed << setprecision(32) << accumulatedValue << endl;	//setPrecision has 32 passed into it as an argument because floats have 32-bits of precision
			
			cout << "%error = " << floatError(accumulatedValue) << endl; 	
		}
	}	
	else
	{
		cout << "Please run program again and try a positive argument for number of terms." << endl;	//cannot be a negative number of terms
	}
};

double doubleError(double inApprox)	//error function that calculates using double
  {
          double error = (100*abs(e-inApprox))/(e);
          return error;
  };
 
  void displayDouble(int numTerms, int inPower)		//function that takes in user input to calculate and output the cumulative sums of terms per iteration, until the inputted amount of terms is reached. Calculates in double
  {
          if (numTerms > 0)
          {
                  cout << "---------------------" << endl;
                  cout << "#terms = 1" << endl;
                  cout << "approx = 1" << endl;
                  cout << "%error = " << floatError(1) << endl;		//this segment will always remain constant and is therefore omitted from the for loop below. The first term of the power series does not follow a pattern
                  double accumulatedValue = 1;
                  double currentTerm = 1;		//this variable is what multiplies with the previous term to produce a new term that can be added to the cumulative sum
                  for (int i = 1; i < numTerms; i++)
                  {
                          cout << "---------------------" << endl;
 
                          cout << "#terms = " << i+1 << endl;
 
                          currentTerm *= ((double)inPower/i);	//by mutiplying previous term by x/(new denominator), the new term is calculated
                          accumulatedValue += currentTerm;
 
                          cout << "approx = " << fixed << setprecision(64) << accumulatedValue << endl;	//setPrecision has 64 passed into it as an argument because floats have 32-bits of precision
 
                          cout << "%error = " << doubleError(accumulatedValue) << endl;
                  }
          }
          else
          {
                  cout << "Please run program again and try a positive argument for number of terms." << endl;
          }
  };

int main()			//main function that prompts user for input to determine amount of terms and power to raise e to, in order to caluclate terms using displayFloat and displayDouble
{
	cout << "Input the number of terms in the power series using FLOATS: " << endl;
	int numTerms;
	cin >> numTerms;
	cout << "Input the power to raise e to" << endl;
	int power;
	cin >> power;			//asking for input for arguments of function

	displayFloat(numTerms, power);	
	
	cout << "Input the number of terms in the power series using DOUBLES: " << endl;
	int numTerms1;
	cin >> numTerms1;
	cout << "Input the power to raise e to" << endl;
	int power1;
	cin >> power1;	//asking for input for arguments of function

	displayDouble(numTerms1, power1);

	return 0;
} // end main
