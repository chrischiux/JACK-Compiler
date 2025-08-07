/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: Chris Chiu
Student ID: 201594017
Email: sc22phc@leeds.ac.uk
Date Work Commenced: 10 Feb 2024
*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

struct node{
  Token token;
  struct node *next;
};

struct node *head;
struct node *first;
struct node *tail;

FILE* file;

int skipFirstNode(){
  // skip first empty node
  if (head->next != NULL) {
    head = head->next;
  }
}

int addEofNode(){
  struct node* new_node = malloc(sizeof(struct node));
  new_node->next = NULL;
  new_node->token.tp = EOFile;
  strcpy(new_node->token.lx, "End of File");
  tail->next = new_node;
  tail = new_node;
}


// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name){
  // Initialise linked list
  head = malloc(sizeof(struct node));
  first = head;
  tail = head;
  head->next = NULL;

  //variables
  char temp[128];
  int counter = 0;
  int str_len = 0;
  int line_num = 1;

  // flags
  int is_comment = 0;

  // open the file
  file = fopen(file_name, "r");
  
  // check if file opend successfully
  if (file == NULL) {
    return 0;
  }

  // loop through file
  char c;
  while ((c = fgetc(file)) != EOF) {

    // white space check
    if (c == ' ' || c == '\t' || c == '\r') {
      continue;
    }else if (c == '\n') {
      line_num++;
      continue;
    }

    // comment check
    if (c == '/') {

      // comment to end of line
      if ((c = fgetc(file)) == '/') {

        // increment line number
        line_num++;

        // skip to the end of the line
        while ((c = fgetc(file)) != '\n') {
          continue;
        }
        continue;
      }

      // comment until closing tag
      else if (c == '*') {
        is_comment = 1;

        // parse until end of comment
        while (is_comment) {
          c = fgetc(file);
          if (c == '\n') {
            line_num++;
          }else if (c == '*') {

            if ((c = fgetc(file)) == '/') {
              is_comment = 0;
            }

          }else if (c == EOF) {
            // create new node
            struct node* new_node = malloc(sizeof(struct node));
            new_node->token.tp = ERR;
            strcpy(new_node->token.lx, "Error: unexpected eof in comment");
            new_node->token.ec = EofInCom;
            new_node->token.ln = line_num;
            strcpy(new_node->token.fl, file_name);
            new_node->next = NULL;
            
            //add to linked list
            tail->next = new_node;
            tail = new_node;

            skipFirstNode();

            addEofNode();

            return 1;
          }
        }
        continue;

      }else if (c != EOF){
        fseek(file, -1, SEEK_CUR);
        c = '/';
      }
    }

    
    
    // string check
    if (c == '"') {

      str_len = 0;

      // create new node
      struct node* new_node = malloc(sizeof(struct node));
      new_node->next = NULL;

      // check if malloc was successful
      if (new_node == NULL) {
        printf("Malloc failed\n");
        return 0;
      }

      // append to linked list
      tail->next = new_node;
      tail = new_node;

      // parse the string
      while ((c = fgetc(file)) != '"') {

        // check EOF
        if (c == EOF || c == '\n') {
          break;
        }

        // no error, so add to temp
        temp[str_len++] = c;
      }

      // EOF error
      if (c == EOF) {
        
        // populate the new token
        new_node->token.tp = ERR;
        strcpy(new_node->token.lx, "Error: unexpected eof in string constant");
        new_node->token.ec = EofInStr;
        new_node->token.ln = line_num;
        strcpy(new_node->token.fl, file_name);

        skipFirstNode();

        addEofNode();

        return 1;

      // new line error
      }else if (c == '\n') {
        
        // populate the new token
        new_node->token.tp = ERR;
        strcpy(new_node->token.lx, "Error: new line in string constant");
        new_node->token.ec = NewLnInStr;
        new_node->token.ln = line_num;
        strcpy(new_node->token.fl, file_name);

        skipFirstNode();

        addEofNode();

        return 1;

      // no error
      }else{

        // add null terminator
        temp[str_len] = '\0';

        // populate the new token
        new_node->token.tp = STRING;
        strcpy(new_node->token.lx, temp);
        new_node->token.ec = 0;
        new_node->token.ln = line_num;
        strcpy(new_node->token.fl, file_name);
      }

      continue;
    }

    // letter check
    if (isalpha(c) || c == '_') {

      str_len = 0;

      temp[str_len++] = c;

      // parse the word
      while (isalpha(c = fgetc(file)) || isdigit(c) || c == '_') {
        temp[str_len++] = c;
      }

      // backtrack to the last character
      if (c != EOF){
        fseek(file, -1, SEEK_CUR);
      }

      // add null terminator
      temp[str_len] = '\0';

      // create new node
      struct node* new_node = malloc(sizeof(struct node));
      new_node->next = NULL;

      // check if malloc was successful
      if (new_node == NULL) {
        printf("Malloc failed\n");
        return 0;
      }

      // append to linked list
      tail->next = new_node;
      tail = new_node;

      //determine token type
      if (strcmp(temp, "class") == 0 || strcmp(temp, "constructor") == 0 || strcmp(temp, "function") == 0 || strcmp(temp, "method") == 0 || 
      strcmp(temp, "field") == 0 || strcmp(temp, "static") == 0 || strcmp(temp, "var") == 0 || 
      strcmp(temp, "int") == 0 || strcmp(temp, "char") == 0 || strcmp(temp, "boolean") == 0 || strcmp(temp, "void") == 0 || 
      strcmp(temp, "true") == 0 || strcmp(temp, "false") == 0 || strcmp(temp, "null") == 0 || 
      strcmp(temp, "this") == 0 ||
      strcmp(temp, "let") == 0 || strcmp(temp, "do") == 0 || strcmp(temp, "if") == 0 || strcmp(temp, "else") == 0 || strcmp(temp, "while") == 0 || strcmp(temp, "return") == 0) {
        new_node->token.tp = RESWORD;
      }else{
        new_node->token.tp = ID;
      }

      // populate the new token
      strcpy(new_node->token.lx, temp);
      new_node->token.ec = 0;
      new_node->token.ln = line_num;
      strcpy(new_node->token.fl, file_name);

      continue;
    }

    // digit check
    else if (isdigit(c)) {

      str_len = 0;

      temp[str_len++] = c;

      // parse the number
      while (isdigit(c = fgetc(file))) {
        temp[str_len++] = c;
      }

      // backtrack to the last character
      if (c != EOF){
        fseek(file, -1, SEEK_CUR);
      }

      // add null terminator
      temp[str_len] = '\0';

      // create new node
      struct node* new_node = malloc(sizeof(struct node));
      new_node->next = NULL;

      // check if malloc was successful
      if (new_node == NULL) {
        printf("Malloc failed\n");
        return 0;
      }

      // append to linked list
      tail->next = new_node;
      tail = new_node;

      // populate the new token
      new_node->token.tp = INT;
      strcpy(new_node->token.lx, temp);
      new_node->token.ec = 0;
      new_node->token.ln = line_num;
      strcpy(new_node->token.fl, file_name);

      continue;
    }

    // must be symbol
    else{

      // create new node
      struct node* new_node = malloc(sizeof(struct node));
      new_node->next = NULL;

      // check if malloc was successful
      if (new_node == NULL) {
        printf("Malloc failed\n");
        return 0;
      }

      // append to linked list
      tail->next = new_node;
      tail = new_node;

      

      //(c != '{' && c != '}' && c != '(' && c != ')' && c != '[' && c != ']' && c != '.' && c != ',' && c != ';' && c != '+' && c != '-' && c != '*' && c != '/' && c != '&' && c != '|' && c != '<' && c != '>' && c != '=' && c != '~')
      if (c == '?') {
        new_node->token.tp = ERR;
        new_node->token.ec = IllSym;
        new_node->token.ln = line_num;
        strcpy(new_node->token.fl, file_name);
        strcpy(new_node->token.lx, "Error: illegal symbol in source file");
        continue;
        
      }else{
        // populate the new token
        new_node->token.tp = SYMBOL;
        new_node->token.lx[0] = c;
        new_node->token.lx[1] = '\0';
        new_node->token.ec = 0;
        new_node->token.ln = line_num;
        strcpy(new_node->token.fl, file_name);
      }

    }

  }
  

  // add EOF node
  addEofNode();

  // skip first empty node
  skipFirstNode();

  return 1;
}


// Get the next token from the source file
Token GetNextToken ()
{
  Token t = head->token;
  head = head->next;

  return t;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken ()
{
  Token t = head->token;

  return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
  // free linked list
  struct node* temp;
  while (first != NULL) {
    temp = first;
    first = first->next;
    free(temp);
  }

  // close file
  fclose(file);

	return 0;
}

// do not remove the next line
#ifndef TEST
int main ()
{
	// implement your main function here
  // NOTE: the autograder will not use your main function
  if (InitLexer("NewLineInStr.jack") == 0){
    printf("Failed to open file\n");
    return 0;
  }
  // Token t = GetNextToken();
  // while (t.tp != EOFile) {
  //   printf("<%s, %i, %s, %i>\n", t.fl, t.ln, t.lx, t.tp);
  //   t = GetNextToken();
  // }
  
	return 0;
}
// do not remove the next line
#endif
