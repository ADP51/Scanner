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
	/*print out successful parse*/	gen_incode("PLATY: Source file parsed");}/**************************************************************************************************************
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
****************************************************************************************************************/void match(int pr_token_code, int pr_token_attribute) {	/*if lookahead is equal to source end of file*/	if (lookahead.code == SEOF_T) return;	/*switch case to verify attribute match with specific code*/	switch (pr_token_code) {	case KW_T: 		/*May have to do some special handling depending on the production rules TODO*/		/*checking if the token attribute is equivalent to the keyword table index*/		if (pr_token_attribute != lookahead.attribute.kwt_idx) {			/*error handler function call if it isn't*/			syn_eh(pr_token_code);			return;		}		break;	case LOG_OP_T:	case ART_OP_T:	case REL_OP_T:		/*if attribute is not equal to lookahead attribute call error handler function*/		if (pr_token_attribute != lookahead.attribute.get_int) {			syn_eh(pr_token_code);			return;		}		break;	/*break if pr_token_code isn't part of the attribute check*/	default: break;	}	/**********************		no SEOF match	***********************/	/*advance to next token*/	lookahead = malar_next_token();	/*check if new token is error token*/	if (lookahead.code == ERR_T) {		/*error printing function*/		syn_printe();		/*look ahead to next token again*/		lookahead = malar_next_token();		/*increment error counter*/		synerrno++;		return;	}	/*if match is unsuccessful*/	if (lookahead.code != pr_token_code) {
		/*error handling function*/
		syn_eh(pr_token_code);
		return;
	}}
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
		lookahead = malar_next_token(sc_buf);

		/*matching token is found, advances the input token one more time before returning*/
		if (lookahead.code == sync_token_code) {
			lookahead = malar_next_token(sc_buf);
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

GRAMMAR->
-FIRST(<program>) = {KW_T(PLATYPUS)}
-FIRST(<opt_statements>) = {Ɛ, AVID_T, SVID_T, KW_T(IF), KW_T(USING), KW_T(READ), KW_T(WRITE)}
****************************************************************************************************************/
void program() {
		match(KW_T, PLATYPUS); match(LBR_T, NO_ATTR); opt_statements();
		match(RBR_T, NO_ATTR);
		gen_incode("PLATY: Program parsed");
}
