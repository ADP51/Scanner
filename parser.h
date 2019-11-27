#ifndef PARSER_H_
#define PARSER_H_

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef TOKEN_H_
#include "token.h"
#endif



/*global static variables*/
static Token lookahead;/*looking for the next token*/
static Buffer* sc_buf; /*scanner buffer*/
int synerrno; /*counter*/

/*external Links to table.h*/
extern char* kw_table[]; /*keyword table*/
extern Buffer* str_LTBL; /* SL buffer */
extern int line; /*line number from the source code file (external link to scanner.c)*/
extern Token malar_next_token();

/*Keyword constants Note: values match the table index*/
#define NO_ATTR -1/*Token with no attribute*/
#define ELSE 0  /*ELSE KEYWORD*/
#define IF 1   /*IF keyword*/
#define PLATYPUS 2 /*PLATYPUS from the keyword table*/
#define READ 3 /*READ KEYWORD*/
#define REPEAT 4 /*REPEAT keyword*/
#define THEN 5 /*then keyword*/
#define TRUE 6 /*true keyword*/
#define WHILE 7 /*while keyword*/
#define WRTIE 8 /*wrtie keyword*/


/*function declaration*/
void parser();
void match(int, int);
void syn_eh(int);
void syn_printe();
void gen_incode(char*);
void program();
void opt_statements();

#endif