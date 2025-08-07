#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

// define your own types and function prototypes for the symbol table(s) module below

typedef enum {stat, field, subroutine, arg, var, non} Kinds;

typedef struct Symbol {
    char name[128];
	char type[128];
	Kinds kind;
	int index;
} Symbol;

typedef struct MethodTable {
	char name[128]; // method name
	int count; // number of symbols in method
	Symbol symbols[128]; // array of symbols in method
	int varCount; // number of local variables in method
	int argCount; // number of arguments in method
	char type[128]; // return type of method
} MethodTable;

typedef struct ClassTable {
	char name[128]; // class name
	int methodCount; // number of methods in class
	int symbolCount; // number of symbols in class
	int staticCount; // number of static variables in class
	int fieldCount; // number of field variables in class
	Symbol symbols[128]; // array of symbols in class
	MethodTable methods[128]; // array of methods in class
} ClassTable;

typedef struct Subroutine {
	char class[128]; // class name
	char method[128]; // method name
	Token token; // token to check
} Subroutine;


void NewClass(Token token);
int InsertClassSymbol(char* name, char* type, Kinds kind);
void NewMethod(char *name, char *type);
int InsertMethodSymbol(char* name, char* type, Kinds kind);
int CheckClassSymbolRedeclare(char *name);
int CheckMethodSymbolRedeclare(char *name);
void ClassStackPush(Token token);
Token CheckClassStack();
void initSymbolTable();
Token CheckVarDeclar(Token token);
void SubStackInsert(char *class, char *method, Token token);
char *getCurrentClass();
int getCurrentClassVarCount();
Token CheckSubStack();
int ExternalLibCheck(char *class);
Token CheckVarMember(char* name);
Token CheckMethodDeclar(Token class, Token method);
int ExternalLibMethodCheck(char *class, char *method);
void printAll();
void CodeGeneration(char* dir_name);
int getCurrentMethodVarCount();
Kinds getKind(Token t);
int getIndexOf(Token t);

char *getSubroutineType(char *class, char *method);

#endif