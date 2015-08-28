#include "minicas.h"

static int CTRLC = 0;
static int fni = 0;

void purger(void)
{
    int c;
 
    while ((c = getchar()) != '\n' && c != EOF)
    {}
}
 
void clean (char *chaine)
{
    char *p = strchr(chaine, '\n');
 
    if (p)
    {
        *p = 0;
    }
 
    else
    {
        purger();
    }
}

int IndexMatrix(char* name)
{
	int i;

	for (i = 0; i < MAX_MATS; i++)
	{
		if (mats[i] != NULL)
		{
			if (strcmp(name, mats[i]->name) == 0)
				return i;
		}
	}

	return -1;
}

int IndexVariable(char* name)
{
	int i;

	for (i = 0; i < MAX_VARS; i++)
	{
		if (vars[i] != NULL)
		{
			if (strcmp(name, vars[i]->name) == 0)
				return i;
		}
	}

	return -1;
}

Matrix GetMatrix(char* name)
{
	int i;

	for (i = 0; i < MAX_MATS; i++)
	{
		if (mats[i] != NULL)
		{
			if (strcmp(name, mats[i]->name) == 0)
				return mats[i]->mat;
		}
	}

	return (Matrix)NULL;
}

void NewMatrix(FunctionCall fc)
{
	int i, index, exist = 0, ref;
	int n = 1, m = 0;
	int k = -1, l = -1;
	int sign = 1;
	
	index = IndexMatrix(fc->name);
	if (index==-1)
	{
		index = cur_mat;
		mats[index] = (StrMatObject*)malloc(sizeof(StrMatObject));
		
		if (mats[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	if ((ref=IndexMatrix(fc->function))!=-1)
	{
		if (ref==index)
		{
			printf("\tSame Matrix\n");
			return;
		}
		else
		{
			if (exist) deleteMatrix(mats[index]->mat);
			mats[index]->mat = clone(mats[ref]->mat);
			displayMatrix(mats[index]->mat);
			cur_mat++;
			return ;
		}
	}
	else
	{
		if (fc->args[0] == '\0')
		{
			printf("\tMatrix %s Not Found\n", fc->function);
			if (!exist) free(mats[index]);
			return ;
		}
	}
	
	// trying to find out the size of the matrix
	for (i = 0; i < MAXSIZE_ARGS; i++)
	{
		if (fc->args[i] == '[')
		{
			n = 1;
		}

		else if (fc->args[i] == ',')
			n++;

		else if (fc->args[i] == ']')
		{
			m++;
		}
		else if (fc->args[i] == '\0') break;
	}

	if (exist) deleteMatrix(mats[index]->mat);
	mats[index]->mat = newMatrix(m,n);

	for (i = 0; i < MAXSIZE_ARGS; i++)
	{
		if (fc->args[i] == '[')
		{
			k++;
			l = 0;
		}

		else if (fc->args[i] == ',')
			l++;

		else if (fc->args[i] == ']')
			continue;
			
		else if (fc->args[i] == '-')
			sign = -1;

		// it is digit so we add the element to the matrix
		else if (isdigit(fc->args[i]))
		{
			E var = (E)strtod(&(fc->args[i]), NULL);
			setElt(mats[index]->mat, sign*var, k, l);
			while (fc->args[i+1]!=',' && fc->args[i+1]!=']') i++;
			sign = 1;
		}

		// we get the variable referenced
		else if (isalpha(fc->args[i]))
		{
			char name[256] = {0};
			int t = 0;
			E var = 0;

			while (fc->args[i] != ',' && fc->args[i] != ']')
			{
				name[t] = fc->args[i];

				t++;
				i++;
			}

			var = GetVariable(name);
			setElt(mats[index]->mat, sign*var, k, l);
			sign = 1;

			l++; // we increment the rows-counting variable
		}
		
		else if (fc->args[i] == '\0') break;
	}

	// test: display the added matrix
	displayMatrix(mats[index]->mat);

	if (!exist) cur_mat++;
}

void Addition(FunctionCall fc)
{
	int i, j, index, exist = 0;
	Matrix a, b, c;
	char name[MAXSIZE_NAME] = {0};

	index = IndexMatrix(fc->name);
	if (index==-1)
	{
		index = cur_mat;
		mats[index] = (StrMatObject*)malloc(sizeof(StrMatObject));
		
		if (mats[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	j = 0;
	// searching for first matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == ',') {
			name[j] = '\0';
			break;
		}
		j++;
	}
	
	a = GetMatrix(name);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	j = 0;
	// searching for second matrix name
	for (i = i+1; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == '\0') break;
		j++;
	}
	
	b = GetMatrix(name);
	if (b==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	// testing numbers of rows and columns of both matrix
	if (a->rows==b->rows && a->cols==b->cols)
	{
		c = addMatricis(a,b);
	}
	else
	{
		printf("\tMatrix must have same size\n");
		if (!exist) free(mats[index]);
		return;
	}
	
	if (exist) deleteMatrix(mats[index]->mat);
	mats[index]->mat = c;
	
	// test: display the result
	displayMatrix(mats[index]->mat);
	
	if (!exist) cur_mat++;
}

void Substraction(FunctionCall fc)
{
	int i, j, index, exist = 0;
	Matrix a, b, c;
	char name[MAXSIZE_NAME] = {0};

	index = IndexMatrix(fc->name);
	if (index==-1)
	{
		index = cur_mat;
		mats[index] = (StrMatObject*)malloc(sizeof(StrMatObject));
		
		if (mats[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	j = 0;
	// searching for first matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == ',') {
			name[j] = '\0';
			break;
		}
		j++;
	}
	
	a = GetMatrix(name);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	j = 0;
	// searching for second matrix name
	for (i = i+1; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == '\0') break;
		j++;
	}
	
	b = GetMatrix(name);
	if (b==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	// testing numbers of rows and columns of both matrix
	if (a->rows==b->rows && a->cols==b->cols)
	{
		c = substractMatricis(a,b);
	}
	else
	{
		printf("\tMatrix must have same size\n");
		if (!exist) free(mats[index]);
		return;
	}
	
	if (exist) deleteMatrix(mats[index]->mat);
	mats[index]->mat = c;
	
	// test: display the result
	displayMatrix(mats[index]->mat);
	
	if (!exist) cur_mat++;
}

void Multiplication(FunctionCall fc)
{
	int i, j, index, exist = 0;
	Matrix a, b, c;
	char name[MAXSIZE_NAME] = {0};

	index = IndexMatrix(fc->name);
	if (index==-1)
	{
		index = cur_mat;
		mats[index] = (StrMatObject*)malloc(sizeof(StrMatObject));
		
		if (mats[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	j = 0;
	// searching for first matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == ',') {
			name[j] = '\0';
			break;
		}
		j++;
	}
	
	a = GetMatrix(name);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	j = 0;
	// searching for second matrix name
	for (i = i+1; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == '\0') break;
		j++;
	}
	
	b = GetMatrix(name);
	if (b==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	// testing numbers of rows and columns of both matrix
	if (a->cols==b->rows)
	{
		c = mulMatricis(a,b);
	}
	else
	{
		printf("\tCols of Matrix 1 must be equal to Rows of Matrix 2\n");
		if (!exist) free(mats[index]);
		return;
	}
	
	if (exist) deleteMatrix(mats[index]->mat);
	mats[index]->mat = c;
	
	// test: display the result
	displayMatrix(mats[index]->mat);
	
	if (!exist) cur_mat++;
}

void Scalar_Mult(FunctionCall fc)
{
	int i, j, sign = 1, index, exist = 0;
	E s;
	Matrix a, c;
	char name[MAXSIZE_NAME] = {0};

	index = IndexMatrix(fc->name);
	if (index==-1)
	{
		index = cur_mat;
		mats[index] = (StrMatObject*)malloc(sizeof(StrMatObject));
		
		if (mats[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	j = 0;
	// searching for matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == ',') {
			name[j] = '\0';
			break;
		}
		j++;
	}
	
	a = GetMatrix(name);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	if (fc->args[i+1] == '-')
	{
		sign = -1;
		i++;
	}
	
	if (isalpha(fc->args[i+1]))
	{
		j = 0;
		// searching for scalar name
		for (i = i+1; i < MAXSIZE_NAME; i++)
		{
			name[j] = fc->args[i];
			if (name[j] == '\0') break;
			j++;
		}
		
		s = GetVariable(name) * sign;
		if (s==NOVAR)
		{
			printf("\tVariable %s Not Found\n", name);
			if (!exist) free(mats[index]);
			return;
		}
	}
	else
	{
		s = (E)strtod(&(fc->args[i+1]), NULL) * sign;
	}
	
	c = mult_scal(a,s);
	
	if (exist) deleteMatrix(mats[index]->mat);
	mats[index]->mat = c;
	
	// test: display the result
	displayMatrix(mats[index]->mat);
	
	if (!exist) cur_mat++;
}

void Exponentiation(FunctionCall fc)
{
	int i, j, s, sign = 1, index, exist = 0;
	Matrix a, c;
	char name[MAXSIZE_NAME] = {0};

	index = IndexMatrix(fc->name);
	if (index==-1)
	{
		index = cur_mat;
		mats[index] = (StrMatObject*)malloc(sizeof(StrMatObject));
		
		if (mats[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	j = 0;
	// searching for matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == ',') {
			name[j] = '\0';
			break;
		}
		j++;
	}
	
	a = GetMatrix(name);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	if (fc->args[i+1] == '-')
	{
		sign = -1;
		i++;
	}
	
	if (isalpha(fc->args[i+1]))
	{
		j = 0;
		// searching for scalar name
		for (i = i+1; i < MAXSIZE_NAME; i++)
		{
			name[j] = fc->args[i];
			if (name[j] == '\0') break;
			j++;
		}
		
		s = (int)GetVariable(name) * sign;
		if (s==NOVAR)
		{
			printf("\tVariable %s Not Found\n", name);
			if (!exist) free(mats[index]);
			return;
		}
	}
	else
	{
		s = atoi(&(fc->args[i+1])) * sign;
	}
	
	if (a->rows == a->cols)
	{
		c = expo(a, s);
	}
	else
	{
		printf("\tMatrix must be square matrix\n");
		if (!exist) free(mats[index]);
		return ;
	}
	
	if (exist) deleteMatrix(mats[index]->mat);
	mats[index]->mat = c;
	
	// test: display the result
	displayMatrix(mats[index]->mat);
	
	if (!exist) cur_mat++;
}

void Transpose(FunctionCall fc)
{
	int i, j, index, exist = 0;
	Matrix a, c;
	char name[MAXSIZE_NAME] = {0};

	index = IndexMatrix(fc->name);
	if (index==-1)
	{
		index = cur_mat;
		mats[index] = (StrMatObject*)malloc(sizeof(StrMatObject));
		
		if (mats[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	j = 0;
	// searching for matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == '\0') break;
		j++;
	}
	
	a = GetMatrix(name);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	c = transposeMatrix(a);
	
	if (exist) deleteMatrix(mats[index]->mat);
	mats[index]->mat = c;
	
	// test: display the result
	displayMatrix(mats[index]->mat);
	
	if (!exist) cur_mat++;
}

void Determinant(FunctionCall fc)
{
	int i, j, index, exist = 0;
	E d;
	Matrix a;
	char name[MAXSIZE_NAME] = {0};

	index = IndexVariable(fc->name);
	if (index==-1)
	{
		index = cur_var;
		vars[index] = (StrVariableObject*)malloc(sizeof(StrVariableObject));
		
		if (vars[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			vars[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	j = 0;
	// searching for matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == '\0') break;
		j++;
	}
	
	a = GetMatrix(name);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(vars[index]);
		return;
	}
	
	d = determinant(a);
	
	vars[index]->value = d;
	
	// test: display the determinant
	printf("\t\t");
	displayE0(vars[index]->value, 3);
	printf("\n");
	
	if (!exist) cur_var++;
}

void Decomposition(FunctionCall fc)
{
	int i, j, indexL, indexU, existL = 0, existU = 0;
	Matrix a, l, u;
	char nameA[MAXSIZE_NAME] = {0};
	char nameL[MAXSIZE_NAME] = {0};
	char nameU[MAXSIZE_NAME] = {0};

	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		nameL[i] = fc->name[i];
		nameU[i] = fc->name[i];
		if (fc->name[i] == '\0')
		{
			if (i<MAXSIZE_NAME-2)
			{
				nameL[i] = '_';
				nameU[i] = '_';
				nameL[i+1] = 'L';
				nameU[i+1] = 'U';
				nameL[i+2] = '\0';
				nameU[i+2] = '\0';
			}
			break;
		}
	}
	
	indexL = IndexMatrix(nameL);
	if (indexL==-1)
	{
		indexL = cur_mat;
		mats[indexL] = (StrMatObject*)malloc(sizeof(StrMatObject));
		if (mats[indexL] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}
		
		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[indexL]->name[i] = nameL[i];
			if (nameL[i]=='\0') break;
		}
	}
	else existL = 1;
	
	indexU = IndexMatrix(nameU);
	if (indexU==-1)
	{
		indexU = cur_mat+1;
		mats[indexU] = (StrMatObject*)malloc(sizeof(StrMatObject));
		if (mats[indexU] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}
		
		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[indexU]->name[i] = nameU[i];
			if (nameU[i]=='\0') break;
		}
	}
	else existU = 1;	
	
	j = 0;
	// searching for matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		nameA[j] = fc->args[i];
		if (nameA[j] == ',') {
			nameA[j] = '\0';
			break;
		}
		j++;
	}
	
	a = GetMatrix(nameA);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", nameA);
		if (!existL) free(mats[indexL]);
		if (!existU) free(mats[indexU]);
		return;
	}
	
	l = identityMatrix(a->rows);
	u = newMatrix(a->rows, a->cols);
	
	if (l!=NULL && u!=NULL)
	{
		decomposition(a, l, u);
	}
	
	if (existL) deleteMatrix(mats[indexL]->mat);
	mats[indexL]->mat = l;
	if (existU) deleteMatrix(mats[indexU]->mat);
	mats[indexU]->mat = u;
	
	// test: display the result
	displayMatrix(mats[indexL]->mat);
	displayMatrix(mats[indexU]->mat);

	if (!existL) cur_mat++;
	if (!existU) cur_mat++;
}

void Solve(FunctionCall fc)
{
	int i, j, index, exist = 0;
	Matrix a, b, x;
	char name[MAXSIZE_NAME] = {0};

	index = IndexMatrix(fc->name);
	if (index==-1)
	{
		index = cur_mat;
		mats[index] = (StrMatObject*)malloc(sizeof(StrMatObject));
		
		if (mats[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	j = 0;
	// searching for first matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == ',') {
			name[j] = '\0';
			break;
		}
		j++;
	}
	
	a = GetMatrix(name);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	j = 0;
	// searching for second matrix name
	for (i = i+1; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == '\0') break;
		j++;
	}
	
	b = GetMatrix(name);
	if (b==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	// testing numbers of rows and columns of both matrix
	if (a->rows==a->cols && a->rows==b->rows && b->cols==1)
	{
		x = gauss(a,b);
	}
	else
	{
		printf("\tMatrix 1 must be square matrix\n");
		printf("\tRows of Matrix 2 must be equal to Rows of Matrix 1\n");
		printf("\tMatrix 2 must only have one Column\n");
		if (!exist) free(mats[index]);
		return;
	}
	
	if (exist) deleteMatrix(mats[index]->mat);
	mats[index]->mat = x;
	
	// test: display the result
	displayMatrix(mats[index]->mat);
	
	if (!exist) cur_mat++;
}

void Inversion(FunctionCall fc)
{
	int i, j, index, exist = 0;
	Matrix a, c;
	char name[MAXSIZE_NAME] = {0};

	index = IndexMatrix(fc->name);
	if (index==-1)
	{
		index = cur_mat;
		mats[index] = (StrMatObject*)malloc(sizeof(StrMatObject));
		
		if (mats[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			mats[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	j = 0;
	// searching for matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == '\0') break;
		j++;
	}
	
	a = GetMatrix(name);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	c = invert(a);
	
	if (c==NULL)
	{
		printf("\tMatrix %s Not Invertible\n", name);
		if (!exist) free(mats[index]);
		return;
	}
	
	if (exist) deleteMatrix(mats[index]->mat);
	mats[index]->mat = c;
	
	// test: display the result
	displayMatrix(mats[index]->mat);
	
	if (!exist) cur_mat++;
}

void Rank(FunctionCall fc)
{
	int i, j, index, exist = 0;
	E r;
	Matrix a;
	char name[MAXSIZE_NAME] = {0};

	index = IndexVariable(fc->name);
	if (index==-1)
	{
		index = cur_var;
		vars[index] = (StrVariableObject*)malloc(sizeof(StrVariableObject));
		
		if (vars[index] == NULL)
		{
			printf("NewMatrix(): Could not allocate the new matrix\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			vars[index]->name[i] = fc->name[i];
			if (fc->name[i]=='\0') break;
		}
	}
	else exist = 1;
	
	j = 0;
	// searching for matrix name
	for (i = 0; i < MAXSIZE_NAME; i++)
	{
		name[j] = fc->args[i];
		if (name[j] == '\0') break;
		j++;
	}
	
	a = GetMatrix(name);
	if (a==NULL)
	{
		printf("\tMatrix %s Not Found\n", name);
		if (!exist) free(vars[index]);
		return;
	}
	
	r = rank(a);
	
	vars[index]->value = r;
	
	// test: display the rank
	printf("\t\t");
	displayE0(vars[index]->value, 3);
	printf("\n");
	
	if (!exist) cur_var++;
}

void Display(FunctionCall fc)
{
	int index;
	
	if (fc->name[0] == '*')
	{
		if (fc->name[1] == '\0' || fc->name[1] == 'V' || fc->name[1] == 'v')
		{
			printf("\n");
			for (index=0; index<cur_var; index++)
			{
				printf("%24s  == ", vars[index]->name);
				displayE3(vars[index]->value, 0);
				printf("\n");
			}
		}
		if (fc->name[1] == '\0' || fc->name[1] == 'M' || fc->name[1] == 'm')
		{
			printf("\n");
			for (index=0; index<cur_mat; index++)
			{
				printf("\tMatrix %s :\n", mats[index]->name);
				displayMatrix(mats[index]->mat);
			}
		}
	}
	else
	{
		index = IndexMatrix(fc->name);
		
		if (index==-1)
		{
			index = IndexVariable(fc->name);
			
			if (index==-1)	printf("\tSymbol %s Not Found\n", fc->name);
			else
			{
				printf("\tVariable %s :", vars[index]->name);
				displayE3(vars[index]->value, 6);
				printf("\n");
			}
		}
		else
		{
			printf("\tMatrix %s :\n", mats[index]->name);
			displayMatrix(mats[index]->mat);
			
			index = IndexVariable(fc->name);
			
			if (index!=-1)
			{
				printf("\tVariable %s :", vars[index]->name);
				displayE3(vars[index]->value, 6);
				printf("\n");
			}
		}
	}
}	

void TokenizeSpeedTest(FunctionCall fc, char* fnc, int* min, int* max, int* step, int *sec)
{
	int i=0, j=0;
	char c = 'a';
	
	while (c!='\0')
	{
		c = fc->name[i];
		i++;
	}
	
	c = 'a';
	while (c!='\0')
	{
		c = fc->name[i];
		i++;
		fnc[j] = c;
		j++;
	}
		
	*min = atoi(&(fc->name[i]));
	
	c = 'a';
	while (c!='\0')
	{
		c = fc->name[i];
		i++;
	}
	
	*max = atoi(&(fc->name[i]));
	
	c = 'a';
	while (c!='\0')
	{
		c = fc->name[i];
		i++;
	}
	
	*step = atoi(&(fc->name[i]));
	
	j=i;
	c = 'a';
	while (c!='\0')
	{
		c = fc->name[i];
		i++;
	}
	
	if (j!=i) *sec = atoi(&(fc->name[i]));
}

void handler(int signum)
{
	if (signum==SIGINT) CTRLC = 1;
}

void SpeedTest(FunctionCall fc)
{
	int i, min, max, step, sec=0;
	int sizeTimeArray, fct;
	
	char fnc[MAXSIZE_FCT];
	char foutput[256];
	
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	
	struct timeval start, end, result;
	double maxtime = 0;
	double time;
	double usec;
	double* timeArray;
	
	Matrix a=NULL, b=NULL, tmp=NULL;
	E s;
	int n; 
	
	TokenizeSpeedTest(fc, fnc, &min, &max, &step, &sec);
	
	if (min>0 && max>0 && step>0)
	{
		sizeTimeArray = ((max-min)/step)+1;
		timeArray = (double*)malloc(sizeTimeArray*sizeof(double));
		for (i=0; i<sizeTimeArray; i++) timeArray[i] = 0;
	}
	
	usec = (double)sec * 1000000;
	
	sprintf(foutput, "speedtest_%s_%d_%d_%d_%d", fnc, min, max, step, sec);
	
	fct = GetFunction(fnc);
	
	switch (fct)
	{
		case NMX :
		{
			printf("\t You must specify another function\n");
			return;
		}
		
		case ADD :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				b = randomMatrix(i, i, MIN_E, MAX_E);
				
				gettimeofday(&start, NULL);
				tmp = addMatricis(a, b);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				deleteMatrix(b);
				deleteMatrix(tmp);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case SUB :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				b = randomMatrix(i, i, MIN_E, MAX_E);
				
				gettimeofday(&start, NULL);
				tmp = substractMatricis(a, b);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				deleteMatrix(b);
				deleteMatrix(tmp);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case MUL :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				b = randomMatrix(i, i, MIN_E, MAX_E);
				
				gettimeofday(&start, NULL);
				tmp = mulMatricis(a, b);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				deleteMatrix(b);
				deleteMatrix(tmp);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case MSC :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				s = mRand(MIN_SCA, MAX_SCA);
				
				gettimeofday(&start, NULL);
				tmp = mult_scal(a, s);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				deleteMatrix(tmp);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case EXP :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				n = (int)mRand(MIN_EXP, MAX_EXP);
				
				gettimeofday(&start, NULL);
				tmp = expo(a, n);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				deleteMatrix(tmp);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case TRA :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				
				gettimeofday(&start, NULL);
				tmp = transposeMatrix(a);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				deleteMatrix(tmp);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case DET :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				
				gettimeofday(&start, NULL);
				s = determinant(a);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case DLU :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				b = newMatrix(i, i);
				tmp = identityMatrix(i);
				
				gettimeofday(&start, NULL);
				decomposition(a, b, tmp);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				deleteMatrix(b);
				deleteMatrix(tmp);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case SOL :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				b = randomMatrix(i, 1, MIN_E, MAX_E);
				
				gettimeofday(&start, NULL);
				tmp = gauss(a, b);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				deleteMatrix(b);
				deleteMatrix(tmp);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case INV :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				
				gettimeofday(&start, NULL);
				tmp = invert(a);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				deleteMatrix(tmp);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case RNK :
		{
			sigaction(SIGINT, &action, NULL);
			
			for (i=min; i<=max; i+=step)
			{
				a = randomMatrix(i, i, MIN_E, MAX_E);
				
				gettimeofday(&start, NULL);
				n = rank(a);
				gettimeofday(&end, NULL);
				
				timersub(&end, &start, &result);
				time = (result.tv_sec*1000000)+result.tv_usec;
				if (maxtime < time) maxtime = time;
				
				timeArray[(i-min)/step] = time;
				printf("\t* %s Size:%5d ;\tTime:%8.0f µs ;\n", fnc, i, time);
				
				deleteMatrix(a);
				
				if (usec >= 1000000) {
					if (time >= usec) {
						i++;
						break;
					}
				}
				if (CTRLC) {
					i++;
					break;
				}
			}
			
			CreateGNUPLOTFile(min, i-step, step, timeArray, maxtime, foutput);
			break;
		}
		
		case VAR : // default
		case NOF : // default
		default :
		{
			printf("\t%s : Function Not Implemented\n", fnc);
			fni++;
			break;
		}
	}
	
	if (fct!=NOF && fct !=VAR) fni = 0;
	
	free(timeArray);
	CTRLC = 0;
	sigemptyset(&action.sa_mask);
}

void CreateGNUPLOTFile(int minsize, int maxsize, int step, double* time, double maxtime, char *foutput)
{
	FILE *file = NULL, *gnu = NULL;
	int i, unit, timesize;
	char gnunit[3], out[256];
	
	// preparing time unit
	if (maxtime > 1000000)
	{
		unit = 1000000;
		sprintf(gnunit, "s");
	}
	else if (maxtime > 1000)
	{
		unit = 1000;
		sprintf(gnunit, "ms");
	}
	else
	{
		unit = 1;
		sprintf(gnunit, "µs");
	}
	
	file = fopen(foutput, "w");
	
	// writing size-time file
	if (file != NULL)
	{
		// size of time array
		timesize = ((maxsize-minsize)/step)+1;
		
		for (i=0; i<timesize; i++)
		{
			fprintf(file, "%d %f\n", (i*step)+minsize, time[i]/unit);
		}
		
		fclose(file);
	}
	else
	{
		printf("\tError while opening %s\n", foutput);
		return;
	}
	
	// preparing writing of gnuplot script
	sprintf(out, "%s.gnu", foutput);
	
	int xtics = (maxsize-minsize)/step;
	if (xtics>160) xtics = step*16;
	else if (xtics>80) xtics = step*8;
	else if (xtics>40) xtics = step*4;
	else if (xtics>20) xtics = step*2;
	else xtics = step;
	
	gnu = fopen(out, "w");
	
	if (gnu != NULL)
	{
		fprintf(gnu, "set terminal png size 800, 400\n");
		fprintf(gnu, "set terminal png enhanced\n");
		fprintf(gnu, "set terminal png font arial 10\n");
		fprintf(gnu, "set output '%s.png'\n", foutput);
		fprintf(gnu, "set key on inside top left box\n");
		fprintf(gnu, "set xrange [%d:%d]\n", minsize, maxsize);
		fprintf(gnu, "set yrange [0:%f]\n", maxtime/unit);
		fprintf(gnu, "set xtics %d\n", xtics);
		fprintf(gnu, "set grid\n");
		fprintf(gnu, "plot \"%s\" using 1:2 title 'time (%s)' with linespoint\n", foutput, gnunit);
		fclose(gnu);
	}
	else
	{
		printf("\tError while opening %s\n", out);
		return;
	}
	
	sprintf(out, "gnuplot < %s.gnu", foutput);
	system(out);
	
	sprintf(out, "rm %s %s.gnu", foutput, foutput);
	system(out);
}

int GetFunction(char* name)
{
	if (name[0] == '\0') return NOF;
	else if (isdigit(name[0]) || name[0]=='-') return VAR;
	else if (strcmp(name, "speedtest") == 0) return SPT;
	else if (strcmp(name, "matrix") == 0) return NMX;
	else if (strcmp(name, "addition")==0) return ADD;
	else if (strcmp(name, "sub")==0) return SUB;
	else if (strcmp(name, "mult")==0) return MUL;
	else if (strcmp(name, "mult_scal")==0) return MSC;
	else if (strcmp(name, "expo")==0) return EXP;
	else if (strcmp(name, "transpose")==0) return TRA;
	else if (strcmp(name, "determinant")==0) return DET;
	else if (strcmp(name, "decomposition")==0) return DLU;
	else if (strcmp(name, "solve")==0) return SOL;
	else if (strcmp(name, "invert")==0) return INV;
	else if (strcmp(name, "rank")==0) return RNK;
	else if (strcmp(name, "display")==0) return DSP;
	else return NOF;
}

void CallFunction(FunctionCall fc)
{
	int function = GetFunction(fc->function);
	
	switch (function)
	{
		case VAR : NewVariable(fc); break;
		case NMX : NewMatrix(fc); break;
		case ADD : Addition(fc); break;
		case SUB : Substraction(fc); break;
		case MUL : Multiplication(fc); break;
		case MSC : Scalar_Mult(fc); break;
		case EXP : Exponentiation(fc); break;
		case TRA : Transpose(fc); break;
		case DET : Determinant(fc); break;
		case DLU : Decomposition(fc); break;
		case SOL : Solve(fc); break;
		case INV : Inversion(fc); break;
		case RNK : Rank(fc); break;
		case DSP : Display(fc); break;
		case NOF : // default
		default :
		{
			if (GetFunction(fc->name)==SPT) SpeedTest(fc);
			else if (IndexVariable(fc->function)!=-1) NewVariable(fc);
			else if (IndexMatrix(fc->function)!=-1) NewMatrix(fc);
			else
			{
				printf("\t%s : Function Not Implemented\n", fc->function);
				fni++;
			}
			break;
		}
	}
	
	if (function!=NOF && function !=VAR) fni = 0;
}

E GetVariable(char *name)
{
	int i;

	for (i = 0; i < MAX_VARS; i++)
	{
		if (vars[i] != NULL)
		{
			if (strcmp(name, vars[i]->name) == 0)
				return vars[i]->value;
		}
	}

	return NOVAR;
}

void NewVariable(FunctionCall fc)
{
	int i, index, sign = 1, n = 0, ref, exist = 0;
	E tmpval;
	
	index = IndexVariable(fc->name);
	if (index==-1)
	{
		index = cur_var;
		vars[index] = (StrVariableObject*)malloc(sizeof(StrVariableObject));

		if (vars[index] == NULL)
		{
			printf("NewVariable(): Could not allocate the new variable\n");
			return;
		}

		for (i = 0; i < MAXSIZE_NAME; i++)
		{
			vars[index]->name[i] = fc->name[i];
		}
	}
	else exist = 1;

	if (fc->function[0] == '-')
	{
		sign = -1;
		n++;
	}
	
	if (!isdigit(fc->function[n]))
	{
		ref = IndexVariable(&(fc->function[n]));
		if (ref == -1)
		{
			printf("\tVariable %s Not Found\n", &(fc->function[n]));
			if (!exist) free(vars[index]);
			return ;
		}
		else
		{
			if (ref==index)
			{
				printf("\tSame Variables\n");
				return ;
			}
			else tmpval = vars[ref]->value;
		}
	}
	else tmpval = (E)strtod(&(fc->function[n]), NULL);
	
	vars[index]->value = tmpval * sign;

	printf("\t\t");
	displayE3(vars[index]->value, 6);
	printf("\n");

	cur_var++;
}

void TokenizeString(char *buf, FunctionCall fc)
{
	int i, j, k = 0;

	if (fc == NULL)
	{
		printf("NewMatrix(): Could not allocate the function call struct\n");
		return;
	}



	// we get the name of the object
	for (i = 0; i < strlen(buf); i++)
	{
		if (buf[i] != ':')
		{
			if (buf[i] == ' ')
				continue;
			fc->name[i] = buf[i];
		}
		else
			break;
	}

	j = i;

	// we get the function
	for (i = j; i < strlen(buf); i++)
	{
		if (buf[i] != '(')
		{
			if (buf[i] == ':' || buf[i] == ' ')
				continue;
			fc->function[k] = buf[i];
			k++;
		}
		else
			break;
	}

	// it is a variable, the "function" is the value of the variable
	//~ if (isdigit(fc->function[0]) || fc->function[0] == '-')
	//~ {
		//~ NewVariable(fc);
		//~ return;
	//~ }

	j = i;
	k = 0;

	// we get the args
	for (i = j; i < strlen(buf); i++)
	{
		if (buf[i] != ')')
		{
			if (buf[i] == ':' || buf[i] == ' ' || buf[i] == '(')
				continue;
			fc->args[k] = buf[i];
			k++;
		}
		else
			break;
	}
	
	fc->args[k] = '\0';
}

void LaunchInterpreter()
{
	char buff[1024] = {0};
	FunctionCall fc = (StrFunctionCall*)malloc(sizeof(StrFunctionCall));

	do
	{		
		memset(fc, 0, MAXSIZE_CMD);
		printf("> ");
		
		if (fni>FNI)
		{
			printf("\tToo much call function errors, program aborting\n");
			break;
		}

		// we get the string from the buffer
		fgets(buff, sizeof buff, stdin);
		clean(buff);

		if (strcmp(buff, "quit")==0) break;
		
		if (buff[0] != '\0')
		{		
			if (buff[0] == '#') printf("%s\n", &(buff[1]));
			else
			{
				TokenizeString(buff, fc);
				CallFunction(fc);
			}
		}
		
	} while (true);
	
	printf("\n");
	
	free(fc);
}


