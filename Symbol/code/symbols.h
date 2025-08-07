#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

// define your own types and function prototypes for the symbol table(s) module below


typedef enum {banana , gelato, bapple} DataTypes;

typedef struct {
	char name[128];
	DataTypes type;
	int address;
} Symbol;




typedef struct {
	char name[128];
} MethodTable;

typedef struct {
	char name[128];
    MethodTable child[100];
} ClassTable;

typedef struct {
	char name[128];
    ClassTable child[100];
} ProgramTable;

void InsertSymbol(char* name, DataTypes type);
int FindSymbol(char* name); // returns the index of the symbol if found, otherwise returns -1 !!!
void InitSymbolTable();


#endif