#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"

// you can declare prototypes of parser functions below
void error(const char *, Token);
ParserInfo classDeclar();
ParserInfo memberDeclar();
ParserInfo classVarDeclar();
ParserInfo type();
ParserInfo subroutineDeclar();
ParserInfo paramList();
ParserInfo subroutineBody();
ParserInfo statement();
ParserInfo varDeclarStatement();
ParserInfo letStatement();
ParserInfo ifStatement();
ParserInfo whileStatement();
ParserInfo doStatement();
ParserInfo subroutineCall();
ParserInfo expressionList();
ParserInfo returnStatement();
ParserInfo expression();
ParserInfo relationalExpression();
ParserInfo ArithmeticExpression();
ParserInfo term();
ParserInfo factor();
ParserInfo operand();
FILE *outputFile;
int commaCount = 0;
long numberOfVars = 0;
int ifCount = 0;
int whileCount = 0;

void error(const char *msg, Token t)
{
	printf("Line %i at or near %s: %s\n", t.ln, t.lx, msg);
	exit(1);
}

ParserInfo classDeclar()
{
	ParserInfo err;
	err.er = none;

	Token t = GetNextToken();
	if (t.tp == ERR)
	{
		err.er = lexerErr;
		err.tk = t;
		return err;
	}

	if (t.tp == RESWORD && !strcmp(t.lx, "class"))
		;
	else
	{
		err.er = classExpected;
		err.tk = t;
		return err;
	}

	t = GetNextToken();
	if (t.tp == ID)
		NewClass(t);
	else
	{
		err.er = idExpected;
		err.tk = t;
		return err;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "{"))
		;
	else
	{
		err.er = openBraceExpected;
		err.tk = t;
		return err;
	}

	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx, "}"))
	{
		err = memberDeclar();
		if (err.er != none)
			return err;
		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}"))
		;
	else
	{
		err.er = closeBraceExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo memberDeclar()
{
	ParserInfo err;
	err.er = none;

	Token t = PeekNextToken();
	if (t.tp == ERR)
	{
		err.er = lexerErr;
		err.tk = t;
		return err;
	}

	if (t.tp == RESWORD && (!strcmp(t.lx, "static") || !strcmp(t.lx, "field")))
	{
		err = classVarDeclar();
		if (err.er != none)
		{
			return err;
		}
	}
	else if (t.tp == RESWORD && (!strcmp(t.lx, "constructor") || !strcmp(t.lx, "function") || !strcmp(t.lx, "method")))
	{
		err = subroutineDeclar();
		if (err.er != none)
		{
			return err;
		}
	}
	else
	{
		err.er = syntaxError;
		err.tk = t;
		return err;
	} // class member declaration must be var declar or subroutine declar
	return err;
}

ParserInfo classVarDeclar()
{
	ParserInfo err;
	err.er = none;
	int kind;
	char symbolType[128];

	Token t = GetNextToken();
	if (t.tp == ERR)
	{
		err.er = lexerErr;
		err.tk = t;
		return err;
	}

	if (t.tp == RESWORD && (!strcmp(t.lx, "static") || !strcmp(t.lx, "field")))
		if (!strcmp(t.lx, "static"))
			kind = stat;
		else
			kind = field;
	else
	{
		err.er = classVarErr;
		err.tk = t;
		return err;
	}

	strcpy(symbolType, PeekNextToken().lx);
	err = type();
	if (err.er != none)
	{
		return err;
	}

	t = GetNextToken();
	if (t.tp == ID)
	{
		if (InsertClassSymbol(t.lx, symbolType, kind) == 1)
		{
			err.er = redecIdentifier;
			err.tk = t;
			return err;
		}
	}
	else
	{
		err.er = idExpected;
		err.tk = t;
		return err;
	}

	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx, ";"))
	{
		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, ","))
			;
		else
		{
			err.er = syntaxError;
			err.tk = t;
			return err;
		}

		t = GetNextToken();
		if (t.tp == ID)
		{
			if (InsertClassSymbol(t.lx, symbolType, kind) == 1)
			{
				err.er = redecIdentifier;
				err.tk = t;
				return err;
			}
		}
		else
		{
			err.er = idExpected;
			err.tk = t;
			return err;
		}

		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";"))
		;
	else
	{
		err.er = semicolonExpected;
		err.tk = t;
		return err;
	}

	return err;
}

ParserInfo type()
{
	ParserInfo err;
	err.er = none;

	Token t = GetNextToken();
	if (t.tp == ERR)
	{
		err.er = lexerErr;
		err.tk = t;
		return err;
	}

	if (t.tp == RESWORD && (!strcmp(t.lx, "int") || !strcmp(t.lx, "char") || !strcmp(t.lx, "boolean")))
		;
	else if (t.tp == ID)
		ClassStackPush(t);
	else
	{
		err.er = illegalType;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo subroutineDeclar()
{
	ParserInfo err;
	err.er = none;

	Token type = GetNextToken();
	Token t;
	Token returnType;
	if (type.tp == ERR)
	{
		err.er = lexerErr;
		err.tk = type;
		return err;
	}

	if (type.tp == RESWORD && (!strcmp(type.lx, "constructor") || !strcmp(type.lx, "function") || !strcmp(type.lx, "method")))
		;
	else
	{
		err.er = memberDeclarErr;
		err.tk = type;
		return err;
	}

	returnType = GetNextToken();
	if ((returnType.tp == RESWORD && (!strcmp(returnType.lx, "void") || !strcmp(returnType.lx, "int") || !strcmp(returnType.lx, "char") || !strcmp(returnType.lx, "boolean"))) || returnType.tp == ID)
		;
	else
	{
		err.er = illegalType;
		err.tk = returnType;
		return err;
	}

	t = GetNextToken();
	if (t.tp == ID)
	{
		NewMethod(t.lx, type.lx);
		ifCount = 0;
		// whileCount = 0;

		// Code Generation function
		fprintf(outputFile, "function %s.%s ", getCurrentClass(), t.lx);
		numberOfVars = ftell(outputFile);
		fprintf(outputFile, " \n");
	}
	else
	{
		err.er = idExpected;
		err.tk = t;
		return err;
	}

	if (!strcmp(type.lx, "constructor"))
	{
		// Code Generation constructor new
		fprintf(outputFile, "push constant %i\n", getCurrentClassVarCount());
		fprintf(outputFile, "call Memory.alloc 1\n");
		fprintf(outputFile, "pop pointer 0\n");
	}
	else if (!strcmp(type.lx, "method"))
	{
		// Code Generation method
		fprintf(outputFile, "push argument 0\n");
		fprintf(outputFile, "pop pointer 0\n");
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "("))
		;
	else
	{
		err.er = openParenExpected;
		err.tk = t;
		return err;
	}

	err = paramList();
	if (err.er != none)
	{
		return err;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
		;
	else
	{
		err.er = closeParenExpected;
		err.tk = t;
		return err;
	}

	err = subroutineBody();

	// Code Generation arg num
	fseek(outputFile, numberOfVars, SEEK_SET);
	fprintf(outputFile, "%d", getCurrentMethodVarCount());
	fseek(outputFile, 0, SEEK_END);

	return err;
}

ParserInfo paramList()
{
	ParserInfo err;
	err.er = none;
	char symbolType[128];

	Token t = PeekNextToken();
	if (t.tp == ERR)
	{
		err.er = lexerErr;
		err.tk = t;
		return err;
	}

	if ((t.tp == RESWORD && (!strcmp(t.lx, "int") || !strcmp(t.lx, "char") || !strcmp(t.lx, "boolean"))) || t.tp == ID)
	{
		strcpy(symbolType, t.lx);
		err = type();
		if (err.er != none)
		{
			return err;
		}

		t = GetNextToken();
		if (t.tp == ID)
			InsertMethodSymbol(t.lx, symbolType, arg);
		else
		{
			err.er = idExpected;
			err.tk = t;
			return err;
		}

		t = PeekNextToken();
		while (t.tp != EOFile && !strcmp(t.lx, ","))
		{
			t = GetNextToken();
			if (t.tp == SYMBOL && !strcmp(t.lx, ","))
				;
			else
			{
				err.er = syntaxError;
				err.tk = t;
				return err;
			} //,

			strcpy(symbolType, PeekNextToken().lx);
			err = type();
			if (err.er != none)
			{
				return err;
			}

			t = GetNextToken();
			if (t.tp == ID)
				InsertMethodSymbol(t.lx, symbolType, arg);
			else
			{
				err.er = idExpected;
				err.tk = t;
				return err;
			}

			t = PeekNextToken();
		}
	}
	return err;
}

ParserInfo subroutineBody()
{
	ParserInfo err;
	err.er = none;

	Token t = GetNextToken();
	if (t.tp == ERR)
	{
		err.er = lexerErr;
		err.tk = t;
		return err;
	}

	if (t.tp == SYMBOL && !strcmp(t.lx, "{"))
		;
	else
	{
		err.er = openBraceExpected;
		err.tk = t;
		return err;
	}

	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx, "}"))
	{
		err = statement();
		if (err.er != none)
		{
			return err;
		}
		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}"))
		;
	else
	{
		err.er = closeBraceExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo statement()
{
	ParserInfo err;
	err.er = none;

	Token t = PeekNextToken();
	if (t.tp == ERR)
	{
		err.er = lexerErr;
		err.tk = t;
		return err;
	}

	if (t.tp == RESWORD)
	{
		if (!strcmp(t.lx, "var"))
			err = varDeclarStatement();
		else if (!strcmp(t.lx, "let"))
			err = letStatement();
		else if (!strcmp(t.lx, "if"))
			err = ifStatement();
		else if (!strcmp(t.lx, "while"))
			err = whileStatement();
		else if (!strcmp(t.lx, "do"))
			err = doStatement();
		else if (!strcmp(t.lx, "return"))
			err = returnStatement();
		else
		{
			err.er = syntaxError;
			err.tk = t;
			return err;
		}
		if (err.er != none)
		{
			return err;
		}
	}
	else
	{
		err.er = syntaxError;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo varDeclarStatement()
{
	ParserInfo err;
	err.er = none;
	char symbolType[128];

	Token t = GetNextToken();
	if (t.tp == ERR)
	{
		err.er = lexerErr;
		err.tk = t;
		return err;
	}

	if (t.tp == RESWORD && !strcmp(t.lx, "var"))
		;
	else
	{
		err.er = syntaxError;
		err.tk = t;
		return err;
	}

	strcpy(symbolType, PeekNextToken().lx);
	err = type();
	if (err.er != none)
	{
		return err;
	}

	t = GetNextToken();
	if (t.tp == ID)
	{
		if (InsertMethodSymbol(t.lx, symbolType, var) == 1)
		{
			err.er = redecIdentifier;
			err.tk = t;
			return err;
		}
	}
	else
	{
		err.er = idExpected;
		err.tk = t;
		return err;
	}

	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx, ";"))
	{
		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, ","))
			;
		else
		{
			err.er = syntaxError;
			err.tk = t;
			return err;
		} //,

		t = GetNextToken();
		if (t.tp == ID)
		{
			if (InsertMethodSymbol(t.lx, symbolType, var) == 1)
			{
				err.er = redecIdentifier;
				err.tk = t;
				return err;
			}
		}
		else
		{
			err.er = idExpected;
			err.tk = t;
			return err;
		}

		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";"))
		;
	else
	{
		err.er = semicolonExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo letStatement()
{
	ParserInfo err;
	err.er = none;

	Token t = GetNextToken();
	Token destination;
	// 0 = not array, 1 = is array
	int array = 0;

	if (t.tp == ERR)
	{
		err.er = lexerErr;
		err.tk = t;
		return err;
	}

	if (t.tp == RESWORD && !strcmp(t.lx, "let"))
		;
	else
	{
		err.er = syntaxError;
		err.tk = t;
		return err;
	}

	destination = GetNextToken();
	if (destination.tp == ID)
	{
		if (CheckVarDeclar(destination).ec != none)
		{
			err.er = undecIdentifier;
			err.tk = destination;
			return err;
		}
	}
	else
	{
		err.er = idExpected;
		err.tk = destination;
		return err;
	}

	t = PeekNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "["))
	{
		array = 1;
		t = GetNextToken();

		err = expression();
		if (err.er != none)
		{
			return err;
		}

		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "]"))
			;
		else
		{
			err.er = closeBracketExpected;
			err.tk = t;
			return err;
		}

		// Code Generation
		if (getKind(destination) == var)
		{
			fprintf(outputFile, "push local %i\n", getIndexOf(destination));
		}
		else if (getKind(destination) == arg)
		{
			fprintf(outputFile, "push argument %i\n", getIndexOf(destination));
		}
		else if (getKind(destination) == stat)
		{
			fprintf(outputFile, "push static %i\n", getIndexOf(destination));
		}
		else if (getKind(destination) == field)
		{
			fprintf(outputFile, "push this %i\n", getIndexOf(destination));
		}
		fprintf(outputFile, "add\n");
	}

	// t = PeekNextToken();
	// if (t.tp == SYMBOL && !strcmp(t.lx, "["))
	// {
	// 	t = GetNextToken();
	// 	if (t.tp == SYMBOL && !strcmp(t.lx, "["))
	// 		;
	// 	else
	// 		error("[ expected", t);

	// 	t = PeekNextToken();
	// 	if (t.tp == RESWORD || t.tp == ID)
	// 		;
	// 	else
	// 		error("expression expected", t);

	// 	t = GetNextToken();
	// 	if (t.tp == SYMBOL && !strcmp(t.lx, "]"))
	// 		;
	// 	else
	// 		return  closeBracketExpected;
	// }

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "="))
		;
	else
	{
		err.er = equalExpected;
		err.tk = t;
		return err;
	}

	// symbol declar check
	//  if(PeekNextToken().tp == ID){
	//  	if(CheckVarDeclar(PeekNextToken()).ec != none)
	//  	{
	//  		err.er = undecIdentifier;
	//  		err.tk = PeekNextToken();
	//  		return err;
	//  	}
	//  }

	err = expression();
	if (err.er != none)
	{
		return err;
	}

	// Code Generation
	if (array == 1)
	{
		// ARRAY IMPLEMENTATION, CHECK
		fprintf(outputFile, "pop temp 0\n");
		fprintf(outputFile, "pop pointer 1\n");
		fprintf(outputFile, "push temp 0\n");
		fprintf(outputFile, "pop that 0\n");
	}
	else
	{
		Kinds temp = getKind(destination);
		int index = getIndexOf(destination);
		if (temp == field)
		{
			fprintf(outputFile, "pop this %i\n", index);
		}
		else if (temp == stat)
		{
			fprintf(outputFile, "pop static %i\n", index);
		}
		else if (temp == arg)
		{
			fprintf(outputFile, "pop argument %i\n", index);
		}
		else if (temp == var)
		{
			fprintf(outputFile, "pop local %i\n", index);
		}
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";"))
		;
	else
	{
		err.er = semicolonExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo ifStatement()
{
	ParserInfo err;
	err.er = none;

	Token t = GetNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "if"))
		;
	else
	{
		err.er = syntaxError;
		err.tk = t;
		return err;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "("))
		;
	else
	{
		err.er = openParenExpected;
		err.tk = t;
		return err;
	}

	err = expression();
	if (err.er != none)
	{
		return err;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
		;
	else
	{
		err.er = closeParenExpected;
		err.tk = t;
		return err;
	}

	// Code Generation
	fprintf(outputFile, "if-goto IF_TRUE%i\n", ifCount);
	fprintf(outputFile, "goto IF_FALSE%i\n", ifCount);
	fprintf(outputFile, "label IF_TRUE%i\n", ifCount);

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "{"))
		;
	else
	{
		err.er = openBraceExpected;
		err.tk = t;
		return err;
	}

	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx, "}"))
	{
		t = PeekNextToken();
		if (t.tp == RESWORD)
		{
			err = statement();
			if (err.er != none)
			{
				return err;
			}
		}
		else
			break;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}"))
		;
	else
	{
		err.er = closeBraceExpected;
		err.tk = t;
		return err;
	}

	t = PeekNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "else"))
	{
		// Code Generation
		fprintf(outputFile, "goto IF_END%i\n", ifCount);
		fprintf(outputFile, "label IF_FALSE%i\n", ifCount);

		t = GetNextToken();
		// if (t.tp == RESWORD && !strcmp(t.lx, "else"))
		// 	;
		// else
		// 	error("else expected", t);

		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "{"))
			;
		else
		{
			err.er = openBraceExpected;
			err.tk = t;
			return err;
		}

		t = PeekNextToken();
		while (t.tp != EOFile && strcmp(t.lx, "}"))
		{
			t = PeekNextToken();
			if (t.tp == RESWORD)
			{
				err = statement();
				if (err.er != none)
				{
					return err;
				}
			}
			else
				break;
		}

		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "}"))
			;
		else
		{
			err.er = closeBraceExpected;
			err.tk = t;
			return err;
		}

		// Code Generation
		fprintf(outputFile, "label IF_END%i\n", ifCount);
	}
	else
	{
		fprintf(outputFile, "label IF_FALSE%i\n", ifCount);
	}
	ifCount++;
	return err;
}

ParserInfo whileStatement()
{
	ParserInfo err;
	err.er = none;
	int currentWhileCount = whileCount;

	Token t = GetNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "while"))
		;
	else
	{
		err.er = syntaxError;
		err.tk = t;
		return err;
	}

	// Code Generation
	fprintf(outputFile, "label WHILE_EXP%i\n", currentWhileCount);

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "("))
		;
	else
	{
		err.er = openParenExpected;
		err.tk = t;
		return err;
	}

	err = expression();
	if (err.er != none)
	{
		return err;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
		;
	else
	{
		err.er = closeParenExpected;
		err.tk = t;
		return err;
	}

	// Code Generation
	fprintf(outputFile, "not\n");
	fprintf(outputFile, "if-goto WHILE_END%i\n", currentWhileCount);

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "{"))
		;
	else
	{
		err.er = openBraceExpected;
		err.tk = t;
		return err;
	}

	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx, "}"))
	{
		if (t.tp == RESWORD)
		{
			err = statement();
			if (err.er != none)
			{
				return err;
			}
		}
		else
		{
			break;
		}
		t = PeekNextToken();
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}"))
	{
		// Code Generation
		fprintf(outputFile, "goto WHILE_EXP%i\n", currentWhileCount);
		fprintf(outputFile, "label WHILE_END%i\n", currentWhileCount);
		whileCount++;
	}
	else
	{
		err.er = closeBraceExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo doStatement()
{
	ParserInfo err;
	err.er = none;

	Token t = GetNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "do"))
		;
	else
	{
		err.er = syntaxError;
		err.tk = t;
		return err;
	}

	err = subroutineCall();
	if (err.er != none)
	{
		return err;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";"))
		;
	else
	{
		err.er = semicolonExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo subroutineCall()
{
	ParserInfo err;
	err.er = none;
	char className[128];
	char methodName[128];
	Token temp;
	int isExt = 0;

	Token first = GetNextToken();
	if (first.tp == ID)
	{
		temp = CheckVarMember(first.lx);
		if (temp.ec = 0)
		{
			strcpy(className, first.lx);
		}
		else
		{
			strcpy(className, temp.lx);

			// Code Generation
			if (getIndexOf(first) != -1 && getKind(first) == var)
			{
				fprintf(outputFile, "push local %i\n", getIndexOf(first));
				isExt = 1;
			}
			else if (getIndexOf(first) != -1 && getKind(first) == field)
			{
				fprintf(outputFile, "push this %i\n", getIndexOf(first));
				isExt = 1;
			}
			// else if (getIndexOf(first) != -1 && getKind(first) == arg)
			// {
			// 	fprintf(outputFile, "push argument %i\n", getIndexOf(first));
			// 	isExt = 1;
			// }
			else if (getIndexOf(first) != -1 && getKind(first) == stat)
			{
				fprintf(outputFile, "push static %i\n", getIndexOf(first));
				isExt = 1;
			}
			// else
			// {
			// 	fprintf(outputFile, "push pointer 0\n");
			// }
		}
	}
	else
	{
		err.er = idExpected;
		err.tk = first;
		return err;
	}

	Token t = PeekNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "."))
	{
		t = GetNextToken();

		t = GetNextToken();
		if (t.tp == ID)
		{
			strcpy(methodName, t.lx);
			SubStackInsert(className, t.lx, t);
		}
		else
		{
			err.er = idExpected;
			err.tk = t;
			return err;
		}
	}
	else
	{
		SubStackInsert(getCurrentClass(), className, first);
		strcpy(methodName, className);
		strcpy(className, getCurrentClass());

		// Code Generation
		fprintf(outputFile, "push pointer 0\n");
		isExt = 1;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "("))
		;
	else
	{
		err.er = openParenExpected;
		err.tk = t;
		return err;
	}

	commaCount = 0;
	err = expressionList();

	if (err.er != none)
	{
		return err;
	}

	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
		;
	else
	{
		err.er = closeParenExpected;
		err.tk = t;
		return err;
	}

	// Code Generation

	if (ExternalLibCheck(className))
	{
		fprintf(outputFile, "call %s.%s %i\n", className, methodName, commaCount);
	}
	else if (!strcmp(getCurrentClass(), className))
	{
		fprintf(outputFile, "call %s.%s %i\n", className, methodName, commaCount + isExt);
	}
	// else if(!strcmp(getSubroutineType(className, methodName), "method"))
	// {
	// 	fprintf(outputFile, "push pointer 0\n");
	// 	fprintf(outputFile, "call %s.%s %i\n", className, methodName, commaCount + 1);
	// }
	// else
	// {
	// 	fprintf(outputFile, "call %s.%s %i\n", className, methodName, commaCount + isExt);
	// }
	else
	{
		// fprintf(outputFile, "push pointer 0\n");
		fprintf(outputFile, "call %s.%s %i\n", className, methodName, commaCount + isExt);
	}

	fprintf(outputFile, "pop temp 0\n");

	return err;
}

ParserInfo expressionList()
{
	ParserInfo err;
	err.er = none;
	int argCounter = 0;

	Token t = PeekNextToken();
	if (t.tp == RESWORD || t.tp == ID || (t.tp == SYMBOL && strcmp(t.lx, ")")) || t.tp == STRING || t.tp == INT) // NEED TO CHECK
	// if (t.tp == RESWORD || t.tp == ID || t.tp == STRING || t.tp == INT)
	{
		argCounter++;
		err = expression();
		if (err.er != none)
		{
			return err;
		}

		t = PeekNextToken();
		while (t.tp != EOFile && !strcmp(t.lx, ","))
		{

			t = GetNextToken();
			// if (t.tp == SYMBOL && !strcmp(t.lx, ","))
			// 	;
			// else
			// 	{err.er = syntaxError;
			// 		err.tk = t;
			// 		return err;
			// 	} //,

			err = expression();

			if (err.er != none)
			{
				return err;
			}
			argCounter++;
			t = PeekNextToken();
		}
	}
	commaCount = argCounter;
	return err;
}

ParserInfo returnStatement()
{
	ParserInfo err;
	err.er = none;

	Token t = GetNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "return"))
		;
	else
	{
		err.er = syntaxError;
		err.tk = t;
		return err;
	}

	t = PeekNextToken();
	if (t.tp == RESWORD || t.tp == ID || t.tp == STRING || t.tp == INT)
	{
		// // Code Generation
		// if (t.tp == RESWORD && !strcmp(t.lx, "this"))
		// {
		// 	fprintf(outputFile, "push pointer 0\n");
		// }
		err = expression();
	}
	else
	{
		// Code Generation
		fprintf(outputFile, "push constant 0\n");
	}
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";"))

		// Code Generation
		fprintf(outputFile, "return\n");

	else
	{
		err.er = semicolonExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo expression()
{
	ParserInfo err;
	err.er = none;

	err = relationalExpression();
	if (err.er != none)
	{
		return err;
	}

	Token t = PeekNextToken();
	while (t.tp != EOFile && (!strcmp(t.lx, "&") || !strcmp(t.lx, "|")))
	{
		t = GetNextToken();
		// if (t.tp == SYMBOL && (!strcmp(t.lx, "&") || !strcmp(t.lx, "|")))
		// 	;
		// else
		// 	error("operator expected", t);

		err = relationalExpression();
		if (err.er != none)
		{
			return err;
		}

		if (t.tp == SYMBOL && !strcmp(t.lx, "&"))
			fprintf(outputFile, "and\n");
		else if (t.tp == SYMBOL && !strcmp(t.lx, "|"))
			fprintf(outputFile, "or\n");

		t = PeekNextToken();
	}
	return err;
}

ParserInfo relationalExpression()
{
	ParserInfo err;
	err.er = none;

	err = ArithmeticExpression();
	if (err.er != none)
	{
		return err;
	}

	Token t = PeekNextToken();
	if (t.tp == SYMBOL && (!strcmp(t.lx, "<") || !strcmp(t.lx, ">") || !strcmp(t.lx, "=")))
	{
		t = GetNextToken();

		err = ArithmeticExpression();
		if (err.er != none)
		{
			return err;
		}

		if (t.tp == SYMBOL && !strcmp(t.lx, ">"))
			fprintf(outputFile, "gt\n");
		else if (t.tp == SYMBOL && !strcmp(t.lx, "<"))
			fprintf(outputFile, "lt\n");
		else if (t.tp == SYMBOL && !strcmp(t.lx, "="))
			fprintf(outputFile, "eq\n");
	}
	return err;
}

ParserInfo ArithmeticExpression()
{
	ParserInfo err;
	err.er = none;

	err = term();
	if (err.er != none)
	{
		return err;
	}

	Token t = PeekNextToken();
	while (t.tp != EOFile && (!strcmp(t.lx, "+") || !strcmp(t.lx, "-")))
	{
		t = GetNextToken();
		// if (t.tp == SYMBOL && (!strcmp(t.lx, "+") || !strcmp(t.lx, "-")))
		// 	;
		// else
		// 	error("operator expected", t);

		err = term();
		if (err.er != none)
		{
			return err;
		}

		if (t.tp == SYMBOL && !strcmp(t.lx, "+"))
			fprintf(outputFile, "add\n");
		else if (t.tp == SYMBOL && !strcmp(t.lx, "-"))
			fprintf(outputFile, "sub\n");

		t = PeekNextToken();
	}
	return err;
}

ParserInfo term()
{
	ParserInfo err;
	err.er = none;

	err = factor();
	if (err.er != none)
	{
		return err;
	}

	Token t = PeekNextToken();
	while (t.tp != EOFile && (!strcmp(t.lx, "*") || !strcmp(t.lx, "/")))
	{
		t = GetNextToken();
		// if (t.tp == SYMBOL && (!strcmp(t.lx, "*") || !strcmp(t.lx, "/")))
		// 	;
		// else
		// 	error("operator expected", t);

		err = factor();
		if (err.er != none)
		{
			return err;
		}

		// Code Generation
		if (t.tp == SYMBOL && !strcmp(t.lx, "*"))
			fprintf(outputFile, "call Math.multiply 2\n");
		else if (t.tp == SYMBOL && !strcmp(t.lx, "/"))
			fprintf(outputFile, "call Math.divide 2\n");

		t = PeekNextToken();
	}
	return err;
}

ParserInfo factor()
{
	ParserInfo err;
	err.er = none;

	Token t = PeekNextToken();
	if (t.tp == SYMBOL && (!strcmp(t.lx, "-") || !strcmp(t.lx, "~")))
	{
		t = GetNextToken();
	}
	err = operand();

	// Code Generation
	if (t.tp == SYMBOL && !strcmp(t.lx, "-"))
		fprintf(outputFile, "neg\n");
	else if (t.tp == SYMBOL && !strcmp(t.lx, "~"))
		fprintf(outputFile, "not\n");

	return err;
}

ParserInfo operand()
{
	ParserInfo err;
	err.er = none;
	Token temp;
	Token result;
	char className[128];
	char methodName[128];
	int isArg = 0;
	int isExt = 0;

	Token t = PeekNextToken();
	if (t.tp == INT)
	{
		t = GetNextToken();

		// Code Generation
		fprintf(outputFile, "push constant %i\n", atoi(t.lx));
	}
	else if (t.tp == STRING)
	{
		t = GetNextToken();

		// Code Generation
		fprintf(outputFile, "push constant %li\n", strlen(t.lx));
		fprintf(outputFile, "call String.new 1\n");
		for (int i = 0; i < strlen(t.lx); i++)
		{
			fprintf(outputFile, "push constant %i\n", t.lx[i]);
			fprintf(outputFile, "call String.appendChar 2\n");
		}
	}
	else if (t.tp == RESWORD)
	{
		t = GetNextToken();
		if (t.tp == RESWORD && (!strcmp(t.lx, "true") || !strcmp(t.lx, "false") || !strcmp(t.lx, "null") || !strcmp(t.lx, "this")))
			;
		else
		{
			err.er = syntaxError;
			err.tk = t;
			return err;
		} // keyword

		// Code Generation
		if (!strcmp(t.lx, "true"))
		{
			fprintf(outputFile, "push constant 0\n");
			fprintf(outputFile, "not\n");
		}
		else if (!strcmp(t.lx, "false") || !strcmp(t.lx, "null"))
		{
			fprintf(outputFile, "push constant 0\n");
		}
		else if (!strcmp(t.lx, "this"))
		{
			fprintf(outputFile, "push pointer 0\n");
		}
	}
	else if (t.tp == ID)
	{
		temp = GetNextToken();
		result = CheckVarMember(temp.lx);
		if (temp.ec = 0)
		{
			strcpy(className, temp.lx);
		}
		else
		{
			strcpy(className, result.lx);

			// // Code Generation
			// fprintf(outputFile, "push local %i\n", getIndexOf(temp));
			// isExt = 1;
		}

		t = PeekNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "."))
		{
			t = GetNextToken();

			t = GetNextToken();
			if (t.tp == ID)
			{
				SubStackInsert(className, t.lx, t);
				strcpy(methodName, t.lx);
			}
			else
			{
				err.er = idExpected;
				err.tk = t;
				return err;
			}

			// Code Generation
			if (getKind(temp) == arg)
			{
				fprintf(outputFile, "push argument %i\n", getIndexOf(temp));
				isExt = 1;
			}
			else if (getKind(temp) == var)
			{
				fprintf(outputFile, "push local %i\n", getIndexOf(temp));
				isExt = 1;
			}
			else if (getKind(temp) == field)
			{
				fprintf(outputFile, "push this %i\n", getIndexOf(temp));
				isExt = 1;
			}
			// else if (getKind(temp) == stat)
			// {
			// 	fprintf(outputFile, "push static %i\n", getIndexOf(temp));
			// }
		}
		else
		{
			if (ExternalLibCheck(temp.lx) == 0)
			{
				if (CheckVarDeclar(temp).ec != none)
				{
					err.er = undecIdentifier;
					err.tk = temp;
					return err;
				}

				// Code Generation
				if (getKind(temp) == var)
				{
					t = PeekNextToken();
					if (t.tp == SYMBOL && strcmp(t.lx, "["))
					{
						fprintf(outputFile, "push local %i\n", getIndexOf(temp));
					}
				}
				else if (getKind(temp) == arg)
				{
					fprintf(outputFile, "push argument %i\n", getIndexOf(temp));
				}
				else if (getKind(temp) == field)
				{
					fprintf(outputFile, "push this %i\n", getIndexOf(temp));
				}
				else if (getKind(temp) == stat)
				{
					fprintf(outputFile, "push static %i\n", getIndexOf(temp));
				}
			}
		}

		t = PeekNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "["))
		{
			t = GetNextToken();

			err = expression();
			if (err.er != none)
			{
				return err;
			}

			// Code Generation
			fprintf(outputFile, "push local %i\n", getIndexOf(temp));

			t = GetNextToken();
			if (t.tp == SYMBOL && !strcmp(t.lx, "]"))
				;
			else
			{
				err.er = closeBracketExpected;
				err.tk = t;
				return err;
			}
			// Code Generation
			fprintf(outputFile, "add\n");
			fprintf(outputFile, "pop pointer 1\n");
			fprintf(outputFile, "push that 0\n");
		}
		else if (t.tp == SYMBOL && !strcmp(t.lx, "("))
		{
			t = GetNextToken();

			// if (isArg == 1)
			// {
			// 	commaCount = 1;
			// }
			// else
			// {
			// 	commaCount = 0;
			// }

			err = expressionList();
			if (err.er != none)
			{
				return err;
			}

			t = GetNextToken();
			if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
				;
			else
			{
				err.er = closeParenExpected;
				err.tk = t;
				return err;
			}

			// Code Generation
			fprintf(outputFile, "call %s.%s %i\n", className, methodName, commaCount + isExt);
		}
	}
	else if (t.tp == SYMBOL && !strcmp(t.lx, "("))
	{
		t = GetNextToken();

		err = expression();
		if (err.er != none)
		{
			return err;
		}

		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
			;
		else
		{
			err.er = closeParenExpected;
			err.tk = t;
			return err;
		}
	}
	else
	{
		err.er = syntaxError;
		err.tk = t;
		return err;
	}
	return err;
}

int InitParser(char *file_name)
{
	char vm_path[256];
	char class_name[125];

	strncpy(class_name, file_name, strlen(file_name) - 5);
	sprintf(vm_path, "%s.vm", class_name);
	outputFile = fopen(vm_path, "w");

	ifCount = 0;
	whileCount = 0;

	return 1;
}

ParserInfo Parse()
{
	ParserInfo pi;

	pi = classDeclar();
	// if (pi.er != none)
	// {
	// 	error(ErrorString(pi.er), pi.tk);
	// }

	return pi;
}

int StopParser()
{
	fclose(outputFile);
	return 1;
}
