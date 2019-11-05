/* Filename: scanner.c
/* PURPOSE:
 *    SCANNER.C: Functions implementing a Lexical Analyzer (Scanner)
 *    as required for CST8152, Assignment #2
 *    scanner_init() must be called before using the scanner.
 *    The file is incomplete;
 *    Provided by: Svillen Ranev
 *    Version: 1.19.2
 *    Date: 2 October 2019
 *******************************************************************
 *    REPLACE THIS HEADER WITH YOUR HEADER
 *******************************************************************
 */

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


/*Initializes scanner */
int scanner_init(pBuffer psc_buf) {
	if (b_isempty(psc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	b_rewind(psc_buf);
	b_clear(str_LTBL);
	line = 1;
	sc_buf = psc_buf;
	return EXIT_SUCCESS;/*0*/
/*   scerrnum = 0;  *//*no need - global ANSI C */
}

Token malar_next_token(void) {

	Token t = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c; /* input symbol */
	int state = 0; /* initial state of the FSM */
	short lexstart;  /*start offset of a lexeme in the input char buffer (array) */
	short lexend;    /*end   offset of a lexeme in the input char buffer (array)*/

	/*DECLARE YOUR LOCAL VARIABLES HERE IF NEEDED*/

	while (1) { /* endless loop broken by token returns it will generate a warning */

		c = b_getc(sc_buf);


		/* Part 1: Implementation of token driven scanner */
		/* every token is possessed by its own dedicated code */
			  /*  Special case tokens processed separately one by one
			   *  in the token-driven part of the scanner
			   *  '=' , ' ' , '(' , ')' , '{' , '}' , == , <> , '>' , '<' , ';',
			   *  white space
			   *  !!comment , ',' , ';' , '-' , '+' , '*' , '/', << ,
			   *  .AND., .OR. , SEOF,
			   */

		switch (c) {
		case SEOF:
			t.code = SEOF_T;
			break;
		case ' ':
			continue; /* If char is whitespace ignore and continue */
		case NL: /* Check for New Line */
			line++; /* increment line number */
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
			t.code = LPR_T;
			return t;
		case ')':
			t.code = RPR_T;
			return t;
		case '{':
			t.code = LBR_T;
			return t;
		case '}':
			t.code = RBR_T;
			return t;
		case '<':
			c = b_getc(sc_buf);
			if (c == '>') { /* check for not equal operator */
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
				return t;
			}
			else if (c == '<') { /* Check for String concatenation */
				t.code = SCC_OP_T;
				return t;
			}
			b_retract(sc_buf);
			t.code = REL_OP_T;
			t.attribute.rel_op = LT; /* Assign less than attribute */
			return t;
		case '>':
			t.code = REL_OP_T;
			t.attribute.rel_op = GT;
			return t;
		case ';':
			t.code = EOS_T;
			return t;
		case '!':
			c = b_getc(sc_buf);
			if (c == '!') { /* Check for !! comment symbol */
				while (c != NL) { /* Loop through and ignore the entire line*/
					b_getc(sc_buf);
				}
				line++;/* increment line */
				continue;
			}
			else {
				t.code = ERR_T;
				t.attribute.err_lex[0] = '!';
				t.attribute.err_lex[1] = c;
				t.attribute.err_lex[2] = '\0';
				while (c != NL) {
					c = b_getc(sc_buf);
				}
				line++;
				return t;
			}
		case ',':
			t.code = COM_T;
			return t;

		/* Arithmetic operators */
		case '-':
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS;
			return t;
		case '+':
			t.code = ART_OP_T;
			t.attribute.arr_op = PLUS;
			return t;
		case '*':
			t.code = ART_OP_T;
			t.attribute.arr_op = MULT;
			return t;
		case '/':
			t.code = ART_OP_T;
			t.attribute.arr_op = DIV;
			return t;
		case '.':

		}
	}
}

/*				NOTE :
			IF LEXICAL ERROR OR ILLEGAL CHARACTER ARE FOUND THE SCANNER MUST RETURN AN ERROR TOKEN.
				ILLEGAL CHARACTER IS ONE THAT IS NOT DEFINED IN THE LANGUAGE SPECIFICATION
				OR IT IS OUT OF CONTEXT.
				THE ILLEGAL CHAR IS THE ATTRIBUTE OF THE ERROR TOKEN
				THE ILLEGAL CHARACTERS ARE PROCESSED BY THE TRANSITION TABLE.
				SOME OF THE LEXICAL ERRORS ARE ALSO PROCESSED BY THE TRANSITION TABLE.

				IN A CASE OF RUNTIME ERROR, THE FUNCTION MUST STORE
				A NON - NEGATIVE NUMBER INTO THE GLOBAL VARIABLE scerrnum
				AND RETURN A RUN TIME ERROR TOKEN.THE RUN TIME ERROR TOKEN ATTRIBUTE
				MUST BE THE STRING "RUN TIME ERROR: "

				IF(c == SOME CHARACTER)
				...
				SKIP CHARACTER(FOR EXAMPLE SPACE)
				continue;
			OR SET TOKEN(SET TOKEN CODE AND TOKEN ATTRIBUTE(IF AVAILABLE))
				return t;
		EXAMPLE:
			if (c == ' ') continue;
			if (c == '{') {
				t.code = RBR_T; /*no attribute *//* return t;
			/*	if (c == '+') {
					t.code = ART_OP_T; t.attribute.arr_op = PLUS * / return t;
					...

						IF(c == '.') TRY TO PROCESS.AND. or .OR.
						IF SOMETHING ELSE FOLLOWS.OR THE LAST.IS MISSING
						RETURN AN ERROR TOKEN
						IF(c == '!') TRY TO PROCESS COMMENT
						IF THE FOLLOWING CHAR IS NOT !REPORT AN ERROR
						ELSE IN A LOOP SKIP CHARACTERS UNTIL line terminator is found THEN continue;
					...

						IF(c == ANOTHER CHARACTER)
						SET TOKEN
						return t;


					/* Part 2: Implementation of Finite State Machine (DFA)
							   or Transition Table driven Scanner
							   Note: Part 2 must follow Part 1 to catch the illegal symbols
					*/

					/*		SET THE MARK AT THE BEGINING OF THE LEXEME AND SAVE IT IN lexstart
								lexstart = b_mark(sc_buf, ...);
							....
								PLACE THE CODE OF YOUR FINITE STATE MACHINE HERE(FSM or DFA)
								IT MUST IMPLEMENT THE FOLLOWING ALGORITHM :

							FSM0.Begin with state = 0 and the input character c.
								FSM1.Get the next state from the transition table calling
								state = get_next_state(state, c);
							FSM2.Use the as_table to get the type of the state.
								If the state is not an accepting(NOAS) state,
								get the next character from the input bufferand repeat FSM1.
								FSM3.If the state is an accepting state, token is found, leave the machineand
								call an accepting function as described below.

								IF THE ACCEPTING STATE IS A RETRACTING ACCEPTING STATE
								RETRACT  getc_offset USING AN APPROPRIATE BUFFER FUNCTION.

								SET lexend TO getc_offset USING AN APPROPRIATE BUFFER FUNCTION

								CREATE  A TEMPORRARY LEXEME BUFFER HERE;
							lex_buf = b_allocate(...);
							.RETRACT getc_offset to the MARK SET PREVIOUSLY AT THE BEGINNING OF THE LEXEME AND
								.USING b_getc() COPY THE LEXEME BETWEEN lexstart AND lexend FROM THE INPUT BUFFER INTO lex_buf USING b_addc(...),
								. WHEN VID(KEYWORDS INCLUDED), FPL, IL OR SL IS RECOGNIZED
								.YOU MUST CALL THE ACCEPTING FUNCTION USING THE ARRAY aa_table, WHICH
								.CONTAINS POINTERS TO FUNCTIONS.THE ARRAY INDEX OF THE FUNCTION TO BE
								.CALLED IS STORED IN THE VARIABLE state.
								.YOU ARE NOT ALLOWED TO CALL ANY OF THE ACCEPTING FUNCTIONS BY NAME.
								.THE ARGUMENT TO THE FUNCTION IS THE STRING STORED IN lex_buf char array.
								....
								b_free(lex_buf);

							return t;
						}end while(1)
					}*/


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
int char_class(char c) {
	int val = 7;
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
		val = 5;
	/*Column 6 value SEOF*/
	else if (c == SEOF)
		val = 6;
	else
		/*Column 7 value other*/
		val = 7;

	return val;
}

Token aa_func02(char lexeme[]) {
	Token t;
	unsigned int i, j = 0;
	/********STEP 1 Check if lexeme has a keyword*******/
	for (i = 0; i < KWT_SIZE; i++) {
		/*if keyword is found using string compare*/
		if (strcmp(lexeme, kw_table[i]) == 0) {
			/*set keyword token*/
			t.code = KW_T;
			/*set the keyword index from keyword table*/
			t.attribute.kwt_idx = i;
			return t;
		}
	}

	/*******STEP 2 if no keyword is found**********/
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


Token aa_func05(char lexeme[]) {
	/*temp token storage*/
	Token t;
	/*storage for string conversion to float*/
	double toFloat;
	/*convert string to double(atof return double) just to make it possible to check for max and min*/
	toFloat = atof(lexeme);
	/*check if the value is less or greater then MAX or MIN*/
	if (toFloat < FLT_MIN || toFloat > FLT_MAX)
		/*return err token*/
		return aa_func12(lexeme);

	/*setting the token code to floating point token*/
	t.code = FPL_T;
	/*setting attribute floating point value*/
	t.attribute.flt_value = (float)toFloat;

	return t;
}

Token aa_func08(char lexeme[]) {
	/*temporary token*/
	Token t;
	/*variable storage to store the converted integer*/
	int toInt;
	/*convert the lexeme to integer*/
	toInt = atoi(lexeme);

	/*if the integer is out of 2 byte integer range, return error*/
	if (toInt > SHRT_MAX || toInt < SHRT_MIN)
		return aa_func12(lexeme);

	/*set token code*/
	t.code = INL_T;
	/*set attribute*/
	t.attribute.int_value = toInt;

	return t;
}

Token aa_func10(char lexeme[]) {
	/*Temporary token*/
	Token t;
	/*counter variable*/
	unsigned int i;

	/*Set token attribute to head of str table using b_location*/
	t.attribute.str_offset = b_location(str_LTBL);

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

	/*SET the token code to STR_T Token*/
	t.code = STR_T;
	return t;
}


Token aa_func12(char lexeme[]) {
	/*Token storage*/
	Token t;
	/*Assigning the token code to ERR_T*/
	t.code = ERR_T;
	/*Checking if the lexeme string length is greater then ERR_LEN*/
	if (strlen(lexeme) > ERR_LEN) {
		int i = 0;

		/*Assigning lexeme characters to t.attribute.err_lex within for loop and assigning the last 3 char ...*/
		for (i = 0; i < ERR_LEN - 3; i++) {
			/*if new line character is found*/
			if (lexeme[i] == '\n') line++;
			/*adding characters to err_lex*/
			t.attribute.err_lex[i] = lexeme[i];
		}
		/* assign the last three chars to err_lex ...*/
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



/*		HERE YOU WRITE YOUR ADDITIONAL FUNCTIONS(IF ANY).
			FOR EXAMPLE
			int iskeyword(char* kw_lexeme) {}*/