/**********************************************************************************************************
* File Name:		buffer.c
* Compiler:			MS Visual Studio 2013
* Author:			Sv. Ranev, Andrew Palmer
* Course:			CST 8152 - Compilers, Lab Section: 012
* Assignment:		1
* Date:				29/9/2019
* Professor:		Sv.Ranev
* Purpose:			Contains buffer structure and function definitions
********************************************************************************************/
#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

/* constant definitions */
#define RT_FAIL_1 (-1)         /* operation failure return value -1 */
#define RT_FAIL_2 (-2)         /* operation failure return value -2 */
#define LOAD_FAIL (-2)         /* load fail return value */

#define DEFAULT_INIT_CAPACITY 200   /* default initial buffer capacity */
#define DEFAULT_INC_FACTOR 15       /* default increment factor */


/* You should add your own constant definitions here */

/* Add your bit-masks constant definitions here */
#define DEFAULT_FLAGS  0xFFFC	/* The default flags value */
#define SET_EOB  0xFFFE			/* The value used to set EOB to 1 */
#define RESET_EOB 0xFFFD		/* The value used to reset EOB */
#define CHECK_EOB 0x002			/* Value used to check EOB */
#define SET_R_FLAG 0xFFFD		/* Value used to se r_flag */
#define RESET_R_FLAG 0xFFFE		/* Value used to reset r_flag */
#define CHECK_R_FLAG 0x0001		/* Value used to check r_flag */
#define O_MODE_A 1				/* Value to set mode to additive */
#define O_MODE_F 0				/* Value to set mode to fixed */
#define O_MODE_M (-1)			/* Value to set mode to multiplicative */
#define TRUE 1					/* Value of true */
#define FALSE 0					/* Value of false */
#define INCFACTOR_FAIL 0x100	/* Value to be returned for runtime error */

/* user data type declarations */
typedef struct BufferDescriptor {
	char* cb_head;   /* pointer to the beginning of character array (character buffer) */
	short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
	short addc_offset;  /* the offset (in chars) to the add-character location */
	short getc_offset;  /* the offset (in chars) to the get-character location */
	short markc_offset; /* the offset (in chars) to the mark location */
	char  inc_factor; /* character array increment factor */
	char  mode;       /* operational mode indicator*/
	unsigned short flags;     /* contains character array reallocation flag and end-of-buffer flag */
} Buffer, * pBuffer;
/*typedef Buffer *pBuffer;*/

/* function declarations */
Buffer* b_allocate(short init_capacity, char inc_factor, char o_mode);
Buffer* b_addc(pBuffer const pBD, char symbol);
int b_clear(Buffer* const pBD);
void b_free(Buffer* const pBD);
int b_isFull(Buffer* const pBD);
short b_limit(Buffer* const pBD);
short b_capacity(Buffer* pBD);
short b_mark(pBuffer const pBD, short mark);
int b_mode(Buffer* const pBD);
size_t b_incfactor(Buffer* const pBD);
int b_load(FILE* const fi, Buffer* const pBD);
int b_isempty(Buffer* const pBD);
char b_getc(Buffer* const pBD);
int b_eob(Buffer* const pBD);
int b_print(Buffer* const pBD, char nl);
Buffer* b_compact(Buffer* const pBD, char symbol);
char b_rflag(Buffer* const pBD);
short b_retract(Buffer* const pBD);
short b_reset(Buffer* const pBD);
short b_getcoffset(Buffer* const pBD);
int b_rewind(Buffer* const pBD);
char* b_location(Buffer* const pBD);

#endif

