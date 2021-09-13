#ifndef COMPLEX_H
#define COMPLEX_H
#include <iostream>
class Complex;

class Complex
{
public:
	Complex();		//default constructor
	Complex(double);	//constructor with real value
	Complex(double, double);	//constructor with both values
	void setReal(double);	//real setter
	double getReal();		//real getter
	void setImag(double);	//imaginary setter
	double getImag();		//imaginary getter
	void setNaN(bool inNaN);	//NaN setter
	bool getNaN();				//NaN getter
	Complex operator+(Complex);	//overload for addition, subtraction, multiplication, division, and assignment of Complex objects
	Complex operator-(Complex);
	Complex operator*(Complex);
	Complex operator/(Complex);
	Complex& operator=(Complex);
	friend std::ostream& operator<<(std::ostream&, Complex);	//overload operator for output of a complex object
	void displayPolar();	//displaying polar version of complex objects
	void displayRect();		//should be same as << operator
	void setComplex(double, double);	//set both values 
private:	//respective data
	double real;
	double imag;
	bool NaN;
};

	std::ostream& operator<<(std::ostream&, Complex);
#endif