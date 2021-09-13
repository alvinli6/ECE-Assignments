#include <iostream>
#include <cmath>
#include "complex.h"
#include <math.h>

using namespace std;
//default constructor initializes coordinates to 0 and NaN to false (meaning it is okay)
Complex::Complex()
{
	real = 0;
	imag = 0;
	NaN = false;
}
//constructor that has only real value
Complex::Complex(double inReal)
{
	real = inReal;
	imag = 0;
	NaN = false;
}
//constructor with both values
Complex::Complex(double inReal, double inImag)
{
	real = inReal;
	imag = inImag;
	NaN = false;
}
//setter and getters for all 3 private data members below
void Complex::setReal(double inReal)
{
	real = inReal;
}
double Complex::getReal()
{
	return real;
}
void Complex::setImag(double inImag)
{
	imag = inImag;
}
double Complex::getImag()
{
	return imag;
}
void Complex::setNaN(bool inNaN)
{
	NaN = inNaN;
}
bool Complex::getNaN()
{
	return NaN;
}

//implement operator overloads here
//addition sets the new complex object's coordinates as the sum of the two real's and the two imag's of the caller and argument
Complex Complex::operator+(Complex other)
{
	if (this->getNaN() == true|| other.getNaN() == true)
	{
		Complex err;
		err.setNaN(true);
		return err;
	}

	Complex sum;
	sum.setReal(this->getReal() + other.getReal());
	sum.setImag(this->getImag() + other.getImag());

	return(sum);
}
//subtraction sets the new complex object's coordinates as the diff of the two real's and the two imag's of the caller and argument
Complex Complex::operator-(Complex other)
{
	if (this->getNaN() == true|| other.getNaN() == true)
	{
		Complex err;
		err.setNaN(true);
		return err;
	}

	Complex diff;
	diff.setReal(this->getReal()-other.getReal());
	diff.setImag(this->getImag()-other.getImag());

	return(diff);
}

//multiplication sets the new complex object's coordinates as the product of the two real's and the two imag's of the caller and argument
Complex Complex::operator*(Complex other)
{
	Complex prod;
	prod.setReal(this->getReal()*other.getReal()-this->getImag()*other.getImag());
	prod.setImag(this->getReal()*other.getImag()+this->getImag()*other.getReal());

	return(prod);
}
//division acts the same way
Complex Complex::operator/(Complex other)
{
	Complex err;
	//if the divisor is 0 then return a new complex object with NaN set to true
	if (other.getReal() == 0 && other.getImag() == 0)
	{
		err.setNaN(true);
		return err;
	}
	Complex quotient;
	Complex conjugate;
	conjugate.setReal(other.getReal());				//conjugate is created to do the algebra
	conjugate.setImag((-1)*other.getImag());
	
	quotient.setReal((this->getReal()*conjugate.getReal()-this->getImag()*conjugate.getImag())/(other.getReal()*other.getReal()+other.getImag()*other.getImag()));
	quotient.setImag((this->getReal()*conjugate.getImag()+this->getImag()*conjugate.getReal())/(other.getReal()*other.getReal()+other.getImag()*other.getImag()));

	return(quotient);
}


//ostream operator that outputs rectangular coordinates of the Complex object
std::ostream& operator<<(ostream& os, Complex obj)
{
	if (obj.getNaN() == true)
	{
		os << "NaN";
		return os;
	}
	else
	{
		os << obj.getReal() << " + " << obj.getImag() << "j";
		return os;
	}
}

//displays Complex object in polar form. Magnitude and phasor angle are found
void Complex::displayPolar()
{
	if (this->getNaN() == true)
	{
		cout << "NaN" << endl;
	}
	else
	{
		double mag = sqrt(this->getReal()*this->getReal()+this->getImag()*this->getImag());
		double angle = atan(this->getImag()/this->getReal());
		cout << mag << " < " << angle << endl;
	}
}

//same as <<
void Complex::displayRect()
{
	if (this->getNaN() == true)
	{
		cout << "NaN" << endl;
	}
	else
	{
		cout << this->getReal() << " + " << this->getImag() << "j" << endl;
	}
}

//set both coordinate values for a complex number object
void Complex::setComplex(double inR, double inI)
{
	this->setReal(inR);
	this->setImag(inI);
}

//assignment operator that allows one complex object to equal the other in terms of each data member
Complex& Complex::operator=(Complex other)
{
	this->setReal(other.getReal());
	this->setImag(other.getImag());
	this->setNaN(other.getNaN());

	return(*this);
}