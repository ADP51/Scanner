/************************************************************************************************
							FILE HEADER
 File Name: Scanner.c
Compiler : MS Visual Studio 2019 
Author: Johnathon Cameron and Andrew Palmer
Course: Compilers CST8152_012:
Assignment #: 2 - Scanner
Date: 2019/11/12
Professor: Sv. Ranev
Description: Implementation of a Scanner for the platypus language compiler (Lexical Analyzer)
Function List:
		smalar_next_token(), get_next_state(), char_class,aa_function02(),aa_function03(),aa_function05(),
		aa_function08(), aa_function10(), aa_function12(),iskeyword();
**************************************************************************************************************/
 /* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
  * to suppress the warnings about using "unsafe" functions like fopen()
  * and standard sting library functions defined in string.h.
  * The define does not have any effect in Borland compiler projects.
  */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

  /*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
   It is defined in platy_st.c */
extern pBuffer str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

/* Local(file) global objects - variables */
static pBuffer lex_buf;/*pointer to temporary lexeme buffer*/
static pBuffer sc_buf; /*pointer to input source buffer*/
/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */
static int char_class(char c); /* character class function */
static int get_next_state(int, char); /* state machine function */
static int iskeyword(char* kw_lexeme); /*keywords lookup functuion */

/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: scanner_init
Author: Johnathon Cameron and Andrew Palmer
History/Version: 1.0
Called Function:b_isempty(),b_rewind(),b_clear()
Parameters:     Name:               Type:           Range:
				psc_buf				 Buffer*        any character if it has been read before or empty. 
Return:  Integer : return 0 if success, 1 if did not succeed
Algorithm:Checks : -Check if the buffer is empty, if so return failure.
				   - rewinds the buffer and clear it incase it has be read before
				   - assign sc_buf to psc_buf
				   - exit with a success status
****************************************************************************************************************/
/*Initializes scanner */
int scanner_init(Buffer* psc_buf)
{
	if (b_isempty(psc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	b_rewind(psc_buf);
	b_clear(str_LTBL);
	line = 1;
	sc_buf = psc_buf;
	return EXIT_SUCCESS;/*0*/
	/*   scerrnum = 0;  *//*no need - global ANSI C */
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: malar_next_token
Author: Andrew Palmer and Johnathon Cameron
History/Version: 1.5
Called Function: isalnum(),b_getc(),b_retract(),b_getcoffset(),b_allocate(),b_addc(),free(),b_mark()
Parameters:N/A
Return:  Token
Algorithm: 
			- Get a character from the buffer
			- if the character is source end of file set the token and attribute for seof
			- State Machine algorithm to see if a letter or a digit is found. 


			PART 1 : Check For symbols within switch case and assign token code and attributes

						- Symbols defined within the language : ",.,+,-,=,*,/,<<,{,},(,), <,>,<>,;,!
						- Logical Operators : .AND. .OR.

						- if any of these operators are found, assign the token code and attribute if applies and return the token.
						- Check for any invalid symbols within the case default and return error representing syntax error
				
				PART 2: State machine has to be done first to avoid problems when looking for invalid symbols in the switch case	
					State Machine: 
						-set the lexstart to the beginning of the input
						-set markcoffset of the input buffer to the start
						-init starting state to 0 
						-if the state is NOAS keep looping until the state reaches and accepting state
						-if the state a retracting accepting state retract the buffer
						-set lexend when it has reach an accepting state
						-allocate temporary fixed mode buffer
						-reset getcoffset to start at the beginning of the Lexeme
						-copy the lexeme to the lex_buf and add the SEOF to show the end of a string identifier
						-call the accepting functions within the table
						-free the temporary buffer and return the token							 
****************************************************************************************************************/
Token malar_next_token(void) {

	Token t = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c; /* input symbol */
	int state = 0; /* initial state of the FSM */
	short lexstart;  /*start offset of a lexeme in the input char buffer (array) */
	short lexend;    /*end   offset of a lexeme in the input char buffer (array)*/

	while (1) { /* endless loop broken by token returns it will generate a warning */

	

		c = b_getc(sc_buf);


		/****************************************
			PART 1  Symbol Check
		****************************************/
		/*Switch case to find language syntax*/
		switch (c) {
		case SEOF:
		case S_EOF:
			t.code = SEOF_T;
			t.attribute.seof = SEOF_EOF;
			return t;

		case NL: /* Check for New Line */
			line++; /* increment line number */
			continue;
		case ' ':
			continue; /* If char is whitespace ignore and continue */
		case '\t':
			continue;
		case '=':
			c = b_getc(sc_buf); /* get the next char */
			if (c == '=') { /* If the next char is = then == means relations operator */
				t.code = REL_OP_T;
				t.attribute.rel_op = EQ;
				return t;
			}
			b_retract(sc_buf); /* If not == then retract c back onto buffer */
			t.code = ASS_OP_T; /* set token to assignment operator */
			return t;
		case '(':
			t.code = LPR_T; /*Left parenthesis token*/
			return t;
		case ')':
			t.code = RPR_T;/*Right parenthesis token*/
			return t;
		case '{':
			t.code = LBR_T;/*left bracket token*/
			return t;
		case '}':
			t.code = RBR_T;/*right bracket token*/
			return t;
		case '<':
			c = b_getc(sc_buf);
			if (c == '>') { /* check for not equal operator */
				t.code = REL_OP_T; /*relation operator token*/
				t.attribute.rel_op = NE;/*attribute*/
				return t;
			}
			else if (c == '<') { /* Check for String concatenation */
				t.code = SCC_OP_T;/*Strong string concatination token*/
				return t;
			}
			b_retract(sc_buf);
			t.code = REL_OP_T;/*relation operator token*/
			t.attribute.rel_op = LT; /* Assign less than attribute */
			return t;
		case '>':
			t.code = REL_OP_T;/*relational operator token*/
			t.attribute.rel_op = GT;
			return t;
		case ';':
			t.code = EOS_T;/*end of statement token*/
			return t;
		case '!':
			c = b_getc(sc_buf);

			if (c != '!') {
				t.code = ERR_T; /*Error token for comments Ex: !1 - invalid*/
				t.attribute.err_lex[0] = '!';
				t.attribute.err_lex[1] = c;
				t.attribute.err_lex[2] = '\0';
				return t;
			}
			while (c != NL) { /* Loop through and ignore the entire line*/
				c = b_getc(sc_buf);
				/*Check for End of File after comments*/
				if (c == SEOF ) {
					t.code = SEOF_T;
					t.attribute.seof = SEOF_EOF;
					return t;
				}
			}
			
			line++;/* increment line */
			
			/*ignore comment*/
			continue;
			
		case ',':
			t.code = COM_T;/*comma token*/
			return t;

		/*** Arithmetic operators ***/
		case '-':
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS;/*subtraction token*/
			return t;
		case '+':
			t.code = ART_OP_T;/*addition token*/
			t.attribute.arr_op = PLUS;
			return t;
		case '*':
			t.code = ART_OP_T;/*multiplication token*/
			t.attribute.arr_op = MULT;
			return t;
		case '/':
			t.code = ART_OP_T;/*division token*/
			t.attribute.arr_op = DIV;
			return t;

		case'.':
			/*Setting markoffset to the char preceding the .*/
			b_mark(sc_buf, b_getcoffset(sc_buf));
			/*get character from buffer*/
			c = b_getc(sc_buf);

			/*try and process logical operators by getting next characters from buffer NOTE: must have preceding .
			  else error token is processed*/
			  /*Check for AND*/
			if (c == 'A' && b_getc(sc_buf) == 'N' && b_getc(sc_buf) == 'D' && b_getc(sc_buf) == '.') {
				t.code = LOG_OP_T;/*Logical operator token*/
				t.attribute.log_op = AND;/* AND value*/
				return t;
				/*Check for OR*/
			}
			else if (c == 'O' && b_getc(sc_buf) == 'R' && b_getc(sc_buf) == '.') {
				t.code = LOG_OP_T;/*Logical operator token*/
				t.attribute.log_op = OR;/* AND value*/
				return t;
				/*Return Error if none are found*/
			}else {
				/*Error code set*/
				/*Reset buff back to the location preceding the case symbol*/
				b_reset(sc_buf);
				t.code = ERR_T;
				/*cause of error sent to err_lex*/
				t.attribute.err_lex[0] = '.';
				t.attribute.err_lex[1] = '\0';
				return t;
			}
		}

		/*************************************
		PART 2 State Machine
		*************************************/
		if (isalnum((int)c) != 0 || c == '"') {
			lexstart = b_retract(sc_buf); /*set lexstart to the beginning of the input*/
			b_mark(sc_buf, lexstart); /* Set Markcoffset of the input buffer to the lexstart*/
			state = 0; /*Start at state 0*/
			c = b_getc(sc_buf);

			/* If the state is NOAS loop until it reaches an AS */
			while (as_table[state] == NOAS) {
				state = get_next_state(state, c);
				if (as_table[state] != NOAS) { /* Break the loop at the correct char */
					break;
				}
				c = b_getc(sc_buf);
			}

			/* if ASWR retract buffer */
			if (as_table[state] == ASWR) {
				b_retract(sc_buf);
			}

			/* Reached an Accepting state set lexend */
			lexend = (short)b_getcoffset(sc_buf);

			/* Create temporary buffer */
			lex_buf = b_allocate((lexend - lexstart) + 1, 0, 'f');
			if (lex_buf == NULL) { /*Error creating buffer*/
				scerrnum = 1;
				aa_func12("RUN TIME ERROR: ");
			}

			b_reset(sc_buf); /* reset getcoffset to the start of the LEXEME */

			/* Copy the LEXEME to the lex_buf */
			for (int i = lexstart; i < lexend; i++) {
				b_addc(lex_buf, b_getc(sc_buf));
			}
			b_addc(lex_buf, SEOF); /* Add SEOF to signify end of string */
			t = aa_table[state](b_location(lex_buf)); /* calls the accepting function */
			b_free(lex_buf); /* frees the temp buffer */
			return t;
		}

		t.code = ERR_T;
		t.attribute.err_lex[0] = c;
		t.attribute.err_lex[1] = '\0';
		return t;
	}
}

/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: get_next_state()
Author: Andrew Palmer and Johnathon Cameron
History/Version: 1.5
Called Function:
Parameters:     Name:               Type:           Range:
				state			    state            0-12
				c                   char	         any possible char
Return:  Integer : returning next state
Algorithm:Checks : gets the next state in the column st_table
****************************************************************************************************************/
int get_next_state(int state, char c) {
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	assert(next != IS);


#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	return next;
}

/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: char_class
Author: Andrew Palmer
History/Version: 1.3
Called Function:isalpha(),isdigit()
Parameters:     Name:               Type:           Range:
				c				    char            any possible symbol

Return:  Integer : value of the column from the state table (st_table) 
Algorithm:Checks : char is equal to  - [a-zA-Z] (is a letter) -- VAL = 0
									 - 0 (is zero) -- VAL = 1
									 - [1-9] (is a digit from 1 to 9) -- VAL = 2
									 - . (period) -- VAL = 3
									 - @ (at symbol) -- VAL = 4
									 - " (quotes) -- VAL = 6
									 - SEOF(source end of file) -- VAL = 7
									 - Other -- VAL = 6

									 returns the column value;
****************************************************************************************************************/
int char_class(char c) {
	int val = 5;
	/*Column 0 [a-zA-Z]*/
	if (isalpha(c))
		val = 0;
	/*Column 1 value 0 */
	else if (c == '0')
		val = 1;
	/*Column 2 value [1-9]*/
	else if (isdigit(c))
		val = 2;
	/*Column 3 value . */
	else if (c == '.')
		val = 3;
	/*Column 4 value @*/
	else if (c == '@')
		val = 4;
	/*Column 5  value " */
	else if (c == '"')
		val = 6;
	/*Column 6 value SEOF*/
	else if (c == SEOF)
		val = 7;
	else
		/*Column 7 value other*/
		val = 5;

	return val;
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: aa_funct02
Author: Johnathon Cameron
History/Version: 1.3
Called Function: strcmp(),strlen()
Parameters:     Name:               Type:           Range:
				lexeme				char[]			length of the lexeme array

Return:  Token  posibility: keyword token or AVID Token
Algorithm: ACCEPTING FUNCTION FOR THE arithmentic variable identifier AND keywords (VID - AVID/KW)
		   State 02 ACCEPTING STATE NUMBER.
		   
		   -Task 1: Check if the char array [] lexeme is holding a keyword, if found set keyword index to i, and set token code to KW_T
		   
		   -Task 2: If no keyword was found. set AVID token and process the characters within the array
					if lexeme is greater then 8 characters, it takes the first 8 characters and stores string identifier \0 at the last position
					else if lexeme is less then 8 characters it stores all chars within the lexeme in 
 
****************************************************************************************************************/
Token aa_func02(char lexeme[]) {
	/*temp token*/
	Token t;
	/*unsigned integers for loop count*/
	unsigned int j = 0;
	/********STEP 1 Check if lexeme has a KEYWORD*******/
	int isKeyword = iskeyword(lexeme);
	/*If a keyword is found*/
	if (isKeyword != KEYWORD_NOT_FOUND) {
		t.code = KW_T;
		/*set the keyword index from keyword table*/
		t.attribute.kwt_idx = isKeyword;
		return t;
	}
	/*******STEP 2 Arithmetic Variable identifier token**********/
		/*Set AVID token*/
	t.code = AVID_T;
	/*if lexeme is > 8 chars*/
	if (strlen(lexeme) > VID_LEN) {
		/*loop through and add lexeme chars to vid_lex*/
		for (j = 0; j < VID_LEN; j++) {
			t.attribute.vid_lex[j] = lexeme[j];
		}
		/*add string terminator at last position*/
		t.attribute.vid_lex[j] = '\0';
		return t;
	}
	/*if lexeme is <  then 8 characters*/
	for (j = 0; j < strlen(lexeme); j++) {
		t.attribute.vid_lex[j] = lexeme[j];
	}
	t.attribute.vid_lex[j] = '\0';
	return t;

}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: aa_funct03
Author: Andrew Palmer
History/Version: 1.3
Called Function: strlen()
Parameters:     Name:               Type:           Range:
				lexeme				char[]			length of the lexeme array

Return:  Token  posibility: String variable identifier token 
Algorithm: ACCEPTING FUNCTION FOR THE string variable identifier (VID - SVID)
		   State 03 ACCEPTING STATE NUMBER
		  
		  -Task1: set token code to SVID_T(string variable identifier)
		   check the length of lexeme if its greater then vid_len (8 char) take the first vid_len-1 from lexeme and add to vid_lex.
		   add an @ variables identifier symbol and string identifier \0 at the end of vid_lex (two last positions) 
		   
		  -Task2: if lexeme is less then vid_len (8 char) add lexeme char to vid_lex and add the string type identifier to the last position (\0) 

****************************************************************************************************************/
Token aa_func03(char lexeme[]) {
	/*Token to return*/
	Token t;
	/*setting token code*/
	t.code = SVID_T;
	/*loop variable*/
	unsigned int i;

	/********DO IF lexeme is greater then VID_LEN*******/
	if (strlen(lexeme) > VID_LEN) {
		/*loop to set vid_lex to lexeme values until len-1*/
		for (i = 0; i < VID_LEN - 1; i++) {
			t.attribute.vid_lex[i] = lexeme[i];
		}

		/*assigning to second last positions @ symbol*/
		t.attribute.vid_lex[VID_LEN - 1] = '@';
		/*assign string terminator to last position in array*/
		t.attribute.vid_lex[VID_LEN] = '\0';

		return t;
	}
	/*********DO IF lexeme is less then VID_LEN**********/
	for (i = 0; i < strlen(lexeme); i++) {
		t.attribute.vid_lex[i] = lexeme[i];
	}
	/*assign string terminator to last position in array*/
	t.attribute.vid_lex[i] = '\0';

	return t;
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: aa_funct05
Author: Johnathon Cameron
History/Version: 1.3
Called Function: atoi()
Parameters:     Name:               Type:           Range:
				lexeme				char[]			length of the lexeme array

Return:  Token  posibility: Integer literal or Error token
Algorithm: ACCEPTING FUNCTION FOR THE integer literal(IL) - decimal constant (DIL)
		   
		   -Task1: convert lexeme to integer and verify it is within the range of a 2 byte integer max and min (short) return error token if not
			if lexeme is integer literal, assign token code to integer literal code and int value to the value of the lexeme integer.

****************************************************************************************************************/
Token aa_func05(char lexeme[]) {
	/*temporary token*/
	Token t;
	/*variable storage to store the converted integer*/
	int toInt;
	/*convert the lexeme to integer*/
	toInt = atoi(lexeme);

	/*if the integer is out of 2 byte integer range, return error*/
	if (toInt > SHRT_MAX || toInt < SHRT_MIN || strlen(lexeme) > INL_LEN)
		return aa_func12(lexeme);

	/*set token code*/
	t.code = INL_T;
	/*set attribute*/
	t.attribute.int_value = toInt;

	return t;
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: aa_funct08
Author: Andrew Palmer
History/Version: 1.3
Called Function: atof()
Parameters:     Name:               Type:           Range:
				lexeme				char[]			length of the lexeme array

Return:  Token  posibility: Floating point literal to or Error
Algorithm: ACCEPTING FUNCTION FOR THE floating-point literal (FPL)

		   -Task1: convert lexeme to double (atof return double) check if toFloat is within the range the max and min of a 4byte floating point number (float) 
			       return error token if it isn't
				   if it is, set token to FPL_T(floating point token) 
				   and attribute float value to value of toFLoat

****************************************************************************************************************/
Token aa_func08(char lexeme[]) {
	/*temp token storage*/
	Token t;
	/*storage for string conversion to float*/
	double toFloat;
	/*convert string to double(atof return double) just to make it possible to check for max and min*/
	toFloat = atof(lexeme);
	/*check for error if lexeme is greater then 32bits (4bytes x 8 bits = 32bits) also if its greater or less then the min and max range*/
	if (((toFloat >= 0 && strlen(lexeme) > 32) && (toFloat< FLT_MIN || toFloat > FLT_MAX)) || (toFloat < 0)) return aa_func12(lexeme);
	/*setting the token code to floating point token*/
	t.code = FPL_T;
	/*setting attribute floating point value*/
	t.attribute.flt_value = (float)toFloat;

	return t;
}
/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: aa_funct10
Author: Johnathon Cameron
History/Version: 1.3
Called Function:strlen(), b_addc()
Parameters:     Name:               Type:           Range:
				lexeme				char[]			length of the lexeme array

Return:  Token  posibility: String literal token 
Algorithm: ACCEPTING FUNCTION FOR THE string literal(SL)

		   Task1:  -set token str offset attribute to head of string literal table
				   -loop through the lexeme array and add string literal from lexeme to buffer str_LTBL
				   -ignore the opening and closing "" and keep count when a new line character is found.
				   -add the String type identifier \0 when all lexeme char are transfered and set String token code

****************************************************************************************************************/
Token aa_func10(char lexeme[]) {
	/*Temporary token*/
	Token t;
	/*counter variable*/
	unsigned int i;

	/*SET the token code to STR_T Token*/
	t.code = STR_T;

	/*Set token attribute to head of str table using b_limit*/
	t.attribute.str_offset = b_limit(str_LTBL);

	for (i = 0; i < strlen(lexeme); i++) {

		/*ignoring opening and closing "*/
		if (lexeme[i] != '"')
			b_addc(str_LTBL, lexeme[i]);

		
		/*if new line character is found */
		if (lexeme[i] == '\n')
			line++;

	}
	/*adding \0 to make the String c Type String*/
	b_addc(str_LTBL, '\0');


	return t;
}

/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: aa_funct12
Author: Andrew Palmer
History/Version: 1.3
Called Function:strlen()
Parameters:     Name:               Type:           Range:
				lexeme				char[]			length of the lexeme array

Return:  Token  posibility: Error token
Algorithm: ACCEPTING FUNCTION FOR THE ERROR TOKEN 

		   Task1:  -Check if the lexeme err characters are greater then ERR_LEN (20 char)
				   -if they add the lexeme char to err_lex with a ERR_LEN-3 size
				   -add . . . at the last 3 before last positions and add the string type identifier at the last position
				   -Note keep count of lines when new line character is found
		   Task2:  If lexeme is less then ERR_LEN (20 char) add lexeme char to err_lex and add the string type identifier at the last position

****************************************************************************************************************/
Token aa_func12(char lexeme[]) {
	/*Token storage*/
	Token t;
	/*Assigning the token code to ERR_T*/
	t.code = ERR_T;
	/*Checking if the lexeme string length is greater then ERR_LEN*/
	if (strlen(lexeme) > ERR_LEN) {
		int i = 0;

		/*search through the entire lexeme but only put the characters up to ERR_LEN -3*/
		for (i = 0; i < strlen(lexeme); i++) {
			if (lexeme[i] == NL) line++;

			if (i < ERR_LEN - 3) {
				t.attribute.err_lex[i] = lexeme[i];
			}
		}

		/*Fil the 4 remaining positions in err_lex*/
		t.attribute.err_lex[ERR_LEN - 3] = '.';
		t.attribute.err_lex[ERR_LEN - 2] = '.';
		t.attribute.err_lex[ERR_LEN - 1] = '.';
		/*assign string \0 type*/
		t.attribute.err_lex[ERR_LEN] = '\0';
		return t;
	}
	/*If lexeme is shorter then ERR_LEN (20 char) */
	unsigned int j = 0;

	for (j = 0; j < strlen(lexeme); j++) {
		/*nl check*/
		if (lexeme[j] == '\n') line++;
		t.attribute.err_lex[j] = lexeme[j];
	}
	/*assign string type*/
	t.attribute.err_lex[++j] = '\0';

	return t;
}

/**************************************************************************************************************
										   FUNCTION HEADER
Function Name: iskeyword()
Author: Johnathon Cameron
History/Version: 1.3
Called Function:strcmp()
Parameters:     Name:               Type:           Range:
				kw_lexeme			char *			any possible string or symbol

Return:  -1 or keyword index of the keyword table
Algorithm: Used to check if a String is a keyword or not
			
		   -Task 1: check if lexeme is null
		   -Task 2: loop through the keyword table (defined in table.h) and compare string. If match then return the index from where it matches
		            if there is no match, return -1. Cannot return 0 because it is an index within the keyword table.

****************************************************************************************************************/
int iskeyword(char* kw_lexeme) {
	/*Loop counter*/
	int i;
	/*Check if kw_lexeme has data return -1 if it is(cannot do index integer)*/
	if (kw_lexeme == NULL)
		return RT_FAIL_1;

	/*Loop through the size of the keyword table and compare with the kw_lexeme*/
	for (i = 0; i < KWT_SIZE; i++) {
		if (strcmp(kw_table[i], kw_lexeme) == MATCH) {
			return i;
		}
	
	}
	/*return -1 if no keyword is found (cannot have a returning integer that matches and index*/
	return KEYWORD_NOT_FOUND;

}


