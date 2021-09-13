/* Name: Alvin Li
 * ECE203
 * This file holds a main function that calculates 25 factorials
 */

#include <iostream>

using namespace std;

int calcFactorial(int inNum)		//factorial function that calculates the factorial of any integer sent in as an argument
{
	int start = inNum-1;		//the first number that the argument will be multiplied by

	while (start > 1)		//loop that continues to multiply the decrementing 'start' variable until it reaches 1, completing the factorial
	{
		inNum *= start;
		start--;
	}
	return inNum;			//the original argument is returned to the user as a new value
};

long int calcLongFactorial(long int inNum)	//the same factorial function but using long int an argument instead of int
{
       	long int start = inNum-1;

        while (start > 1)
        {
                inNum *= start;
                start--;

        }
        return inNum;
};

int main()				//main function
{
	for (int i = 1; i < 26; i++)						//the two for loops iterate through all numbers 1-25 and output the factorial of each. This is done to compare the threshold for which each data type holds
	{
		cout << i << "!" << " = " << calcFactorial(i) << endl;
	}

	for (long int i = 1; i < 26; i++)
	{
		cout << i << "!" << " = " << calcLongFactorial(i) << endl;
	}


} //end main
