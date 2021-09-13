#ifndef MATRIX_H
#define MATRIX_H
#include <iostream>
#include <iomanip>
#include "complex.h"
#include <string>
#define MATRIX_FIELD 30

class Matrix;	//intention to define class Matrix
std::ostream& operator<< (std::ostream &, const Matrix &);
Matrix operator*(Complex, Matrix &);
Matrix operator*(Matrix &, Complex);

class Matrix
{
public:
	friend std::ostream& operator<< (std::ostream &, const Matrix &);	//output a matrix in it's entirety
	friend Matrix operator*(Complex, Matrix &);	//complex object on left side multiplied by matrix on right side
	friend Matrix operator*(Matrix &, Complex);	//vice versa

	Matrix(int, int);		//constructor that creates a matrix with rows and columns defined
	~Matrix();				//destructor frees pointer to matrix
	Matrix(const Matrix &);	//copy constructor

	int getRows() const;	//setters and getters for all values including complex pointer
	void setRows(int);
	int getColumns() const;
	void setColumns(int);
	int getDimension() const;
	void setDimension(int);

	Complex* getComplexPtr() const;
	void setComplexPtr(Complex*);

	Complex& operator()(int, int) const;		//indexing of a matrix that returns the complex object at the location
	Matrix operator+(const Matrix &) const;		//arithmetic with assignment and tranposes of matrices in the next 7 lines
	Matrix operator-(const Matrix &) const;
	Matrix operator*(const Matrix &) const;
	Matrix& operator=(const Matrix &);
	Matrix operator!() const;
	Matrix operator~() const;

	void transpose() const;	//same as ~
	void printMatrix() const; //same as <<

private:
	Complex* complexPtr;	//data members with complexPtr initialized to beginning of matrix
	int row;
	int column;
	int dimension;
};
#endif