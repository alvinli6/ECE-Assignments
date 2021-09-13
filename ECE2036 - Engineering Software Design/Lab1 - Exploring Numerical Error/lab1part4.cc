/* Alvin Li
 * ECE2036
 * This file holds the main function that utilizes global functions to output the sum of 100 terms of the power series going forward and backward using both float and double data types.
 */


#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

double e = 2.71828182845904523536028747135266249775724709369995;	//global variable used for %error calculation
float arrayFloat[100];		//100 element float array
double arrayDouble[100];	//100 element double array

float floatError(float inApprox)		//float function calculating error
{
        float error = (100*abs(e-inApprox))/(e);
        return error;
};

void displayFloat(int numTerms, int inPower)		//function that calculates and outputs the sum of 100 terms of the power series using float data types
{
        if (numTerms > 0)
        {
		float nextTerm = 1;
		arrayFloat[0] = nextTerm;		//first element of the array is set to 1
		for (int i = 1; i < numTerms; i++)
		{
			nextTerm *= ((float)inPower/i);			//each element is now calculated by multiplying the previous element with inPower/i
			arrayFloat[i] = nextTerm;
		}
          	
		float sumFloats1 = 0;		//sum of floats going forward and backward are created as variables initialized to 0
		float sumFloats2 = 0;

                for (int i = 0; i < numTerms; i++)	//for loops that sum all elements in the array, going in different directions
                {
              	 	sumFloats1 += arrayFloat[i];              
                }
		for (int i = numTerms-1; i >= 0; i--)
                {
                        sumFloats2 += arrayFloat[i];
                }		
		cout << "Floats------------------------------" << endl;		//output to user
		cout << "forward approx = " << setprecision(32) << sumFloats1 << endl;
		cout << "forward %error = " << floatError(sumFloats1) << endl;	
        	cout << "backward approx = " << setprecision(32) << sumFloats2 << endl;
		cout << "backward %error = " << floatError(sumFloats2) << endl;
	}
        else
        {
                cout << "Please run program again and try a positive argument for number of terms." << endl;
        }
};

double doubleError(double inApprox)		 //float function calculating error
{
          double error = (100*abs(e-inApprox))/(e);
          return error;
};

void displayDouble(int numTerms, int inPower) 	//function that calculates and outputs the sum of 100 terms of the power series using float data types
{
        if (numTerms > 0)
        {
                double nextTerm = 1;
                arrayDouble[0] = nextTerm;		 //first element of the array is set to 1
                for (int i = 1; i < numTerms; i++)
                {
                        nextTerm *= ((double)inPower/i);	//each element is now calculated by multiplying the previous element with inPower/i
                        arrayDouble[i] = nextTerm;
                }

                float sumDoubles1 = 0;		//sum of floats going forward and backward are created as variables initialized to 0
                float sumDoubles2 = 0;

                for (int i = 0; i < numTerms; i++)		//for loops that sum all elements in the array, going in different directions
                {
                        sumDoubles1 += arrayDouble[i];
                }
                for (int i = numTerms-1; i >= 0; i--)
                {
                        sumDoubles2 += arrayDouble[i];
                }
                cout << "Doubles------------------------------" << endl;		 //output to user
                cout << "forward approx = " << setprecision(64) << sumDoubles1 << endl;
                cout << "forward %error = " << doubleError(sumDoubles1) << endl;
                cout << "backward approx = " << setprecision(64) << sumDoubles2 << endl;
                cout << "backward %error = " << doubleError(sumDoubles2) << endl;
        }
        else
        {
                cout << "Please run program again and try a positive argument for number of terms." << endl;
        }
};

int main()	//main functions that directly call the global sum functions; 100 terms are automatically passed in as a parameter
{
        displayFloat(100, 1);
	displayDouble(100, 1);
        return 0;
} // end main

