/************************************************************************************************
							FILE HEADER
 File Name: parser.c
Compiler : MS Visual Studio 2019
Author: Johnathon Cameron and Andrew Palmer
Course: Compilers CST8152_012:
Assignment #: 3 - Parser
Date: TODO
Professor: Sv. Ranev
Description: Implementation of a Parser for the platypus language compiler 
Function List: TODO
**************************************************************************************************************/
#include <stdlib.h>
#include "parser.h"

/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: parse
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:malar_next_token(),program(),match(),gen_incode()
Parameters: None
Return:  None
Algorithm:
	Function Advances to the next input token, then calls the program function to start parsing the Platypus program  source file.
	It then calls the match function to verify that the source end of file was found and then prints our letting the user know 
	that the source file parsed successfully by calling the gen_incode("PLATY: Source File parsed")
****************************************************************************************************************/
void parser() {
	/*advances to first input token*/
	lookahead = malar_next_token();
	/*program production rule function*/
	program(); 
	/*matching tokens to source end of file*/
	match(SEOF_T, NO_ATTR);
	/*print out successful parse*/
	gen_incode("PLATY: Source file parsed");
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: match
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function: syn_eh(),malar_next_token(),syn_printe()
Parameters:     Name:               Type:           Range:
				pr_token_code	    int             Any token code provided by the Scanner
				pr_token_attribute  int             Any token attribute provided by the Scanner
Return: None
Algorithm:Checks : 
	Function matches two token, the current input token (lookahead) and the token 
	required by the parser. NOTE: The attribute code is used only when the token code is one of
	the following codes: KW_T, LOG_OP_T, ART_OP_T, REL_OP_T. In all other cases
	the token code is matched only
****************************************************************************************************************/
void match(int pr_token_code, int pr_token_attribute) {

	/*if lookahead is equal to source end of file*/
	if (lookahead.code == SEOF_T) return;

	/*switch case to verify attribute match with specific code*/
	switch (pr_token_code) {

	case KW_T: 
		/*May have to do some special handling depending on the production rules TODO*/

		/*checking if the token attribute is equivalent to the keyword table index*/
		if (pr_token_attribute != lookahead.attribute.kwt_idx) {
			/*error handler function call if it isn't*/
			syn_eh(pr_token_code);
			return;
		}
		break;

	case LOG_OP_T:
	case ART_OP_T:
	case REL_OP_T:
		/*if attribute is not equal to lookahead attribute call error handler function*/
		if (pr_token_attribute != lookahead.attribute.get_int) {
			syn_eh(pr_token_code);
			return;
		}
		break;

	/*break if pr_token_code isn't part of the attribute check*/
	default: break;
	}

	/**********************
		no SEOF match
	***********************/

	/*advance to next token*/
	lookahead = malar_next_token();

	/*check if new token is error token*/
	if (lookahead.code == ERR_T) {
		/*error printing function*/
		syn_printe();
		/*look ahead to next token again*/
		lookahead = malar_next_token();
		/*increment error counter*/
		synerrno++;
		return;
	}
	/*if match is unsuccessful*/
	if (lookahead.code != pr_token_code) {
		/*error handling function*/
		syn_eh(pr_token_code);
		return;
	}
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: syn_eh
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:syn_printe(),exit(),malar_next_token()
Parameters:     Name:               Type:           Range:
				sync_token_code		int		        Any token code provided by the Scanner
Return: none
Algorithm:Checks : Error Handler function that goes into panic mode recovery, looking for the matchin token code.
if the code is not match before source end of file it exit is called. If the token is equal to the input token to 
the required parser token, it advances one more time and returns;
****************************************************************************************************************/
void syn_eh(int sync_token_code) {
	/*calling error printing function*/
	syn_printe();
	/*increment error counter*/
	synerrno++;

	/*Panic mode error recovery*/
	while (lookahead.code != sync_token_code) {

		/*looking for SEOF before every move*/
		if (lookahead.code == SEOF_T) {
			exit(synerrno);
			return;
		}
		/*advancing to next input token*/
		lookahead = malar_next_token();

		/*matching token is found, advances the input token one more time before returning*/
		if (lookahead.code == sync_token_code) {
			lookahead = malar_next_token();
			return;
		}
	}
}

/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: syn_orinted
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:b_mark(),printf()
Parameters: none
Return: none
Algorithm : Error printing function
			Example:
					PLATY: Syntax error: Line: 2
					***** Token code: 13 Attribute: NA
					PLATY: Syntax error: Line: 8
					***** Token code: 9 Attribute: 0
					PLATY: Syntax error: Line: 9
					***** Token code: 2 Attribute: sum
					PLATY: Syntax error: Line: 11
					***** Token code: 4 Attribute: 0.5
					PLATY: Syntax error: Line: 17
					***** Token code: 6 Attribute: Result:
					PLATY: Syntax error: Line: 21
					***** Token code: 16 Attribute: ELSE

****************************************************************************************************************/
void syn_printe() {
	Token t = lookahead;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
	case  ERR_T: /* ERR_T     0   Error token */
		printf("%s\n", t.attribute.err_lex);
		break;
	case  SEOF_T: /*SEOF_T    1   Source end-of-file token */
		printf("SEOF_T\t\t%d\t\n", t.attribute.seof);
		break;
	case  AVID_T: /* AVID_T    2   Arithmetic Variable identifier token */
	case  SVID_T:/* SVID_T    3  String Variable identifier token */
		printf("%s\n", t.attribute.vid_lex);
		break;
	case  FPL_T: /* FPL_T     4  Floating point literal token */
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T: /* INL_T      5   Integer literal token */
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:/* STR_T     6   String literal token */
		b_mark(str_LTBL, t.attribute.str_offset);
		printf("%s\n", b_location(str_LTBL));
		break;

	case SCC_OP_T: /* 7   String concatenation operator token */
		printf("NA\n");
		break;

	case  ASS_OP_T:/* ASS_OP_T  8   Assignment operator token */
		printf("NA\n");
		break;
	case  ART_OP_T:/* ART_OP_T  9   Arithmetic operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T: /*REL_OP_T  10   Relational operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:/*LOG_OP_T 11  Logical operator token */
		printf("%d\n", t.attribute.get_int);
		break;

	case  LPR_T: /*LPR_T    12  Left parenthesis token */
		printf("NA\n");
		break;
	case  RPR_T: /*RPR_T    13  Right parenthesis token */
		printf("NA\n");
		break;
	case LBR_T: /*    14   Left brace token */
		printf("NA\n");
		break;
	case RBR_T: /*    15  Right brace token */
		printf("NA\n");
		break;

	case KW_T: /*     16   Keyword token */
		printf("%s\n", kw_table[t.attribute.get_int]);
		break;

	case COM_T: /* 17   Comma token */
		printf("NA\n");
		break;
	case EOS_T: /*    18  End of statement *(semi - colon) */
		printf("NA\n");
		break;
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}/*end switch*/
}/* end syn_printe()*/
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: gen_incode
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function: printf()
Parameters:     Name:               Type:           Range:
				str                 char*           Any input parameter string whenever called upon
Return: none
Algorithm: prints param input string
****************************************************************************************************************/
void gen_incode(char* str) {
	
	/*printing string parameter*/
	printf("%s\n", str);
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: program
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(), opt_statements(),gen_incode()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from the <program> non terminal using the principle
of FIRST set.

GRAMMAR-> 3.1 --------->FIRST(<program>) = {KW_T(PLATYPUS)}
****************************************************************************************************************/
void program() {
		match(KW_T, PLATYPUS); match(LBR_T, NO_ATTR); opt_statements();
		match(RBR_T, NO_ATTR);
		gen_incode("PLATY: Program parsed");
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: opt_statements
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:statements(),statements_prime()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <opt_statements>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.1 ------->FIRST(<opt_statements>) = {Ɛ, AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE)}
****************************************************************************************************************/
void opt_statements() {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T: statements(); break;
	case KW_T:
		/* check for IF,WHILE,READ,WRITE and in statements_p()*/
		if (lookahead.attribute.get_int == IF
			|| lookahead.attribute.get_int == WHILE
			|| lookahead.attribute.get_int == READ
			|| lookahead.attribute.get_int == WRITE) {
			statements();
			break;
		}
	default: /*empty string possible*/;
		gen_incode("PLATY: Opt_statements parsed");
	}
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: statements
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:statement(),statements_prime()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <statements>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.1 ------->FIRST(<statements>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE) }
****************************************************************************************************************/
void statements() {
	statement();
	statements_prime();
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: statements_prime
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <statements'>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.1 ------->FIRST(<statements’>) = {Ɛ, AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ), KW_T(WRITE)}
****************************************************************************************************************/
void statements_prime() {

	switch (lookahead.code) {
	case AVID_T:/*Arithmetic variable identifier*/
	case SVID_T:/*String variable identifier*/
		statement();/*call statement FIRST set string/arr variable identifier*/
		statements_prime();/*recursive call*/
	case KW_T:
		/*check for if while read write*/
		if (lookahead.attribute.get_int == IF
			|| lookahead.attribute.get_int == WHILE
			|| lookahead.attribute.get_int == READ
			|| lookahead.attribute.get_int == WRITE) {
			statements();
			break;
		}
	default: /* empty string possible*/;
		gen_incode("PLATY: Opt_statements parsed");
	}
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: statement
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:assign_expression,gen_incode(),selection_statement(),input_statement(),iteration_statement(),output_statement(),syn_printe()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <statement>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.1 ------->FIRST(<statement>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(READ),KW_T(WRITE)}
****************************************************************************************************************/
void statement() {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		assign_expression();
		break;
	case KW_T:
		if (lookahead.attribute.kwt_idx == IF) {
			selection_statement();
		}
		else if (lookahead.attribute.kwt_idx == WHILE) {
			iteration_statement();
		}
		else if (lookahead.attribute.kwt_idx == READ) {
			input_statement();
		}
		else if (lookahead.attribute.kwt_idx == WRITE) {
			output_statement();
		}
		else {
			syn_printe();
		}
		break;
	default:
		syn_printe();
	}
}

/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: assign_statement
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(), assign_expression,gen_incode()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <assignment statement>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.2.1 ------->FIRST(<assignment statement>) = { FIRST(<assignment expression>) }
													   = { AVID_T, SVID_T }
****************************************************************************************************************/
void assign_statement() {
	assign_expression();
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Assignment statement parsed\n");
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: assign_expression
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(), arithmetic_expression(),string_expression(),syn_printe(),gen_incode()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <assignment expression>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.2.1 -------> FIRST(<assignment expression>) = { AVID_T, SVID_T }
****************************************************************************************************************/
void assign_expression() {
	switch (lookahead.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR);
		match(ASS_OP_T, EQ);
		arithmetic_expression();
		gen_incode("PLATY: Assignment expression 'arithmetic' parsed");
		break;
	case SVID_T:
		match(SVID_T, NO_ATTR);
		match(ASS_OP_T, EQ);
		string_expression();
		gen_incode("PLATY: Assignment expression 'string' parsed");
		break;
	default:
		syn_printe();
		return;
	}
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: selection_statement
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(),assign_expression(),conditional_expression(),gen_incode(),opt_statements()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <selection_statement>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.2.2 -------> FIRST(<selection statement>) = {KW_T(IF)}
****************************************************************************************************************/
void selection_statement(){
	match(KW_T, IF);
	match(KW_T, TRUE);
	match(LPR_T, NO_ATTR);
	conditional_expression();
	match(RPR_T, NO_ATTR);
	match(KW_T, THEN);
	opt_statements();
	match(KW_T, ELSE);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: IF statement parsed");
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: iteration_statement
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(),assign_expression(),conditional_expression(),gen_incode(),opt_statements()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <iteration_statement>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.2.3 -------> FIRST(<iteration statement>) = { KW_T(WHILE) }
****************************************************************************************************************/
void iteration_statement(){
	match(KW_T, WHILE);
	match(LPR_T, NO_ATTR);
	assign_expression();
	match(COM_T, NO_ATTR);
	conditional_expression();
	match(COM_T, NO_ATTR);
	assign_expression();
	match(RBR_T, NO_ATTR);
	match(KW_T, REPEAT);
	match(LPR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: WHILE statement parsed");
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: input_statement
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(),variable_identifier(),variable_list(),gen_incode()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <input_statement>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.2.4 ------->FIRST(<input statement>) = { KW_T(READ) }}
****************************************************************************************************************/
void input_statement(void) {
	
	/*Syntax READ();*/
	/*needs to be READ*/
	match(KW_T, READ);
	/*needs ( symbol*/
	match(LPR_T, NO_ATTR);
	/*variable list, can be empty or have any variable identifier*/
	variable_list();
	/*needs ) symbol*/
	match(RPR_T, NO_ATTR);
	/*needs end of statement ;*/
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Input statement parsed");
}


/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: variable_list
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(),variable_identifier(),variable_list_p()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <variable list>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.2.4 ------->FIRST(<variables list>) = { FIRST(<variable identifier>) }
											    = {AVID_T,SVID_T}
****************************************************************************************************************/
void variable_list() {
	/*need a variable identifier*/
	variable_identifier();
	/*check for comma, identifiers*/
	variable_list_prime();
	gen_incode("PLATY: Variable list parsed");

}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: variable_list_prime
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(),variable_identifier(),variable_list_p()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <variable list'>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.2.4 ------->FIRST(<variable list’>) = {COM_T, AVID_T,SVID_T,Ɛ)
****************************************************************************************************************/
void variable_list_prime() {
	/*if comma token is found*/
	if (lookahead.code == COM_T) {
		/*make sure they match*/
		match(COM_T, NO_ATTR);
		/*recursion*/
		variable_identifier();
		variable_list_p();
	}
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: variable_identifier
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(),syn_printe()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <variable identifier>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.2.4 ------->FIRST(<variable identifier>) = {AVID_T,SVID_T} 
****************************************************************************************************************/
void variable_identifier() {
	/*Check in Specs if you can to WRITE(); empty*/
	switch (lookahead.code) {
	/*String variable identifier*/
	case SVID_T:
		match(SVID_T, NO_ATTR);
		break;
	/*Arithmetic Variable identifier*/
	case AVID_T:
		match(AVID_T, NO_ATTR);
		break;
    /*if its none of the above token, print error*/
	default : 
		syn_printe();
		break;
	}
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: output_statement
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(), output_list(),gen_incode()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <output statement>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.2.5 ------->FIRST(<output statement>) = { KW_T(WRITE) }
****************************************************************************************************************/
void output_statement() {
	
	/*Syntax WRITE();*/
	/*WRITE needs to match*/
	match(KW_T, WRITE);
	/*( needs to match*/
	match(LPR_T, NO_ATTR);
	/*output list*/
	output_list();
	/*needs to match with ) */
	match(RPR_T, NO_ATTR);
	/*needs to match with end of statements ;*/
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Output statement parsed");


}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: output_list
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(), variable_list(),gen_incode()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <output list>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.2.5------->	FIRST(<output list>)  = {FIRST(<variable list>)}
											  = {Ɛ, AVID_T, SVID_T, STR_T}
****************************************************************************************************************/
void output_list() {
	/*trying to match tokens with production (FIRST)*/
	switch (lookahead.code) {

	case AVID_T:
		/*looking for the variable value*/
		variable_list();
		break;
	case SVID_T:
		/*looking for the variable*/
		variable_list();
		break;

	case STR_T:
		/*String literal*/
		match(STR_T, NO_ATTR);
		gen_incode("PLATY: Output list (string literal) parsed");
		break;
	default: 
		/*empty*/
		gen_incode("PLATY: Output list (empty) parsed");
		break;
	}
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: arithmetic_expression
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:additive_arithmetic_expression(),unary_arithmetic_expression,gen_incode()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <arithmetic expression>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.3.1-------> FIRST(<arithmetic expression>) = { FIRST(<unary arithmetic expression>,FIRST(<additive arithmetic expression>) }
													   = { -, +, AVID_T, FPL_T, INL_T, ( }
****************************************************************************************************************/
void arithmetic_expression() {
	
	switch (lookahead.code) {/*Look for arithmetic operator token*/
	case ART_OP_T:
		/*if attribute matches +(0) or -(1) unary is the type of expression*/
		if (lookahead.attribute.arr_op == PLUS || lookahead.attribute.arr_op == MINUS) {
			unary_arithmetic_expression();/*unary expression FIRST set*/
			gen_incode("PLATY: Arithmetic expression parsed");
		}else {
			syn_printe();/*error print*/
			
		}

		break;
	case AVID_T:/*Arithmetic Variable identifier*/
	case FPL_T:/*Floating point literal*/
	case INL_T:/*Integer literal*/
	case LPR_T:/*left bracket*/
		additive_arithmetic_expression();/*resolves in a additive Arithmetic expression call FIRST sets*/
		gen_incode("PLATY: Arithmetic expression parsed");
		break;

	default :
		syn_printe();
	}
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name:unary_arithmetic_expression
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(),gen_incode(), primary_arithmetic_expression(),syn_printe()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <unary arithmetic expression>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.3.1------->FIRST(<unary arithmetic expression>) = { -,+}
****************************************************************************************************************/
void unary_arithmetic_expression() {
	/*make sure token is ART_OP_T*/
	switch (lookahead.code) {
	case ART_OP_T:
		/*Check if attribute is PLUS*/
		if (lookahead.attribute.arr_op == PLUS) {
			match(ART_OP_T, PLUS);/*match with arithmetic operator token and attribut*/
			primary_arithmetic_expression();/*move on to primary expression*/
		/* if not plus is it MINUS*/
		}else if (lookahead.attribute.arr_op == MINUS) {
			match(ART_OP_T, MINUS);/*match with arithmetic operator token and attribut*/
			primary_arithmetic_expression();/*move on to primary expression*/
		/*Error*/
		}else
			syn_printe();/*Error print*/
		
		break;

	default: break; 
	}
	gen_incode("PLATY: Unary arithmetic expression parsed");
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name:additive_arithmetic_expression
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:multiplicative_arithmetic_expression(),additive_arithmetic_expression()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <additive_arithmetic_expression>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.3.1------->FIRST(<additive arithmetic expression>) = { FIRST(<multiplicative arithmetic expression>) }
															   = { AVID_T, FPL_T, INL_T, LPR_T }
****************************************************************************************************************/
void additive_arithmetic_expression() {
	multiplicative_arithmetic_expression();/*FIRST sets multiplicative called*/
	additive_arthmetic_expression_prime(); /*additive prime called has all the token values*/
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name:additive_arithmetic_expression_prime
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:multiplicative_arithmetic_expression(),additive_arithmetic_expression_prime(), match(),syn_printe()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <additive_arithmetic_expression'>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.3.1------->FIRST(<additive arithmetic expression’>) = { Ɛ, ART_OP_T(-), ART_OP_T(+)}
****************************************************************************************************************/
void additive_arthmetic_expression_prime() {
	switch (lookahead.code) {
	case ART_OP_T:
		/*Check if attribute is PLUS*/
		if (lookahead.attribute.arr_op == PLUS) {
			match(ART_OP_T, PLUS);/*match with arithmetic operator token and attribut*/
			multiplicative_arithmetic_expression();/*multiplicative_arithmetic_expression*/
			additive_arthmetic_expression_prime();/*recursion*/
		/* if not plus is it MINUS*/
		}else if (lookahead.attribute.arr_op == MINUS) {
			match(ART_OP_T, MINUS);/*match with arithmetic operator token and attribut*/
			multiplicative_arithmetic_expression();/*multiplicative_arithmetic_expression*/
			additive_arthmetic_expression_prime();/*recursion*/
		/*Error*/
		}
		else
			syn_printe();/*Error print*/
		break;
	}
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name:multiplicative_arithmetic_expression
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:multiplicative_arithmetic_expression_prime(),primary_arithmetic_expression()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <multiplicative_arithmetic_expression>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.3.1------->FIRST(<multiplicative arithmetic expression>) = { FIRST(<primary arithmetic  expression>)}
																     = {Ɛ, ART_OP_T(DIV), ART_OP_T(MULT)}
****************************************************************************************************************/
void multiplicative_arithmetic_expression() {
	primary_arithmetic_expression();/*calling primary arr expression FIRST set*/
	multiplicative_arithmetic_expression_prime();/*calling multiplicative prime to match possible token values*/
}


/**************************************************************************************************************
										   FUNCTION HEADER
Function Name:multiplicative_arithmetic_expression_prime
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:multiplicative_arithmetic_expression_prime(),primary_arithmetic_expression(),match(), gen_incode()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <multiplicative_arithmetic_expression'>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.3.1------->FIRST(<multiplicative arithmetic expression’>) = {Ɛ, ART_OP_T(/), ART_OP_T(*)}
****************************************************************************************************************/
void multiplicative_arithmetic_expression_prime() {
	switch (lookahead.code) {/*make sure arithmetic operation token is being passed*/
	case ART_OP_T:/*Arithmetic operator token*/
		if (lookahead.attribute.arr_op == MULT) {
			match(ART_OP_T, MULT); /*check for match on * attribute*/
			primary_arithmetic_expression();/*primary arithmetic expression call FIRST set*/
			multiplicative_arithmetic_expression_prime();/*recursion*/
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
		}
		else if (lookahead.attribute.arr_op == DIV) {
			match(ART_OP_T, DIV); /*check for match on / attribute*/
			primary_arithmetic_expression();/*primary arithmetic expression call FIRST set*/
			multiplicative_arithmetic_expression_prime();/*recursion*/
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
		}
		//else
			//syn_printe();/*error print*/
	}
}


/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: primary_arithmetic_expression
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:match(),arithmetic_expression()
Parameters: none
Return:  none
Algorithm:Checks : Function performs the production rules from <primary_arithmetic_expression>  non terminal using the principle
of FIRST set.

GRAMMAR-> 3.3.1------->FIRST(<primary arithmetic expression>) = { AVID_T, FPL_T, INL_T, LPR_T }
****************************************************************************************************************/
void primary_arithmetic_expression() {
	switch (lookahead.code) {/*match code with one of 4 FIRST set posibilites of primary expression*/
	case AVID_T:/*arithmetic variable identifier*/
		match(AVID_T, NO_ATTR);/*match arithmetic variable identifier*/
		break;
	case FPL_T:/*floating point literal*/
		match(FPL_T, NO_ATTR);/*match floating point literal identifier*/
		break;
	case INL_T:/*integer literal*/
		match(INL_T, NO_ATTR);/*match interger literal */
		break;
	case LPR_T:
		match(LPR_T, NO_ATTR);/*if match left bracket must restart to top(recursive call) of arithmetic expression*/
		arithmetic_expression();
		match(RPR_T, NO_ATTR);/*closing bracket match expression finished*/
		break;
	}
	gen_incode("PLATY: Primary arithmetic expression parsed");
}

/*3.3.2 FIRST(<string expression>) ={FIRST(<primary string expression>)} = { SVID_T, STR_T }*/
void string_expression() {}

/*3.3.2 FIRST(<string expression’>) = { Ɛ , SCC_OP_T}*/
void string_expression_prime() {}

/*3.3.2 FIRST(<primary string expression>) = { SVID_T, STR_T }*/
void primary_string_expression() {}

/*3.3.3 FIRST(<conditional expression>) ={FIRST(<logical OR expression>)}
										= { STR_T, SVID_T, AVID_T, FPL_T, INL_T }*/
void conditional_expression() {}

/*3.3.3 FIRST(<logical OR expression>) = {FIRST(<logical AND expression)}
									   = { STR_T, SVID_T, AVID_T, FPL_T, INL_T }*/
void logical_OR_expression() {}

/*3.3.3 FIRST(<logical OR expression’>) = { Ɛ, LOG_OP_T(OR)}*/
void logical_OR_expression_prime() {}

/*3.3.3 FIRST(<logical AND expression>) ={FIRST(<relational expression>)}
										= { STR_T, SVID_T, AVID_T, FPL_T, INL_T }*/
void logical_AND_expression() {}

/*3.3.3 FIRST(<logical AND expression’>) = {Ɛ, LOG_OP_T(AND)}*/
void logical_AND_expression_prime() {}

/*3.3.4 FIRST(<relational expression>) = { FIRST(<primary a_relational expression>),FIRST(primary s_relational expression>) }
									   = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }*/
void relational_expression() {}

/*3.3.4 FIRST(<primary a_relational expression>) = { AVID_T, FPL_T, INL_T }*/
void primary_a_relational_expression() {}

/*3.3.4 FIRST(<primary a_relational expression’>) = { REL_OP_T(EQ), REL_OP_T(NE), REL_OP_T(GT), REL_OP_T(LT) }*/
void primary_a_relational_expression_prime() {}
/*3.3.4 FIRST(<primary s_relational expression>) = { STR_T, SVID_T }*/
void primary_s_relational_expression() {}

/*3.3.4 FIRST(<primary s_relational expression’>) = { REL_OP_T(EQ), REL_OP_T(NE), REL_OP_T(GT), REL_OP_T(LT) }*/
void primary_s_relational_expression_prime() {
	
}