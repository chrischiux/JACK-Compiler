#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


// you can declare prototypes of parser functions below
void error(const char*, Token);
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

void error(const char* msg, Token t)
{
	printf("Line %i at or near %s: %s\n", t.ln, t.lx, msg);
	exit(1);
}

ParserInfo classDeclar(){
	ParserInfo err;
	err.er = none;

	Token t = GetNextToken();
	if (t.tp == ERR){
		err.er = lexerErr;
		err.tk = t;
		return err;
	}

	if (t.tp == RESWORD && !strcmp(t.lx, "class"))
		;
	else{
		err.er = classExpected;
		err.tk = t;
		return err;
	}
	
	t = GetNextToken();
	if (t.tp == ID)
		;
	else
		{err.er = idExpected;
		err.tk = t;
		return err;
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "{"))
		;
	else
		{err.er = openBraceExpected;
		err.tk = t;
		return err;
	}

	t = PeekNextToken();
	while (t.tp != EOFile && strcmp (t.lx , "}" ))
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
		{err.er = closeBraceExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo memberDeclar(){
	ParserInfo err;
	err.er = none;
	
	Token t = PeekNextToken();
	if (t.tp == RESWORD && (!strcmp(t.lx, "static") || !strcmp(t.lx, "field"))){
		err = classVarDeclar();
		if (err.er != none){
			return err;
		}
	}else if (t.tp == RESWORD && (!strcmp(t.lx, "constructor") || !strcmp(t.lx, "function") || !strcmp(t.lx, "method"))){
		err = subroutineDeclar();
		if (err.er != none){
			return err;
		}
	}else
		{err.er = syntaxError;
		err.tk = t;
		return err;
	} //class member declaration must be var declar or subroutine declar
	return err;
	
}

ParserInfo classVarDeclar(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == RESWORD && (!strcmp(t.lx, "static") || !strcmp(t.lx, "field")))
		;
	else
		{err.er = classVarErr;
		err.tk = t;
		return err;
	}
	
	err = type();
	if (err.er != none){
			return err;
		}
	
	t = GetNextToken();
	if (t.tp == ID)
		;
	else
		{err.er = idExpected;
		err.tk = t;
		return err;
	}
	
	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx , ";"))
	{
		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, ","))
			;
		else
			{err.er = syntaxError;
		err.tk = t;
		return err;
	} //,
		
		t = GetNextToken();
		if (t.tp == ID)
			;
		else
			{err.er = idExpected;
		err.tk = t;
		return err;
	}

		
		t = PeekNextToken();
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";"))
		;
	else
		{err.er = semicolonExpected;
		err.tk = t;
		return err;
	}

	return err;
}

ParserInfo type(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == RESWORD && (!strcmp(t.lx, "int") || !strcmp(t.lx, "char") || !strcmp(t.lx, "boolean")))
		;
	else if (t.tp == ID)
		;
	else
		{err.er = illegalType;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo subroutineDeclar(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == RESWORD && (!strcmp(t.lx, "constructor") || !strcmp(t.lx, "function") || !strcmp(t.lx, "method")))
		;
	else
		{err.er = memberDeclarErr;
		err.tk = t;
		return err;
	}
	
	t = GetNextToken();
	if ((t.tp == RESWORD && (!strcmp(t.lx, "void") || !strcmp(t.lx, "int") || !strcmp(t.lx, "char") || !strcmp(t.lx, "boolean"))) || t.tp == ID)
		;
	else
		{err.er = illegalType;
		err.tk = t;
		return err;
	}
	
	t = GetNextToken();
	if (t.tp == ID)
		;
	else
		{err.er = idExpected;
		err.tk = t;
		return err;
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "("))
		;
	else
		{err.er = openParenExpected;
		err.tk = t;
		return err;
	}
	
	err = paramList();
	if (err.er != none){
			return err;
		}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
		;
	else
		{err.er = closeParenExpected;
		err.tk = t;
		return err;
	}
	
	err = subroutineBody();
	return err;
}

ParserInfo paramList(){
	ParserInfo err;
	err.er = none;
	
	Token t = PeekNextToken();
	if ((t.tp == RESWORD && (!strcmp(t.lx, "int") || !strcmp(t.lx, "char") || !strcmp(t.lx, "boolean"))) || t.tp == ID)
	{
		err = type();
		if (err.er != none){
			return err;
		}
		
		t = GetNextToken();
		if (t.tp == ID)
			;
		else
			{err.er = idExpected;
		err.tk = t;
		return err;
	}
		
		t = PeekNextToken();
		while (t.tp != EOFile && !strcmp(t.lx , ","))
		{
			t = GetNextToken();
			if (t.tp == SYMBOL && !strcmp(t.lx, ","))
				;
			else
				{err.er = syntaxError;
		err.tk = t;
		return err;
	} //,
			
			err = type();
			if (err.er != none){
			return err;
		}
			
			t = GetNextToken();
			if (t.tp == ID)
				;
			else
				{err.er = idExpected;
		err.tk = t;
		return err;
	}
			
			t = PeekNextToken();
		}
	}
	return err;
}

ParserInfo subroutineBody(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "{"))
		;
	else
		{err.er = openBraceExpected;
		err.tk = t;
		return err;
	}
	
	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx , "}"))
	{
		
		
			err = statement();
			if (err.er != none){
			return err;
		}
		t = PeekNextToken();
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}"))
		;
	else
		{err.er = closeBraceExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo statement(){
	ParserInfo err;
	err.er = none;
	
	Token t = PeekNextToken();
	if(t.tp == RESWORD){
		if(!strcmp(t.lx, "var"))
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
			{err.er = syntaxError;
		err.tk = t;
		return err;
			}
		if (err.er != none){
			return err;
		}
	}else
		{err.er = syntaxError;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo varDeclarStatement(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "var"))
		;
	else
		{err.er = syntaxError;
		err.tk = t;
		return err;
	}
	
	err = type();
	if (err.er != none){
			return err;
		}
	
	t = GetNextToken();
	if (t.tp == ID)
		;
	else
		{err.er = idExpected;
		err.tk = t;
		return err;
	}
	
	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx , ";"))
	{
		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, ","))
			;
		else
			{err.er = syntaxError;
		err.tk = t;
		return err;
	} //,
		
		t = GetNextToken();
		if (t.tp == ID)
			;
		else
			{err.er = idExpected;
		err.tk = t;
		return err;
	}
		
		t = PeekNextToken();
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";"))
		;
	else
		{err.er = semicolonExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo letStatement(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "let"))
		;
	else
		{err.er = syntaxError;
		err.tk = t;
		return err;
	}
	
	t = GetNextToken();
	if (t.tp == ID)
		;
	else
		{err.er = idExpected;
		err.tk = t;
		return err;
	}
	
	t = PeekNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "["))
	{
		t = GetNextToken();
		// if (t.tp == SYMBOL && !strcmp(t.lx, "["))
		// 	;
		// else
		// 	error("[ expected", t);
		
		err = expression();
		if (err.er != none){
			return err;
		}
		
		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "]"))
			;
		else
			{err.er = closeBracketExpected;
		err.tk = t;
		return err;
	}
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
		{err.er = equalExpected;
		err.tk = t;
		return err;
	}
	
	err = expression();
	if (err.er != none){
			return err;
		}

	// t = PeekNextToken();
	// if (t.tp == RESWORD || t.tp == ID)
	// 	;
	// else
	// 	error("expression expected", t);
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";"))
		;
	else
		{err.er = semicolonExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo ifStatement(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "if"))
		;
	else
		{err.er = syntaxError;
		err.tk = t;
		return err;
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "("))
		;
	else
		{err.er = openParenExpected;
		err.tk = t;
		return err;
	}
	
	err = expression();
	if (err.er != none){
			return err;
		}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
		;
	else
		{err.er = closeParenExpected;
		err.tk = t;
		return err;
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "{"))
		;
	else
		{err.er = openBraceExpected;
		err.tk = t;
		return err;
	}
	
	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx , "}"))
	{
		t = PeekNextToken();
		if (t.tp == RESWORD){
			err = statement();
			if (err.er != none){
			return err;
		}}
		else
			break;
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}"))
		;
	else
		{err.er = closeBraceExpected;
		err.tk = t;
		return err;
	}
	
	t = PeekNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "else"))
	{
		t = GetNextToken();
		// if (t.tp == RESWORD && !strcmp(t.lx, "else"))
		// 	;
		// else
		// 	error("else expected", t);
		
		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "{"))
			;
		else
			{err.er = openBraceExpected;
		err.tk = t;
		return err;
	}
		
		t = PeekNextToken();
		while (t.tp != EOFile && strcmp(t.lx , "}"))
		{
			t = PeekNextToken();
			if (t.tp == RESWORD){
				err = statement();
				if (err.er != none){
			return err;
		}}
			else
				break;
		}
		
		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "}"))
			;
		else
			{err.er = closeBraceExpected;
		err.tk = t;
		return err;
	}
	}
	return err;
}

ParserInfo whileStatement(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "while"))
		;
	else
		{err.er = syntaxError;
		err.tk = t;
		return err;
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "("))
		;
	else
		{err.er = openParenExpected;
		err.tk = t;
		return err;
	}
	
	err = expression();
	if (err.er != none){
			return err;
		}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
		;
	else
		{err.er = closeParenExpected;
		err.tk = t;
		return err;
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "{"))
		;
	else
		{err.er = openBraceExpected;
		err.tk = t;
		return err;
	}
	
	t = PeekNextToken();
	while (t.tp != EOFile && strcmp(t.lx , "}"))
	{
		if (t.tp == RESWORD){
			err = statement();
			if (err.er != none){
			return err;
		}}
		else
			break;
		t = PeekNextToken();
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "}"))
		;
	else
		{err.er = closeBraceExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo doStatement(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "do"))
		;
	else
		{err.er = syntaxError;
		err.tk = t;
		return err;
	}
	
	err = subroutineCall();
	if (err.er != none){
			return err;
		}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";"))
		;
	else
		{err.er = semicolonExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo subroutineCall(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == ID)
		;
	else
		{err.er = idExpected;
		err.tk = t;
		return err;
	}
	
	t = PeekNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "."))
	{
		t = GetNextToken();
		
		t = GetNextToken();
		if (t.tp == ID)
			;
		else{
			err.er = idExpected;
			err.tk = t;
			return err;
		}
	}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, "("))
		;
	else
		{err.er = openParenExpected;
		err.tk = t;
		return err;
	}

	err = expressionList();

	if (err.er != none){
			return err;
		}
	
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
		;
	else
		{err.er = closeParenExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo expressionList(){
	ParserInfo err;
	err.er = none;
	
	Token t = PeekNextToken();
	if (t.tp == RESWORD || t.tp == ID || (t.tp == SYMBOL && strcmp(t.lx, ")")) || t.tp == STRING || t.tp == INT) // NEED TO CHECK
	{
		err = expression();
		if (err.er != none){
			return err;
		}
		
		t = PeekNextToken();
		while (t.tp != EOFile && !strcmp(t.lx , ","))
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
			if (err.er != none){
			return err;
		}
			
			t = PeekNextToken();
		}
	}
	return err;
}

ParserInfo returnStatement(){
	ParserInfo err;
	err.er = none;
	
	Token t = GetNextToken();
	if (t.tp == RESWORD && !strcmp(t.lx, "return"))
		;
	else{
		err.er = syntaxError;
		err.tk = t;
		return err;
	}
	
	t = PeekNextToken();
	if (t.tp == RESWORD || t.tp == ID || (t.tp == SYMBOL && strcmp(t.lx, ";"))|| t.tp == STRING || t.tp == INT){ // NEED TO CHECK
		err = expression();
		// if (err.er != none){
		// 	return err;
		// }
	}
	t = GetNextToken();
	if (t.tp == SYMBOL && !strcmp(t.lx, ";"))
		;
	else
		{err.er = semicolonExpected;
		err.tk = t;
		return err;
	}
	return err;
}

ParserInfo expression(){
	ParserInfo err;
	err.er = none;
	
	err = relationalExpression();
	if (err.er != none){
			return err;
		}
	
	Token t = PeekNextToken();
	while(t.tp != EOFile && (!strcmp(t.lx, "&") || !strcmp(t.lx, "|"))){
		t = GetNextToken();
		// if (t.tp == SYMBOL && (!strcmp(t.lx, "&") || !strcmp(t.lx, "|")))
		// 	;
		// else
		// 	error("operator expected", t);
		
		err = relationalExpression();
		if (err.er != none){
			return err;
		}
		
		t = PeekNextToken();
	}
	return err;
}

ParserInfo relationalExpression(){
	ParserInfo err;
	err.er = none;
	
	err = ArithmeticExpression();
	if (err.er != none){
			return err;
		}
	
	Token t = PeekNextToken();
	if (t.tp == SYMBOL && (!strcmp(t.lx, "<") || !strcmp(t.lx, ">") || !strcmp(t.lx, "=")))
	{
		t = GetNextToken();
		// if (t.tp == SYMBOL && (!strcmp(t.lx, "<") || !strcmp(t.lx, ">") || !strcmp(t.lx, "=")))
		// 	;
		// else
		// 	error("operator expected", t);
		
		err = ArithmeticExpression();
		if (err.er != none){
			return err;
		}
	}
	return err;
}

ParserInfo ArithmeticExpression(){
	ParserInfo err;
	err.er = none;
	
	err = term();
	if (err.er != none){
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
		if (err.er != none){
			return err;
		}
		
		t = PeekNextToken();
	}
	return err;
}

ParserInfo term(){
	ParserInfo err;
	err.er = none;
	
	err = factor();
	if (err.er != none){
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
		if (err.er != none){
			return err;
		}
		
		t = PeekNextToken();
	}
	return err;
}

ParserInfo factor(){
	ParserInfo err;
	err.er = none;
	
	Token t = PeekNextToken();
	if (t.tp == SYMBOL && (!strcmp(t.lx, "-") || !strcmp(t.lx, "~")))
	{
		t = GetNextToken();
		// if (t.tp == SYMBOL && (!strcmp(t.lx, "-") || !strcmp(t.lx, "~")))
		// 	;
		// else
		// 	error("unary operator expected", t);
	}
	err = operand();
	return err;
}

ParserInfo operand(){
	ParserInfo err;
	err.er = none;
	
	Token t = PeekNextToken();
	if (t.tp == INT)
	{
		t = GetNextToken();
	}
	else if (t.tp == STRING)
	{
		t = GetNextToken();
	}
	else if (t.tp == RESWORD)
	{
		t = GetNextToken();
		if (t.tp == RESWORD && (!strcmp(t.lx, "true") || !strcmp(t.lx, "false") || !strcmp(t.lx, "null") || !strcmp(t.lx, "this")))
			;
		else
			{err.er = syntaxError;
			err.tk = t;
			return err;
		} //keyword
	}
	else if (t.tp == ID)
	{
		t = GetNextToken();
		// if (t.tp == ID)
		// 	;
		// else
		// 	{err.er = idExpected;
		// err.tk = t;
		// return err;
		// }
		
		t = PeekNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "."))
		{
			t = GetNextToken();
			
			t = GetNextToken();
			if (t.tp == ID)
				;
			else
				{err.er = idExpected;
				err.tk = t;
				return err;
				}
		}

		t = PeekNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, "["))
		{
			t = GetNextToken();
			// if (t.tp == SYMBOL && !strcmp(t.lx, "["))
			// 	;
			// else
			// 	error("[ expected", t);
			
			err = expression();
			if (err.er != none){
			return err;
			}
			
			t = GetNextToken();
			if (t.tp == SYMBOL && !strcmp(t.lx, "]"))
				;
			else
				{err.er = closeBracketExpected;
				err.tk = t;
				return err;
			}
		}else if (t.tp == SYMBOL && !strcmp(t.lx, "("))
		{
			t = GetNextToken();
			
			err = expressionList();
			if (err.er != none){
			return err;
		}
			
			t = GetNextToken();
			if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
				;
			else
				{err.er = closeParenExpected;
		err.tk = t;
		return err;
	}
		}
	}
	else if (t.tp == SYMBOL && !strcmp(t.lx, "("))
	{
		t = GetNextToken();
		
		err = expression();
		if (err.er != none){
			return err;
		}
		
		t = GetNextToken();
		if (t.tp == SYMBOL && !strcmp(t.lx, ")"))
			;
		else{
			err.er = closeParenExpected;
			err.tk = t;
			return err;
		}
	}else{
		err.er = syntaxError;
		err.tk = t;
		return err;
	}
	return err;
}

int InitParser (char* file_name)
{

	return 1;
}

ParserInfo Parse ()
{
	ParserInfo pi;


	pi = classDeclar();
	// if (pi.er != none)
	// {
	// 	error(ErrorString(pi.er), pi.tk);
	// }
	

	return pi;
}


int StopParser ()
{
	return 1;
}




#ifndef TEST_PARSER
int main ()
{

	return 1;
}
#endif
