#include <iostream>
#include <iomanip>
#include "matrix.h"
#include "complex.h"

using namespace std;

int main()
{
//first let's declare an array of complex numbers
Complex a[5];
Complex result[5];
Complex test1(4.5,6.5);


//I will fill these with data
a[0].setComplex(3,4);
a[1].setComplex(-1,3);
a[2].setComplex(-1.23,-9.83);
a[3].setComplex(3.14,-98.3);
a[4].setComplex(2.71,1.61);

//Now print the data in polar format 
cout << "\nPrint Array of Complex Numbers in Polar Format" << endl;
for (int i = 0; i < 5; i++)
{
   a[i].displayPolar();
}

//Now test the add function
cout << "\nTesting add operator a[0] + a[1]" << endl;
result[0] = a[0] + a[1];
result[0].displayRect();

//Now test the sub function
cout << "\nTesting subtract operator a[1] - a[2]" << endl;
result[1] = a[1] - a[2];
cout << result[1] << endl;
//Now test the multiply function
cout << "\nTesting multiply operator a[2] * a[3]" << endl;
result[2] = a[2] * a[3];
cout << result[2] << endl;
result[2].displayRect();

//Now test the divide function
cout << "\nTesting divide operator a[3] / a[4]" << endl;
result[3] = a[3] / a[4];
result[3].displayRect();

//Now test the divide by zero 
cout << "\nTesting divide by zero a[4] / (0)" << endl;
result[4] = a[4] / Complex(0,0);
cout << result[4] << endl;

//Now display the results array in polar format 
cout << "\nNow display the results array in polar format" << endl;
for (int i = 0; i < 5 ; i++)
{
        result[i].displayPolar();
}

//Now start to test the matrix 

Matrix A(3,3);
Matrix C(4,4);
Matrix D(2,3);
Matrix E(2,3);
Complex num(1,1);
int counter =0;

for (int i = 1; i<=3; i++)
   for (int j = 1; j<=3; j++)
        { A(i,j) = Complex(counter++,0); }

for (int i = 1; i<=2; i++)
   for (int j = 1; j<=3; j++)
   { 
          D(i,j) = Complex(counter,counter);
          counter++;
    }

Matrix B(A);

cout << "A Matrix" << endl;
A.printMatrix();
cout << endl;

cout << "B transpose" << endl;
B.transpose();
cout << B << endl;
cout << endl;

cout << "The C matrix " << endl;

C(1,1) = num;
C(2,2) = Complex(4,2);
C(3,3) = Complex(1,1);
C(4,4) = Complex(0,1);

cout << C << endl;

cout << "D Matrix" << endl;
D.printMatrix();
cout << endl;

A = B*B;
cout << "The A = B*B matrix is " << endl;
cout << A << endl;
cout << endl;

cout << "The transpose of A is then" << endl;
(~A).printMatrix();
cout << endl;

cout << "The matrix A is still the following" << endl;
cout << A << endl;

B = B+B;
cout << "The B = B+B matrix is " << endl;
cout << B << endl;

B = Complex(6,7)*B;
cout << "The B = (6+7j)*B gives B as " << endl;
cout << B << endl;

B = B*Complex(7,7);
cout << "The B = B*(7+7j) gives B as " << endl;
cout << B << endl;

B = !B;
cout << "The conjugate transpose of B is " << endl;
cout << B << endl;

A = C;

cout << "The A = A-A matrix is " << endl;
A = A - A;
cout << A << endl;

cout << "Multiply with rectanglur matrix D: E=D*B" << endl;
E = D*B;
cout << E << endl;

cout << "Try multiplying mismatched matrices" << endl;
C = A*B;
cout << C << endl;

cout << "Try adding mismatched matrices" << endl;
A = A+B;
cout << A << endl;

cout << "Try subtracting mismatched matrices" << endl;
A = A-B;
cout << A << endl;
return 0;

}

