#ifndef MINICAS___H
#define MINICAS___H

#include "matrix.h"
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#define MAXSIZE_NAME 256
#define MAXSIZE_FCT 256
#define MAXSIZE_ARGS 512

#define MAXSIZE_CMD MAXSIZE_NAME+MAXSIZE_FCT+MAXSIZE_ARGS

#define MAX_VARS 256
#define MAX_MATS 256

#define NOVAR -9999

#define MIN_E 0
#define MAX_E 20

#define MIN_EXP 2
#define MAX_EXP 10

#define MIN_SCA -10
#define MAX_SCA 10

#define NMX 0
#define ADD 1
#define SUB 2
#define MUL 3
#define MSC 4
#define EXP 5
#define INV 6
#define RNK 7
#define DLU 8
#define TRA 9
#define DET 10 
#define SOL 11
#define SPT 12
#define NOF 13
#define VAR 14
#define DSP 15

#define FNI 5

typedef struct
{
	Matrix mat;
	char name[MAXSIZE_NAME];
} StrMatObject, *MatObject;

typedef struct
{
	E value;
	char name[MAXSIZE_NAME];
} StrVariableObject, *VariableObject;

typedef struct
{
	char name[MAXSIZE_NAME];
	char function[MAXSIZE_FCT];
	char args[MAXSIZE_ARGS];
} StrFunctionCall, *FunctionCall;

extern MatObject mats[MAX_MATS];
extern VariableObject vars[MAX_VARS];
extern int cur_mat;
extern int cur_var;

int IndexMatrix(char* name);
int IndexVariable(char* name);
Matrix GetMatrix(char* name);
void NewMatrix(FunctionCall fc);
void Addition(FunctionCall fc);
void Substraction(FunctionCall fc);
void Multiplication(FunctionCall fc);
void Scalar_Mult(FunctionCall fc);
void Exponentiation(FunctionCall fc);
void Transpose(FunctionCall fc);
void Determinant(FunctionCall fc);
void Decomposition(FunctionCall fc);
void Solve(FunctionCall fc);
void Inversion(FunctionCall fc);
void Rank(FunctionCall fc);
void Display(FunctionCall fc);
void TokenizeSpeedTest(FunctionCall fc, char* fnc, int* min, int* max, int* step, int *sec);
void handler(int signum);
void SpeedTest(FunctionCall fc);
void CreateGNUPLOTFile(int minsize, int maxsize, int step, double* time, double maxtime, char *foutput);
int GetFunction(char* name);
void CallFunction(FunctionCall fc);
E GetVariable(char *name);
void NewVariable(FunctionCall fc);
void TokenizeString(char *buf, FunctionCall fc);
void LaunchInterpreter();

void purger(void);
void clean (char *chaine);

#endif
