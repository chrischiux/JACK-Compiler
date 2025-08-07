/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name:
Student ID:
Email:
Date Work Commenced:
*************************************************************************/

#include "compiler.h"
#include "symbols.h"
#include "parser.h"
#include "lexer.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>

int InitCompiler()
{
	return 1;
}

ParserInfo compile(char *dir_name)
{
	ParserInfo p;
	DIR *d;
	Token t;
	struct dirent *dir;
	char full_path[256];
	d = opendir(dir_name);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			
			char *dot = strrchr(dir->d_name, '.');
			if (dot && !strcmp(dot, ".jack"))
			{
				strcpy(full_path, dir_name);
				strcat(full_path, "/");
				strcat(full_path, dir->d_name);

				printf("%s\n", full_path);

				InitLexer(full_path);
				ParserInfo pi = Parse();

				if (pi.er != none)
				{
					return pi;
				}
				
				StopLexer();
			}
			
			t = CheckClassStack();
			if (t.ec != none)
			{
				p.er = undecIdentifier;
				p.tk = t;
				return p;
			}

		}
		closedir(d);
	}

	p.er = none;
	return p;
}

int StopCompiler()
{

	return 1;
}

// int PrintError(ParserInfo p)
// {
// 	printf("Error: %d\n", p.er);
// }

#ifndef TEST_COMPILER
int main()
{
	InitCompiler();
	ParserInfo p = compile("Pong");
	// PrintError (p);
	printf("%d\n\n", p.er);
	p = compile("UNDECLAR_CLASS");
	// PrintError (p);
	printf("%d\n", p.er);
	StopCompiler();
	return 1;
}
#endif
