#ifndef MATRIX___H
#define MATRIX___H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef int Bool;

#define true 1
#define false 0

typedef double E;

typedef struct  
{
	E *mat;
	int rows, cols;
} StrMatrix, *Matrix;

Matrix newMatrix(int r, int c);
E getElt(Matrix m, int r, int c);
void setElt(Matrix m, E e, int r, int c);
void deleteMatrix(Matrix m); 
void displayMatrix(Matrix m);
int NoPrecision(Matrix m, int* width);
void displayE0(E e, int width);
void displayE3(E e, int width);

Bool areSameSize(Matrix m1, Matrix m2);

E mRand(E min, E max);
Matrix identityMatrix(int s);
Matrix randomMatrix(int r, int c, int min, int max);
Matrix clone(Matrix m);

Matrix addMatricis(Matrix m1, Matrix m2);
Matrix substractMatricis(Matrix m1, Matrix m2);
Matrix mulMatricis(Matrix m1, Matrix m2);
Matrix mult_scal(Matrix m, E s);

Matrix transposeMatrix(Matrix m);

void echangeLigne(Matrix m1, Matrix m2, int i, int j);
void addMultLigne(Matrix m1, Matrix m2, int i, E c, int j);
int triangulaire(Matrix m1, Matrix m2);
int choixPivotPartiel(Matrix m, int i);
E determinant(Matrix m);
void remontee(Matrix a, Matrix b, Matrix x);
Matrix gauss(Matrix a, Matrix b);

Matrix expo(Matrix m, int n);
E somme(Matrix l, Matrix u, int k, int n, int i, int j);
void decomposition(Matrix a, Matrix l, Matrix u);
Matrix invert(Matrix m);
int rank(Matrix m);

#endif
