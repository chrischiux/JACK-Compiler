
/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Symbol Tables Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Chris Chiu
Student ID: 201594017
Email: sc22phc@leeds.ac.uk
Date Work Commenced: 24/03/24
*************************************************************************/

#include "symbols.h"
#include <string.h>
#include <stdio.h>

ClassTable table[100];
int classCount = 0;
ClassTable *currentClass;
MethodTable *currentMethod;
Token classStack[100];
int classStackCount = 0;
Subroutine subroutineStack[100];
int subroutineCount = 0;

FILE *outputFile;


void NewClass(Token token)
{
	currentClass = &table[classCount];
	strcpy(currentClass->name, token.lx);
	currentClass->methodCount = 0;
	currentClass->symbolCount = 0;
	currentClass->staticCount = 0;
	currentClass->fieldCount = 0;
	classCount++;
}

// 0 = success, 1 = redeclaration
int InsertClassSymbol(char *name, char *type, Kinds kind)
{
	if (CheckClassSymbolRedeclare(name))
	{
		return 1;
	}
	// }

	// insert symbol
	strcpy(currentClass->symbols[currentClass->symbolCount].name, name);
	strcpy(currentClass->symbols[currentClass->symbolCount].type, type);
	currentClass->symbols[currentClass->symbolCount].kind = kind;

	if (kind == field)
	{
		currentClass->symbols[currentClass->symbolCount].index = currentClass->fieldCount;
		currentClass->fieldCount++;
	}
	else if (kind == stat)
	{
		currentClass->symbols[currentClass->symbolCount].index = currentClass->staticCount;
		currentClass->staticCount++;
	}
	currentClass->symbolCount++;
	return 0;
}

void NewMethod(char *name)
{
	strcpy(currentClass->methods[currentClass->methodCount].name, name);
	currentMethod = &currentClass->methods[currentClass->methodCount];
	currentMethod->count = 0;
	currentMethod->argCount = 0;
	currentMethod->varCount = 0;
	currentClass->methodCount++;
}

// 0 = success, 1 = redeclaration
int InsertMethodSymbol(char *name, char *type, Kinds kind)
{
	if (ExternalLibCheck(type))
	{
		;
	}
	else
	// check redeclaration
	{
		if (CheckMethodSymbolRedeclare(name))
		{
			return 1;
		}
	}
	strcpy(currentMethod->symbols[currentMethod->count].name, name);
	strcpy(currentMethod->symbols[currentMethod->count].type, type);
	currentMethod->symbols[currentMethod->count].kind = kind;

	if (kind == arg)
	{
		currentMethod->argCount++;
	}
	else if (kind == var)
	{
		currentMethod->varCount++;
	}

	currentMethod->count++;
	return 0;
}

int CheckClassSymbolRedeclare(char *name)
{
	for (int i = 0; i < currentClass->symbolCount; i++)
	{
		if (strcmp(currentClass->symbols[i].name, name) == 0)
		{
			return 1;
		}
	}
	return 0;
}

int CheckClassSymbolDeclare(char *name)
{
	for (int i = 0; i < currentClass->symbolCount; i++)
	{
		if (strcmp(currentClass->symbols[i].name, name) == 0)
		{
			return 1;
		}
	}
	return 0;
}

int CheckMethodSymbolRedeclare(char *name)
{
	for (int i = 0; i < currentMethod->count; i++)
	{
		if (strcmp(currentMethod->symbols[i].name, name) == 0)
		{
			return 1;
		}
	}
	return 0;
}

void ClassStackPush(Token token)
{
	// create deep copy of token lx
	strcpy(classStack[classStackCount].lx, token.lx);
	classStack[classStackCount].ln = token.ln;
	classStack[classStackCount].tp = token.tp;
	strcpy(classStack[classStackCount].fl, token.fl);
	classStackCount++;
}

Token CheckClassStack()
{
	Token found;
	for (int i = 0; i < classStackCount; i++)
	{
		if (ExternalLibCheck(classStack[i].lx))
		{
			continue;
		}

		found.ec = undecIdentifier;
		for (int j = 0; j < classCount; j++)
		{
			if (strcmp(classStack[i].lx, table[j].name) == 0)
			{
				found.ec = none;
			}
		}
		if (found.ec == undecIdentifier)
		{
			classStack[i].ec = found.ec;
			return classStack[i];
		}
	}
	found.ec = none;
	return found;
}

Token CheckVarDeclar(Token token)
{
	Token found;
	found.ec = undecIdentifier;
	for (int i = 0; i < currentMethod->count; i++)
	{
		if (strcmp(token.lx, currentMethod->symbols[i].name) == 0)
		{
			found.ec = none;
			return found;
		}
	}
	for (int i = 0; i < currentClass->symbolCount; i++)
	{
		if (strcmp(token.lx, currentClass->symbols[i].name) == 0)
		{
			found.ec = none;
			return found;
		}
	}
	if (found.ec == undecIdentifier)
	{
		token.ec = found.ec;
		return token;
	}
}

Token CheckMethodDeclar(Token class, Token method)
{
	Token found;
	found.ec = undecIdentifier;
	for (int i = 0; i < classCount; i++)
	{
	}
	if (found.ec == undecIdentifier)
	{
		method.ec = found.ec;
		return method;
	}
}

Token CheckVarMember(char *name)
{
	Token found;
	found.ec = 1;
	for (int i = 0; i < currentMethod->count; i++)
	{
		if (strcmp(name, currentMethod->symbols[i].name) == 0)
		{
			strcpy(found.lx, currentMethod->symbols[i].type);
			return found;
		}
	}
	for (int i = 0; i < currentClass->symbolCount; i++)
	{
		if (strcmp(name, currentClass->symbols[i].name) == 0)
		{
			strcpy(found.lx, currentClass->symbols[i].type);
			return found;
		}
	}
	found.ec = 0;
	return found;
}

int ExternalLibMethodCheck(char *class, char *method)
{
	if (!strcmp(class, "Math"))
	{
		if (!strcmp(method, "abs") || !strcmp(method, "max") || !strcmp(method, "min") || !strcmp(method, "sqrt") || !strcmp(method, "multiply") || !strcmp(method, "divide"))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else if (!strcmp(class, "Memory"))
	{
		if (!strcmp(method, "peek") || !strcmp(method, "poke") || !strcmp(method, "alloc") || !strcmp(method, "deAlloc"))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else if (!strcmp(class, "Screen"))
	{
		if (!strcmp(method, "clearScreen") || !strcmp(method, "setColor") || !strcmp(method, "drawLine") || !strcmp(method, "drawRectangle") || !strcmp(method, "drawCircle") || !strcmp(method, "drawPixel"))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else if (!strcmp(class, "Keyboard"))
	{
		if (!strcmp(method, "keyPressed") || !strcmp(method, "readChar") || !strcmp(method, "readLine") || !strcmp(method, "readInt"))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else if (!strcmp(class, "Output"))
	{
		if (!strcmp(method, "init") || !strcmp(method, "moveCursor") || !strcmp(method, "printChar") || !strcmp(method, "printString") || !strcmp(method, "printInt") || !strcmp(method, "println") || !strcmp(method, "backSpace"))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else if (!strcmp(class, "String"))
	{
		if (!strcmp(method, "new") || !strcmp(method, "length") || !strcmp(method, "dispose") || !strcmp(method, "charAt")|| !strcmp(method, "setCharAt") || !strcmp(method, "appendChar") || !strcmp(method, "eraseLastChar") || !strcmp(method, "intValue") || !strcmp(method, "setInt") || !strcmp(method, "newLine") || !strcmp(method, "backSpace") || !strcmp(method, "doubleQuote"))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else if (!strcmp(class, "Array"))
	{
		if (!strcmp(method, "new") || !strcmp(method, "dispose"))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else if (!strcmp(class, "Sys"))
	{
		if (!strcmp(method, "halt") || !strcmp(method, "error") || !strcmp(method, "wait"))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
}

void SubStackInsert(char *class, char *method, Token token)
{
	int isExternal = 0;
	char external[128];
	if (ExternalLibCheck(class))
	{
		isExternal = 1;
		strcpy(external, class);
	}
	else
	{
		for (int i = 0; i < currentClass->methodCount; i++)
		{
			if (strcmp(class, currentMethod->symbols->name) == 0)
			{
				strcpy(external, currentMethod->symbols->type);
				isExternal = 1;
				break;
			}
		}
	}

	if (isExternal == 0)
	{
		strcpy(subroutineStack[subroutineCount].class, class);
		strcpy(subroutineStack[subroutineCount].method, method);

		strcpy(subroutineStack[subroutineCount].token.fl, token.fl);
		subroutineStack[subroutineCount].token.ln = token.ln;
		subroutineStack[subroutineCount].token.tp = token.tp;
		strcpy(subroutineStack[subroutineCount].token.lx, token.lx);
	}
	else
	{
		strcpy(subroutineStack[subroutineCount].class, external);
		strcpy(subroutineStack[subroutineCount].method, method);

		strcpy(subroutineStack[subroutineCount].token.fl, token.fl);
		subroutineStack[subroutineCount].token.ln = token.ln;
		subroutineStack[subroutineCount].token.tp = token.tp;
		strcpy(subroutineStack[subroutineCount].token.lx, token.lx);
	}

	subroutineCount++;
	return;
}

int ExternalLibCheck(char *class)
{
	if ((!strcmp(class, "Math")) || (!strcmp(class, "Memory")) || (!strcmp(class, "Output")) || (!strcmp(class, "Screen")) || (!strcmp(class, "Keyboard")) || !(strcmp(class, "Array")) || (!strcmp(class, "String")) || (!strcmp(class, "Sys")))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

Token CheckSubStack()
{
	int found, classFound;
	for (int i = 0; i < subroutineCount; i++)
	{
		// external library check
		if (ExternalLibCheck(subroutineStack[i].class))
		{
			if (ExternalLibMethodCheck(subroutineStack[i].class, subroutineStack[i].method))
			{
				subroutineStack[i].token.ec = NoLexErr;
				strcpy(subroutineStack[i].token.lx, subroutineStack[i].method);
				return subroutineStack[i].token;
			}
			else
			{
				continue;
			}
		}
		classFound = 0;
		found = 0;
		for (int j = 0; j < classCount; j++)
		{
			if (strcmp(subroutineStack[i].class, table[j].name) == 0)
			{
				classFound = 1;
				for (int k = 0; k < table[j].methodCount; k++)
				{
					if (strcmp(subroutineStack[i].method, table[j].methods[k].name) == 0)
					{
						found = 1;
					}
				}
			}
		}
		if (classFound == 0)
		{
			subroutineStack[i].token.ec = NoLexErr;
			strcpy(subroutineStack[i].token.lx, subroutineStack[i].class);
			return subroutineStack[i].token;
		}
		else if (found == 0)
		{
			subroutineStack[i].token.ec = NoLexErr;
			return subroutineStack[i].token;
		}
	}
	Token foundToken;
	foundToken.ec = none;
	return foundToken;
}

void initSymbolTable()
{
	classCount = 0;
	classStackCount = 0;
	subroutineCount = 0;
}

char *getCurrentClass()
{
	return currentClass->name;
}

void printAll(){
	for (int i = 0; i < classCount; i++)
	{
		printf("====================================\n");
		printf("Class: %s | %i\n", table[i].name, table[i].symbolCount);
		for (int j = 0; j < table[i].symbolCount; j++)
		{
			printf("%s, %s, %d\n", table[i].symbols[j].name, table[i].symbols[j].type, table[i].symbols[j].kind);
		}
		for (int j = 0; j < table[i].methodCount; j++)
		{
			printf("\nMethod: %s | %i\n", table[i].methods[j].name, table[i].methods[j].count);
			for (int k = 0; k < table[i].methods[j].count; k++)
			{
				printf("%s, %s, %d\n", table[i].methods[j].symbols[k].name, table[i].methods[j].symbols[k].type, table[i].methods[j].symbols[k].kind);
			}
		}
	}
}

void CodeGeneration(char* dir_name){
	
	for(int i = 0; i < classCount; i++){
		char filename[256];
		sprintf(filename, "%s/%s.vm", dir_name, table[i].name);
		outputFile = fopen(filename, "w");

		for(int j = 0; j < table[i].methodCount; j++){
			fprintf(outputFile, "function %s.%s %i\n", table[i].name, table[i].methods[j].name, table[i].methods[j].varCount);
		}
		// fprintf(outputFile, "class %s\n", table[i].name);
		// for(int j = 0; j < table[i].symbolCount; j++){
		// 	fprintf(outputFile, "field %s %s %d\n", table[i].symbols[j].name, table[i].symbols[j].type, table[i].symbols[j].index);
		// }
		// for(int j = 0; j < table[i].methodCount; j++){
		// 	fprintf(outputFile, "subroutine %s\n", table[i].methods[j].name);
		// 	for(int k = 0; k < table[i].methods[j].count; k++){
		// 		fprintf(outputFile, "var %s %s %d\n", table[i].methods[j].symbols[k].name, table[i].methods[j].symbols[k].type, table[i].methods[j].symbols[k].index);
		// 	}
		// }
		fclose(outputFile);

	}
}