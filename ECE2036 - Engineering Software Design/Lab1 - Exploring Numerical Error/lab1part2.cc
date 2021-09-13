/* Alvin Li
 * ECE2036
 * This file holds a main function that calls other functions to calculated quadratic roots of a specified equation, and output those roots' %error with respect to the actual value.
 */

#include <iostream>
#include <cmath>

using namespace std;

float quadraticFloatRoots(bool sign, float a, float b, float c)		//uses the float datatype to calculate a root of a quadratic equation. The boolean argument passed through decides which root it calculates
{
	float root;
	if (sign == true)
	{
		root = ((-b)+sqrt(b*b-4*a*c))/(2*a);
	}
	else if (sign == false)
	{
		root = ((-b)-sqrt(b*b-4*a*c))/(2*a);
	}
	return root;
};

double quadraticDoubleRoots(bool sign, double a, double b, double c)	//uses the double datatype to calculate a root of a quadratic equation. Boolean works same way as with quadraticFloatRoots
{
        double root;
        if (sign == true)
        {
                root = ((-b)+sqrt(b*b-4*a*c))/(2*a);
        }
        else if (sign == false)
        {
                root = ((-b)-sqrt(b*b-4*a*c))/(2*a);
        }
	return root;
};

float floatError1(float inFloat)		//calculates error of the first root in float
{
	float error = abs((100*(-0.001-inFloat))/-0.001);
	return error;
};

float floatError2(float inFloat)		//calculates error of the second root in float
{
        float error = abs((100*(-3000-inFloat))/-3000);
        return error;
};

double doubleError1(double inDouble)		//calculates error of the first root in double
{
        double error = abs((100*(-0.001-inDouble))/-0.001);
        return error;
};

double doubleError2(double inDouble)		//calculates error of the second root in double
{
        double error = abs((100*(-3000-inDouble))/-3000);
        return error;
};
int main()					//main function that calls the quadraticRoot functions to calculate both the roots in float and double, then compare with the actual values to output %error
{
	float actualFloatRoot1 = -0.001;	//actual values are declared for comparison later
	double actualDoubleRoot1 = -0.001;
	float actualFloatRoot2 = -3000;
	double actualDoubleRoot2 = -3000;

	float calcFloatRoot1;			//variables are created to set equal to the returned data from called quadraticRoot functions
	float calcFloatRoot2;
	double calcDoubleRoot1;
	double calcDoubleRoot2;

	bool pos = true;			//arguments used to determine which root is calculated at the "+/-" part of the numerator in the quadratic formula
	bool neg = false;

	calcFloatRoot1 = quadraticFloatRoots(pos, 1, 3000.001, 3);	//calcRoot variables here store the value of the returned data from the quadraticRoot functions
	calcFloatRoot2 = quadraticFloatRoots(neg, 1, 3000.001, 3);

	calcDoubleRoot1 = quadraticDoubleRoots(pos, 1, 3000.001, 3);
	calcDoubleRoot2 = quadraticDoubleRoots(neg, 1, 3000.001, 3);

	cout << "Using float data types:" << endl;		//output data to user
	cout << "x1 = " << calcFloatRoot1 << "\t% error = " << floatError1(calcFloatRoot1) << endl;
	cout << "x2 = " << calcFloatRoot2 << "\t% error = " << floatError2(calcFloatRoot2) << endl;

	cout << endl;

	cout << "Using double data types:" << endl;
        cout << "x1 = " << calcDoubleRoot1 << "\t% error = " << doubleError1(calcDoubleRoot1) << endl;
	cout << "x2 = " << calcDoubleRoot2 << "\t% error = " << doubleError2(calcDoubleRoot2) << endl;		


} // end main
