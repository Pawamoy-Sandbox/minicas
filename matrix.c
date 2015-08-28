#include "matrix.h"

Matrix newMatrix(int r, int c)
{
	Matrix m = malloc(sizeof(StrMatrix));
	int i;
	
	if (m != NULL)
	{
		m->rows = r;
		m->cols = c;
		
		m->mat = malloc(sizeof(E)*r*c);
		
		if (m->mat != NULL)
		{
			for (i = 0; i < r*c; i++)
			{
				m->mat[i] = 0.0f;
			}
		}
	}
	
	return m;	
}

E getElt(Matrix m, int r, int c)
{
	return m->mat[m->cols*r+c];
}

void setElt(Matrix m, E e, int r, int c)
{
	m->mat[m->cols*r+c] = e;
}

void deleteMatrix(Matrix m)
{
	free(m->mat);
	m->mat = NULL;
	
	free(m);
}

void displayMatrix(Matrix m)
{
	int i, j, width=0;
	int np = NoPrecision(m, &width);
	
	for (i = 0; i < m->rows; i++)
	{
		printf("\t\t[");
		for (j = 0; j < m->cols; j++)
		{
			if (np) displayE0(getElt(m, i, j), width);
			else displayE3(getElt(m, i, j), width);
		}
			
		printf("]\n");
	}
	
	printf("\n");
}

int NoPrecision(Matrix m, int* width)
{
	int i, j, ret = 1;
	E tmp, ftmp, ptmp;
	
	for (i = 0; i < m->rows; i++)
	{
		for (j = 0; j < m->cols; j++)
		{
			tmp = getElt(m, i, j);
			if (ret==1)
			{
				ptmp = fabs(tmp-floor(tmp));
				if (ptmp>=0.001) ret = 0;
			}
			
			ftmp = fabs(tmp);
			if (ftmp>=100)
			{
				if ((*width)<4)
				{
					if (tmp<0) *width=4;
					else *width=3;
				}
			}
			else if (ftmp>=10)
			{
				if ((*width)<3)
				{
					if (tmp<0) *width=3;
					else *width=2;
				}
			}
			else
			{
				if ((*width)<2)
				{
					if (tmp<0) *width=2;
					else *width=1;
				}
			}
		}
	}
	return ret;
}

void displayE0(E e, int width)
{
	printf(" %*.0f ", width, e);
}

void displayE3(E e, int width)
{
	printf(" %*.3f ", width+5, e);
}

Matrix identityMatrix(int s)
{
	Matrix id = newMatrix(s,s);
	int i;
	
	for (i = 0; i < id->cols; i++)
	{
		setElt(id, 1.0f, i, i);
	}
	
	return id;
}

// http://stackoverflow.com/questions/686353/c-random-float
E mRand(E min, E max) 
{
   return (E)((max-min)*((E)rand()/RAND_MAX))+min;
}

Matrix randomMatrix(int r, int c, int min, int max)
{
	Matrix m = newMatrix(r,c);
	int i, j;
	
	for (i = 0; i < m->rows; i++)
	{
		for (j = 0; j < m->cols; j++)
		{
			setElt(m, mRand(min,max), i, j);
		}
	}
	
	return m;
}

Matrix clone(Matrix m)
{
	Matrix c = newMatrix(m->rows, m->cols);
	int i, j;
	for (i=0; i<m->rows; i++)
	{
		for (j=0; j<m->cols; j++)
		{
			setElt(c, getElt(m, i, j), i, j);
		}
	}
	return c;
}

// précondition : m1 et m2 ont la meme taille
Matrix addMatricis(Matrix m1, Matrix m2)
{
	Matrix m = newMatrix(m1->rows,m1->cols);
	
	int i, j;
	
	for (i = 0; i < m->rows; i++)
	{
		for (j = 0; j < m->cols; j++)
		{
			setElt(m, getElt(m1,i,j)+getElt(m2,i,j), i, j);
		}
	}
	
	return m;
}

// précondition : m1 et m2 ont la meme taille
Matrix substractMatricis(Matrix m1, Matrix m2)
{
	Matrix m = newMatrix(m1->rows,m1->cols);
	
	int i, j;
	
	for (i = 0; i < m->rows; i++)
	{
		for (j = 0; j < m->cols; j++)
		{
			setElt(m, getElt(m1,i,j)-getElt(m2,i,j), i, j);
		}
	}
	
	return m;
}

// précondition : m1->cols == m2->rows
Matrix mulMatricis(Matrix m1, Matrix m2)
{
	Matrix m = newMatrix(m1->rows,m2->cols);
	
	int i, j, k;
	E a, b, c, r;
	
	for (i = 0; i < m->rows; i++)
	{
		for (j = 0; j < m->cols; j++)
		{
			c = 0;
			
			for (k = 0; k < m->cols; k++)
			{
				a = getElt(m1,i,k);
				b = getElt(m2,k,j);
				r = a*b;
				c += r;
			}
			
			setElt(m, c, i, j);
		}
	}
	
	return m;
}

Matrix mult_scal(Matrix m, E s)
{
	Matrix r = newMatrix(m->rows,m->cols);
	
	int i,j;
	for(i=0; i<m->rows; i++){
		for(j=0; j<m->cols; j++){
			setElt(r, getElt(m,i,j) * s, i, j);
		}
	}
	return r;
}

Matrix transposeMatrix(Matrix m)
{
	Matrix t = newMatrix(m->cols,m->rows);

	int i, j;
	
	for (i = 0; i < t->rows; i++)
	{
		for (j = 0; j < t->cols; j++)
		{
			setElt(t, getElt(m,j,i), i,j);
		}
	}
	
	return t;
}

void echangeLigne(Matrix m1, Matrix m2, int i, int j)
{
	int k; E tmp;
	
	for(k=0; k<m1->rows; k++)
	{
		tmp = getElt(m1,i,k);
		setElt(m1,getElt(m1,j,k),i,k);
		setElt(m1,tmp,j,k);
	}
	
	if (m2!=NULL){
		tmp = getElt(m2, i, 0);
		setElt(m2, getElt(m2, j, 0), i, 0);
		setElt(m2, tmp, j, 0);
	}
}

void addMultLigne(Matrix m1, Matrix m2, int i, E c, int j)
{
	int k; E tmp;
	
	for(k=0; k<m1->rows; k++)
	{
		tmp = getElt(m1,i,k)+(c*getElt(m1,j,k));
		setElt(m1,tmp,i,k);
	}
	
	if (m2!=NULL) setElt(m2, getElt(m2, i, 0)+(c*getElt(m2, j, 0)), i, 0);
}

int triangulaire(Matrix m1, Matrix m2)
{
	int signe = 1, i, j;
	for(i=0 ; i < m1->rows-1 ; i++)
	{
		j = choixPivotPartiel(m1, i);
		
		if (j != i)
		{
			echangeLigne(m1, m2, i, j);
			signe *= -1;
		}
		
		for (j = i+1 ; j < m1->rows ; j++)
		{
			addMultLigne(m1, m2, j, -((getElt(m1,j,i))/(getElt(m1,i,i))), i);
		}
	}
	return signe;
}

int choixPivotPartiel(Matrix m, int i)
{
	int p = i;
	int j;
	E v = fabs(getElt(m,i,i));
	
	for (j = i+1 ; j < m->rows ; j++)
	{
		if (fabs(getElt(m,j,i)) > v)
		{
			p = j;
			v = fabs(getElt(m,j,i));
		}
	}
	return p;
}

E determinant(Matrix m)
{
	Matrix cm = clone(m);
	
	int i;
	E det = triangulaire(cm, NULL);

	for (i=0 ; i< cm->rows ; i++)
	{
		det *= getElt(cm,i,i);
	}

	deleteMatrix(cm);
	return det;
}

void remontee(Matrix a, Matrix b, Matrix x)
{
	int i,j;
	for(i = a->rows-1 ; i>= 0 ; i--)
	{
		setElt(x, getElt(b,i,0),i,0);
		for(j = i+1 ; j < a->rows ; j++)
		{
			setElt(x, getElt(x,i,0) - (getElt(a,i,j) * getElt(x,j,0)),i,0);
		}
		setElt(x, getElt(x,i,0) / getElt(a,i,i),i,0);
	}
	return ;
}

// précondition : a est carrée, b->rows==a->rows et b->cols==1
Matrix gauss(Matrix a, Matrix b)
{
	Matrix x = newMatrix(b->rows, b->cols);
	Matrix ca = clone(a);
	Matrix cb = clone(b);
	triangulaire(ca, cb);
	remontee(ca, cb, x);
	deleteMatrix(ca);
	deleteMatrix(cb);
	return x;
}

// précondition : m est carrée
Matrix expo(Matrix m, int n)
{
	Matrix tmp, a = clone(m), res = identityMatrix(m->rows);

	while (n > 0) {
		if (n % 2 == 0)
		{
			tmp = mulMatricis(a, a);
			deleteMatrix(a);
			a = tmp;
			n /= 2;
		}
		else {
			tmp = mulMatricis(res, a);
			deleteMatrix(res);
			res = tmp;
			n--;
		}
	}
	deleteMatrix(a);
	return res;
}

E somme(Matrix l, Matrix u, int k, int n, int i, int j)
{
	E res = 0;
	for (k=k; k<n; k++)
	{
		res += getElt(l, i, k)*getElt(u, k, j);
	}
	return res;
}

void decomposition(Matrix a, Matrix l, Matrix u)
{
	int i, j;
	setElt(u, getElt(a, 0, 0), 0, 0);
	for (j=1; j<a->rows; j++)
	{
		setElt(u, getElt(a, 0, j), 0, j);
		setElt(l, getElt(a, j, 0)/getElt(a, 0, 0), j, 0);
	}
	
	for (i=1; i<a->rows-1; i++)
	{
		setElt(u, getElt(a, i, i)-somme(l, u, 0, i, i, i), i, i);
		for (j=i+1; j<a->rows; j++)
		{
			setElt(u, getElt(a, i, j)-somme(l, u, 0, i, i, j), i, j);
			setElt(l, (getElt(a, j, i)-somme(l, u, 0, i, j, i))/getElt(u, i, i), j, i);
		}
	}
	
	i = a->rows-1;
	setElt(u, getElt(a, i, i)-somme(l, u, 0, i, i, i), i, i);
}

// précondition : m est carrée
Matrix invert(Matrix m)
{
	// A^-1 = (LU)^-1 = U^-1 L^-1 = V M
	Matrix L = identityMatrix(m->rows);
	Matrix U = newMatrix(m->rows, m->rows);
	
	decomposition(m, L, U);
	
	Matrix M = newMatrix(L->rows, L->rows);
	Matrix V = newMatrix(U->rows, U->rows);
	
	Matrix tmp;
	
	int i, j, k, n;
	
	// calculating M = L^-1
	n = M->rows;
	for (i=0; i<n; i++)
	{
		for (j=0; j<=i; j++)
		{
			if (j==i)
			{
				setElt(M, 1/getElt(L, i, j), i, j);
			}
			else
			{
				for (k=j; k<i; k++)
				{
					setElt(M, getElt(M, i, j) + (getElt(L, i, k)*getElt(M, k, j)), i, j);
				}
				
				setElt(M, -(getElt(M, i, j)/getElt(L, i, i)), i, j);
			}
		}
	}
	
	// calculating V = U^-1
	n = V->rows;
	for (i=n-1; i>=0; i--)
	{
		for (j=n-1; j>=i; j--)
		{
			if (j==i)
			{
				setElt(V, 1/getElt(U, i, j), i, j);
			}
			else
			{
				for (k=j; k>i; k--)
				{
					setElt(V, getElt(V, i, j) + (getElt(U, i, k)*getElt(V, k, j)), i, j);
				}
				
				setElt(V, -(getElt(V, i, j)/getElt(U, i, i)), i, j);
			}
		}
	}
	
	tmp = mulMatricis(V, M);
	
	deleteMatrix(L);
	deleteMatrix(U);
	deleteMatrix(M);
	deleteMatrix(V);
	
	return tmp;
}

int rank(Matrix m) // todo 
{
	Matrix cm = clone(m);
	int i, r = (m->rows>m->cols)?m->rows:m->cols;
	E tmp;
	
	triangulaire(cm, NULL);
	
	for (i=0; i<cm->rows && i<m->cols; i++)
	{
		tmp = getElt(cm, i, i);
		if (tmp<0.001 && tmp>-0.001) r--;
	}
	
	deleteMatrix(cm);
	
	return r;
}


