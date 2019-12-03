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
#define FALSE 1
#define IF 2   /*IF keyword*/
#define PLATYPUS 3 /*PLATYPUS from the keyword table*/
#define READ 4 /*READ KEYWORD*/
#define REPEAT 5 /*REPEAT keyword*/
#define THEN 6 /*then keyword*/
#define TRUE 7 /* true keyword */
#define WHILE 8 /*while keyword*/
#define WRITE 9 /*wrtie keyword*/


/*function declaration*/
void parser();
void match(int, int);
void syn_eh(int);
void syn_printe();
void gen_incode(char*);
void program();
void opt_statements();

/*procution rule (Grammar) Functions*/
void opt_statements(); /*3.1 FIRST(<opt_statements>) = {Ɛ, AVID_T, SVID_T, KW_T(IF), KW_T(USING), KW_T(READ), KW_T(WRITE)}*/
void statements(); /*3.1 FIRST(<statements>) = { AVID_T, SVID_T, KW_T(IF), KW_T(USING), KW_T(READ), KW_T(WRITE) }*/
void statements_prime(); /*3.1 FIRST(<statements’>) = {Ɛ, AVID_T, SVID_T, KW_T(IF), KW_T(USING), KW_T(READ), KW_T(WRITE)}*/
void statement(); /*3.2 FIRST(<statement>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), */
void assign_statement(); /*3.2.1FIRST(<assignment statement>) = { FIRST(<assignment expression>) } = { AVID_T, SVID_T }*/
void assign_expression(); /*3.2.1 FIRST(<assignment expression>) = { AVID_T, SVID_T } */
void selection_statement(); /*3.2.1 FIRST(<selection statement>) = {KW_T(IF)}*/
void iteration_statement(); /*3.2.3 FIRST(<iteration statement>) = { KW_T(WHILE) }*/
void input_statement(); /*3.2.4 FIRST(<iteration statement>) = { KW_T(WHILE) } */
void variable_list(); /*3.2.4 FIRST(<variables list>) = { FIRST(<variable identifier>) } = {AVID_T,SVID_T}*/
void variable_list_prime(); /*3.2.4 FIRST(<variable list’>) = {COM_T, AVID_T,SVID_T,Ɛ) */
void variable_identifier(); /*3.2.4 FIRST(<variable identifier>) = {AVID_T,SVID_T} */
void output_statement(); /*3.2.5 FIRST(<output statement>) = { KW_T(WRITE) }*/
void output_list(); /*3.2.5 FIRST(<output list>)  = {FIRST(<variable list>)} = {Ɛ, AVID_T, SVID_T, STR_T}*/
void arithmetic_expression();/*3.3.1 FIRST(<arithmetic expression>) = { FIRST(<unary arithmetic expression>,FIRST(<additive arithmetic expression>)} = { -, +, AVID_T, FPL_T, INL_T, ( }*/
void unary_arithmetic_expression();
void additive_arithmetic_expression(); /*3.3.1 FIRST(<additive arithmetic expression>) ={FIRST(<multiplicative arithmetic expression>)} = { AVID_T, FPL_T, INL_T, LPR_T }*/
void additive_arthmetic_expression_prime(); /*FIRST(<additive arithmetic expression’>) = { Ɛ, ART_OP_T(-), ART_OP_T(+)}*/
void multiplicative_arithmetic_expression();/*FIRST(<multiplicative arithmetic expression>) = { FIRST(<primary arithmetic  expression>)} = {Ɛ, ART_OP_T(DIV), ART_OP_T(MULT)}*/
void multiplicative_arithmetic_expression_prime(); /*3.3.1 FIRST(<multiplicative arithmetic expression’>) = {Ɛ, ART_OP_T(/), ART_OP_T(*)}*/
void primary_arithmetic_expression(); /*3.3.1 FIRST(<primary arithmetic expression>) = { AVID_T, FPL_T, INL_T, LPR_T }*/
void string_expression();/*3.3.2 FIRST(<string expression>) ={FIRST(<primary string expression>)} = { SVID_T, STR_T }*/
void string_expression_prime();/*3.3.2 FIRST(<string expression’>) = { Ɛ , SCC_OP_T}*/
void primary_string_expression(); /*3.3.2 FIRST(<primary string expression>) = { SVID_T, STR_T }*/
void conditional_expression(); /*3.3.3 FIRST(<conditional expression>) ={FIRST(<logical OR expression>)} = { STR_T, SVID_T, AVID_T, FPL_T, INL_T }*/
void logical_OR_expression();/*3.3.3 FIRST(<logical OR expression>) = {FIRST(<logical AND expression)} = { STR_T, SVID_T, AVID_T, FPL_T, INL_T }*/
void logical_OR_expression_prime(); /*3.3.3 FIRST(<logical OR expression’>) = { Ɛ, LOG_OP_T(OR)}*/
void logical_AND_expression();/*3.3.3 FIRST(<logical AND expression>) ={FIRST(<relational expression>)} = { STR_T, SVID_T, AVID_T, FPL_T, INL_T }*/
void logical_AND_expression_prime(); /*3.3.3 FIRST(<logical AND expression’>) = {Ɛ, LOG_OP_T(AND)}*/
void relational_expression(); /*3.3.4 FIRST(<relational expression>) = { FIRST(<primary a_relational expression>),FIRST(primary s_relational expression>) } = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }*/
void primary_a_relational_expression(); /*3.3.4 FIRST(<primary a_relational expression>) = { AVID_T, FPL_T, INL_T }*/
void primary_a_relational_expression_prime(); /*3.3.4 FIRST(<primary a_relational expression’>) = { REL_OP_T(EQ), REL_OP_T(NE), REL_OP_T(GT), REL_OP_T(LT) }*/
void primary_s_relational_expression(); /*3.3.4 FIRST(<primary s_relational expression’>) = { STR_T, SVID_T }*/
void primary_s_relational_expression_prime(); 
void pre_condition();
#endif