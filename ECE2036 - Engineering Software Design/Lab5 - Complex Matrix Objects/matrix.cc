#include <iostream>
#include "complex.h"
#include "matrix.h"
#include <string>

using namespace std;

//constructor that initializes all dimensions and size of the matrix. allocates memory for complex pointer from the heap
Matrix::Matrix(int inR, int inC)
{
	dimension = inR*inC;
	row = inR;
	column = inC;

	if (inR == 0 && inC == 0)
	{
		this->setComplexPtr(NULL);
	}
	else
	{
		this->setComplexPtr(new Complex[dimension]);
	}
	
}

//destructor frees up memory used by complexPtr
Matrix::~Matrix()
{
	delete[] this->complexPtr;
}

//copy constructor that creates another matrix identical to the one passed as an argument
Matrix::Matrix(const Matrix & other)
{
	this->setRows(other.getRows());
	this->setColumns(other.getColumns());
	this->setDimension(other.getRows()*other.getColumns());
	if (this->getRows() == 0 && this->getColumns() == 0)
	{
		this->setComplexPtr(NULL);
	}
	else
	{
		this->setComplexPtr(new Complex[this->getDimension()]);
	}

	for (int i = 0; i < this->getDimension(); i++)
	{
		this->complexPtr[i] = other.getComplexPtr()[i];
	}
}

//setters and getters for dimensions, size, and complexPtr
int Matrix::getRows() const
{
	return this->row;
}

void Matrix::setRows(int inR)
{
	this->row = inR;
}

int Matrix::getColumns() const
{
	return this->column;
}

void Matrix::setColumns(int inC)
{
	this->column = inC;
}

int Matrix::getDimension() const 
{
	return this->dimension;
}

void Matrix::setDimension(int inD)
{
	this->dimension = inD;
}

Complex* Matrix::getComplexPtr() const
{
	return this->complexPtr;
}

void Matrix::setComplexPtr(Complex* inP)
{
	this->complexPtr = inP;
}

//indexing operator that returns the complex object at the coordinate calculated by the formula below. No 0 indexing
Complex& Matrix::operator()(int inR, int inC) const
{
	return this->getComplexPtr()[(inR-1)*(this->getColumns()) + inC-1];
}

//sums all elements at same indexes between two matrices and produces a new matrix object as the answer
Matrix Matrix::operator+(const Matrix& other) const
{
	
	if (this->row == other.getRows() && this->column == other.getColumns())
	{
		Matrix sum(this->row, this->column);
		for (int i = 0; i < this->dimension; i++)
		{
			sum.complexPtr[i] = this->complexPtr[i] + other.complexPtr[i];
		}
		return sum;
	}
	else
	{
		Matrix sum(0, 0);
		return sum;
	}
}

//subtracts all elements at same indexes between two matrices and produces a new matrix object as the answer
Matrix Matrix::operator-(const Matrix& other) const
{
	
	if (this->row == other.getRows() && this->column == other.getColumns())
	{
		Matrix diff(this->row, this->column);
		for (int i = 0; i < this->dimension; i++)
		{
			diff.complexPtr[i] = this->complexPtr[i] - other.complexPtr[i];
		}
		return diff;
	}
	else
	{
		Matrix diff(0, 0);
		return diff;
	}
}

//multiples matrices using dot products between columns and rows
Matrix Matrix::operator*(const Matrix & other) const
{
	
	if (this->getColumns() == other.getRows())
	{
		Matrix prod(this->row, other.getColumns());		//product matrix may have new dimensions differing from both of the factors
		for (int i = 1; i <= prod.getRows(); i++)
		{
			for (int j = 1; j <= prod.getColumns(); j++)
			{
				Complex acc;
				for (int k = 1; k <= this->getColumns(); k++)
				{
					acc = acc + (*this)(i, k)*other(k, j);
				}
				prod(i, j) = acc;
			}
		}
		return prod;
	}
	else
	{
		return Matrix(0,0);
	}
	
}

//assignment operator that acts almost like the copy constructor. sets complexPtr equal to the argument's and all dimensions equal
Matrix& Matrix::operator=(const Matrix& other)
{
	if (this->getDimension() != other.getDimension())
	{
		delete[] this->complexPtr;	
		this->complexPtr = new Complex[other.getDimension()];	
		this->dimension = other.getDimension();
	}
	for (int i = 0; i < dimension; i++)
	{
		this->complexPtr[i] = other.complexPtr[i];
	}
	
	this->row = other.row;
	this->column = other.column;

	return(*this);
}

//transpose. Same as void transpose() function so creates a copy matrix that uses the transpose function and returns it
Matrix Matrix::operator~() const
{
	Matrix after = Matrix(*this);
	after.transpose();
	return after;
}

//conjugate transpose. First creates a copy of the matrix, then subtracts twice the magnitude of the imaginary value from itself. Transposes at the end.
Matrix Matrix::operator!() const
{
	Matrix after = (*this);

	for (int i = 0; i < this->getDimension(); i++)
	{
		double subThis = after.complexPtr[i].getImag();		//for example, if complex number is 2+2j
		subThis = subThis*2;								//subThis is 4

		Complex toBeSubtracted(0, subThis);					//create a complex number 0+4j from values above

		after.complexPtr[i] = after.complexPtr[i]-toBeSubtracted;	//subtract 2+2j - (0+4j) to get complex number 2 - 2j as conjugate
	}

	after.transpose();		//transpose matrix after done and return
	return after;
	
}

//transpose that utilizes a temporary complex variable to switch values over from columns to rows
void Matrix::transpose() const 
{
	for (int i = 1; i <= this->getRows(); i++)
	{
		for (int j = i; j <= this->getColumns(); j++)
		{
			Complex tmp = (*this)(i,j);
			(*this)(i,j) = (*this)(j,i);
			(*this)(j,i) = tmp;
		}
	}
}

//output matrix
void Matrix::printMatrix() const 
{
	cout << (*this) << endl;
}

//based on different cases of the Complex object values at the coordinate as indexed, different forms of output will occur.
std::ostream& operator<< (std::ostream& os, const Matrix & obj)
{
	if (obj.getRows() == 0 || obj.getColumns() == 0 || obj.getComplexPtr() == NULL)
	{
		os << "Matrix mismatch error!\nThis matrix has zero elements" << endl;
	}
	else
	{
		for (int i = 1; i <= obj.getRows(); i++)
		{
			for (int j = 1; j <= obj.getColumns(); j++)
				{
					if (obj(i,j).getReal() != 0 && obj(i,j).getImag() != 0)
					{
						os << fixed << setprecision(6) << obj(i,j) << "\t\t";
					}
					else if (obj(i,j).getReal() == 0 && obj(i,j).getImag() != 0)
					{
						os << fixed << setprecision(6) << obj(i,j).getImag() << "j" << "\t\t";
					}
					else if (obj(i,j).getReal() != 0 && obj(i,j).getImag() == 0)
					{
						os << fixed << setprecision(6) << obj(i,j).getReal() << "\t\t";
					}
					else if (obj(i,j).getReal() == 0 && obj(i,j).getImag() == 0)
					{
						os << fixed << setprecision(6) << "0.00000000000000" << "\t\t\t";
					}
				}
			os << endl;		//nextLine at the end of a matrix row
		}
	}
	
	return os;		//return full matrix
}

//multiplying a complex object with a whole matrix
Matrix operator*(Complex mult, Matrix& obj)
{
	Matrix prod(obj.getRows(), obj.getColumns());

	for (int i = 0; i < obj.getDimension(); i++)
	{
		prod.complexPtr[i] = mult*obj.complexPtr[i];
	}

	return prod;
}

//same as function above with arguments' order switched up. Same answer will be obtained.
Matrix operator*(Matrix& obj, Complex mult)
{
	Matrix prod(obj.getRows(), obj.getColumns());

	for (int i = 0; i < obj.getDimension(); i++)
	{
		prod.complexPtr[i] = mult*obj.complexPtr[i];
	}

	return prod;
}
/*
Matrix Matrix::operator*(Complex mult)
{
	Matrix prod(this->getRows(), this->getColumns());

	for (int i = 0; i < this->getDimension(); i++)
	{
		prod.complexPtr[i] = mult*this->complexPtr[i];
	}

	return prod;
}

Complex Matrix::operator*(Matrix& obj)
{
	Matrix prod(obj.getRows(), obj.getColumns());

	for (int i = 0; i < obj.getDimension(); i++)
	{
		prod.complexPtr[i] = (*this)*obj.complexPtr[i];
	}

	return prod;
}*/