
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

Symbol St[100];  // symbol table
int ns; // number of symbols in the table
int addreesCounter;

void InsertSymbol(char* name, DataTypes type)
{
	strcpy (St[ns].name , name);
	St[ns].type = type;
	St[ns].address = addreesCounter++;
	ns++;
}

int FindSymbol(char* name)
{
	for (int i = 0; i < ns; i++)
	{
		if (!strcmp(St[i].name, name))
		{
			return i; //Return idnex
		}

	}
	return -1; //Index was not found
}

void InitSymbolTable()
{
	ns = 0;
	addreesCounter = 0;
}