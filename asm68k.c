//
//	Yet another 68000 assembler
//

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <alloca.h>
#include <stdlib.h>
#include <stdint.h>

//
//	GLOBAL PROGRAM DEFINITIONS
//	==========================
//


//
//	Define some helper values and types.
//
typedef int		bool;		// basic T/F value
typedef uint8_t		byte;		// 8 bit unsigned type
typedef uint16_t	word;		// 16 bit unsigned type
typedef uint32_t	dword;		// 32 bit unsigned type

//
//	The following type is used for expression evaulation.
//
//	While this can be just another 32 bit value, where possible
//	this should be a signed type of greater than 32 bits.  This is
//	to facilitate the handling of signed and unsigned 32 bit values
//	without ambiguity.
//
typedef long int	number;

//
//	Provide some printf format strings to generate correct and
//	consistent output for each of the above define types.
//
#define BOOL_FORMAT	"%d"
#define BYTE_FORMAT	"%02X"
#define WORD_FORMAT	"%04X"
#define DWORD_FORMAT	"%08X"
#define NUMBER_FORMAT	"%ld"

#define ERROR		(-1)

#define TRUE		(0==0)
#define FALSE		(0==1)

#define EOS		'\0'

#define ZERO		'0'
#define NINE		'9'

#define PERIOD		'.'
#define PLUS		'+'
#define MINUS		'-'
#define OPAREN		'('
#define CPAREN		')'
#define COMMA		','
#define DOLLAR		'$'
#define PERCENT		'%'
#define HASH		'#'
#define COLON		':'
#define SEMICOLON	';'
#define ASTERIX		'*'
#define SLASH		'/'
#define UNDERSCORE	'_'
#define QUOTE		'\''
#define QUOTES		'"'
#define ESCAPE		'\\'
#define AMPERSAND	'&'
#define BAR		'|'
#define HAT		'^'
#define BANG		'!'

//
//	Constants impacting program operation.
//
//	BUFFER			The limiting size of an input line.
//
//	DIRECTIVE_ARGS		The maximum number of arguments an assembler
//				directive can have.
//
//	INSTRUCTION_ARGS	The maximum number of agruments an assembler
//				instruction can have.
//
//	MAXIMUM_VERIFICATIONS	The maximum number of times the assembler
//				tries to get the labels to settle.
//
#define BUFFER			200
#define DIRECTIVE_ARGS		8
#define INSTRUCTION_ARGS	2
#define MAXIMUM_VERIFICATIONS	4

//
//	Syntactic sugar for memory allocations and function indirection.
//
#define NEW(t)		((t *)malloc(sizeof(t)))
#define STACK(t)	((t *)alloca(sizeof(t)))
#define STACK_N(t,n)	((t *)alloca(sizeof( t )*(n)))
#define SPACE(n)	((char *)alloca((n)))
#define FUNC(a)		(*(a))

//
//	Word splitting macros
//
#define H(v)		(((v)>>8)&0xff)
#define L(v)		((v)&0xff)

//
//	DWord splitting macros
//	(for MSB ordering)
//
#define HW(v)		(((v)>>16)&0xffff)
#define LW(v)		((v)&0xffff)
//
#define DW0(v)		(((v)>>24)&0xff)
#define DW1(v)		(((v)>>16)&0xff)
#define DW2(v)		(((v)>>8)&0xff)
#define DW3(v)		((v)&0xff)

//
//	Bitty stuff.
//
#define BIT(n)		(1<<(n))
#define MASK(n)		(BIT(n)-1)

//
//	Declare two values that will help with the
//	address managment.
//
#define FIRST_ADDRESS	((dword)0x00000000)
#define LAST_ADDRESS	((dword)0xffffffff)

//
//	Numerical limits
//	================
//
#define MIN_S_BYTE	((number)-128)
#define MAX_S_BYTE	((number)127)
#define MIN_U_BYTE	((number)0)
#define MAX_U_BYTE	((number)255)

#define MIN_S_WORD	((number)-32768)
#define MAX_S_WORD	((number)32767)
#define MIN_U_WORD	((number)0)
#define MAX_U_WORD	((number)65535)

#define MIN_S_DWORD	((number)-2147483648)
#define MAX_S_DWORD	((number)2147483647)
#define MIN_U_DWORD	((number)0)
#define MAX_U_DWORD	((number)4294967295)

//
//	Program Option Flags
//	====================
//

//
//	Define a set of flags that are used to configure
//	and control the assembler.  These are OR'd together
//	into the option_flags variable.
//
#define OPTIONS_NONE		000000
//
#define OPTION_68000		0000001
#define OPTION_68008		0000002
#define OPTION_68010		0000004
#define OPTION_68020		0000010
#define OPTION_68030		0000020
#define OPTION_68881		0000040
#define CPU_MASK		(OPTION_68000|OPTION_68008|OPTION_68010|OPTION_68020|OPTION_68030)
//
#define DISPLAY_TEXT		0000100
#define DISPLAY_INTEL		0000200
#define DISPLAY_MOTOROLA	0000400
#define DISPLAY_NOTHING		0001000
#define DISPLAY_LISTING		0002000
#define DISPLAY_MASK		(DISPLAY_TEXT|DISPLAY_INTEL|DISPLAY_MOTOROLA|DISPLAY_NOTHING|DISPLAY_LISTING)
#define DISPLAY_STDOUT		0004000
//
#define DISPLAY_SYMBOLS		0010000
#define DISPLAY_SECTIONS	0020000
#define DISPLAY___00400		0040000
//
#define DISPLAY_OPCODES		0100000
#define DISPLAY_HELP		0200000
#define DISPLAY_DEBUG		0400000

//
//	This is where the flags are combined.
//
static int option_flags = OPTIONS_NONE;

//
//	COMPILE TIME DEBUGGING
//	======================
//
//	Define ENABLE_DEBUG to include options
//	for including additional checking code
//	and optional debugging output messages.
//

#ifdef ENABLE_DEBUG
//
//	Define for debugging.
//
#define ASSERT(v)	do{ if(!(v)) { fprintf( stderr, "Assert Failed \"%s\" (%s:%d:%s).\n", #v, __FILE__, __LINE__, __FUNCTION__ ); abort(); }}while(0)
#define PRINT(a)	do{ if( option_flags & DISPLAY_DEBUG ) printf a; }while(0)

#else
//
//	Define for not debugging.
//
#define ASSERT(v)
#define PRINT(a)

#endif

//
//	The ABORT macro is always fully specified.  What is
//	the point of crashing without giving some indication
//	of why?
//
#define ABORT(m)	do{ fprintf( stderr, "Program Abort \"%s\" (%s:%d:%s).\n", (m), __FILE__, __LINE__, __FUNCTION__ ); abort(); }while(0)

//
//	GLOBALLY STATIC PASS TRACKING
//	=============================
//
//	The assembler pass indicates which process through the
// 	source code the assembler is currently on.
//
typedef enum {
	NOT_ASSEMBLING,
	DATA_GATHERING,
	DATA_VERIFICATION,
	CODE_GENERATION
} pass_mode;

static pass_mode assembler_pass = NOT_ASSEMBLING;

//
//	ASSEMBLY LANGUAGE CONSTRUCTS
//	============================
//

//
//	Define enumeration to capture all of the possible
//	addressing modes.
//
//	Nomenclature:
//
//		Dn		Data register D0 .. D7
//		An		Address register A0 .. A7
//		(?)		Indirect memory access
//		(?)+		Address register post increment after indirect access
//		-(?)		Address register pre-decrement before indirect access
//		d8(?)		8 bit address displacement
//		d16(?)		16 bit address displacement
//		Rm.s		Generic register as either Word or Long size.
//		a16		Absolute 16 bit memory address
//		a32		Absolute 32 bit memory address
//		#i		An immediate value in byte, word or long sizes
//
typedef enum {
	NO_ARGUMENT		= 000000000000,	// Argument should be empty
	//
	//	The Effective Address arguments
	//
	EA_DREG			= 000000000001,	// EA 000: Dn, Data Register Direct
	EA_AREG			= 000000000002,	// EA 001: An, Address Register Direct
	EA_AREG_IND		= 000000000004,	// EA 010: (An), Address Register Indirect
	EA_AREG_IND_INC		= 000000000010,	// EA 011: (An)+, Address Register Indirect with post-increment
	EA_AREG_DEC_IND		= 000000000020,	// EA 100: -(An), Address Register Indirect with pre-decrement
	EA_AREG_IND_DISP	= 000000000040,	// EA 101: d16(An), Address Register Indirect with 16 bit displacement
	EA_AREG_IND_DISP_REG	= 000000000100,	// EA 110: d8(An,Rm.s), Address Register Indirect with 8 bit displacement and Register offset
	EA_ABS_SHORT_IND	= 000000000200,	// EA 111 000: a16, absolute indirect 16 bit address
	EA_ABS_LONG_IND		= 000000000400,	// EA 111 001: a32, absolute indirect 32 bit address
	EA_PC_IND_DISP		= 000000001000,	// EA 111 010: d16(PC), PC Indirect with 16 bit displacement
	EA_PC_IND_DISP_REG	= 000000002000,	// EA 111 011: d8(PC,Rm.s), PC Indirect with 8 bit displacement and Register offset
	EA_IMMEDIATE		= 000000004000,	// EA 111 100: #i, an immediate value in byte, word or long form (used as source only)
	EA_STATUS_REG		= 000000010000,	// EA 111 100: SR, The status register (used as destination only)
	EA_FLAGS_REG		= 000000020000,	// EA 111 100: CCR, The condition codes register (used as destination only)
	//
	//	The 'specialist' arguments which are
	//	not classified as Effective Address
	//	arguments and are used in specific
	//	opcodes.
	//
	REGISTER_LIST		= 000000040000,	// A specific argument type for the MOVEM opcode.
	ABS_ADDRESS		= 000000100000,	// Argument is an absolute address
	ARG__UNUSED__		= 000000200000,	// Unused argument flag
	NUM_DREG		= 000000400000,	// Number: Dn, Data Register Direct
	NUM_AREG		= 000001000000,	// Number: An, Address Register Direct
	NUM_AREG_DEC_IND	= 000002000000,	// Number: -(An), Address Register Indirect with pre-decrement
	NUM_AREG_IND_INC	= 000004000000,	// Number: (An)+, Address Register Indirect with post-increment
	NUM_IMMEDIATE_3BIT	= 000010000000,	// Number: #i, An unsigned bit number from 0 to 7
	NUM_IMMEDIATE_QUICK	= 000020000000,	// Number: #i, and unsigned number from 1 to 8
	NUM_IMMEDIATE_5BIT	= 000040000000,	// Number: #i, An unsigned bit number from 0 to 31
	NUM_IMMEDIATE_4		= 000100000000,	// Number: #i, An unsigned number from 0 to 15
	NUM_IMMEDIATE_8		= 000200000000,	// Number: #i, a number (signed or unsigned) in the 8 bit range
	NUM_IMMEDIATE_16	= 000400000000,	// Number: #i, a number (signed or unsigned) in the 16 bit range
	CONTROL_REG		= 001000000000	// One of a set of CPU control registers
} arg_type;

//
//	Define some common argument combinations that
//	can be used by the opcodes data file.
//
#define EA_SOURCE_ARG	(EA_DREG|EA_AREG|EA_AREG_IND|EA_AREG_IND_INC|EA_AREG_DEC_IND|EA_AREG_IND_DISP|EA_AREG_IND_DISP_REG|EA_ABS_SHORT_IND|EA_ABS_LONG_IND|EA_PC_IND_DISP|EA_PC_IND_DISP_REG|EA_IMMEDIATE)
#define EA_DEST_ARG	(EA_DREG|EA_AREG_IND|EA_AREG_IND_INC|EA_AREG_DEC_IND|EA_AREG_IND_DISP|EA_AREG_IND_DISP_REG|EA_ABS_SHORT_IND|EA_ABS_LONG_IND)
#define EA_MEMORY_ARG	(EA_AREG_IND|EA_AREG_IND_INC|EA_AREG_DEC_IND|EA_AREG_IND_DISP|EA_AREG_IND_DISP_REG|EA_ABS_SHORT_IND|EA_ABS_LONG_IND)
#define EA_ADDRESS_ARG	(EA_AREG_IND|EA_AREG_IND_DISP|EA_AREG_IND_DISP_REG|EA_ABS_SHORT_IND|EA_ABS_LONG_IND|EA_PC_IND_DISP|EA_PC_IND_DISP_REG)
//
//	EA combination especially for the MOVEM instruction
//
#define EA_MOVEM_ARG	(EA_AREG_IND|EA_AREG_IND_DISP|EA_AREG_IND_DISP_REG|EA_ABS_SHORT_IND|EA_ABS_LONG_IND)

//
//	If a parameter is appended after the opcode then
//	its bit position is marked with the APPEND value
//	telling the code to create additional opcode words.
//
#define APPEND		(-1)

//
//	If a supplied parameter is actually ignored, then
//	mark out with IGNORE.
//
#define IGNORE		(-2)

//
//	Use this macro to specify a bit position in the
//	word following the opcode.  This is characterised
//	as the auxiliary opcode in this program.
//
#define AUX(b)		((b)+16)

//
//	Testing for above and extracting bit from above.
//
#define IS_AUX(b)	((b)>=16)
#define AUX_BIT(b)	((b)-16)

//
//	Identifiers values will have some form of 'scope'
//	which outlines the context in which their value
//	has meaning.
//
typedef enum {
	NO_SCOPE	= 000000,	// When a value has been used but
					// nothing about what it might reflect
					// can be inferred
					
	SCOPE_S8	= 000001,	// An 8 bit signed value
	SCOPE_S16	= 000002,	// A 16 bit signed value
	SCOPE_S32	= 000004,	// A full 32 bit signed value
	
	SCOPE_U8	= 000010,	// An 8 bit unsigned value
	SCOPE_U16	= 000020,	// A 16 bit unsigned value
	SCOPE_U32	= 000040,	// A full 32 bit unsigned value

	SCOPE_ADRS	= 000100,	// An address value not associated
					// with a specific section
	SCOPE_TEXT	= 001000,	// An address in the TEXT section
	SCOPE_DATA	= 002000,	// An address in the DATA section
	SCOPE_BSS	= 004000,	// An address in the BSS section

	//
	//	Scope data pertinent to symbols:
	//
	SCOPE_IMPORT	= 010000,
	SCOPE_EXPORT	= 020000,

	//
	//	Joint scope specifications:
	//
	SCOPE_NUMERIC	= SCOPE_S8|SCOPE_S16|SCOPE_S32|SCOPE_U8|SCOPE_U16|SCOPE_U32,
	SCOPE_ADDRESS	= SCOPE_ADRS|SCOPE_TEXT|SCOPE_DATA|SCOPE_BSS
} scope;

//
//	Opcodes are optionally (commonly) size modified.  The
//	following enumeration captures the sizing options.
//
//	When no size indication is provided (and the instruction
//	expects it) then Word is the assumed size.
//
typedef enum {
	NO_SIZE		= 00,			// No size specification
	//
	//	Individual size specifications..
	//
	SIZE_B		= 01,			// Byte 8 bit values
	SIZE_W		= 02,			// Word 16 bit values
	SIZE_L		= 04,			// Long 32 bit values
	//
	//	Joint size specifications..
	//
	SIZE_WL		= SIZE_W|SIZE_L,
	SIZE_BWL	= SIZE_B|SIZE_W|SIZE_L
} sizing;

//
//	Define common sets of sizes required in the instruction set
//

//
//	Return the size (in bytes) of the size provided.  If multiple
//	bits are set, returns biggest.
//
static int get_size( sizing z ) {
	if( z & SIZE_L ) return( 4 );
	if( z & SIZE_W ) return( 2 );
	if( z & SIZE_B ) return( 1 );
	return( 0 );
}

//
//	provide basic scoping information on a sizing value.
//
static scope scope_size( sizing z ) {
	scope		s;

	s = NO_SCOPE;
	if( z & SIZE_L ) s |= ( SCOPE_S8 | SCOPE_U8 | SCOPE_S16 | SCOPE_U16 | SCOPE_S32 | SCOPE_U32 | SCOPE_ADRS );
	if( z & SIZE_W ) s |= ( SCOPE_S8 | SCOPE_U8 | SCOPE_S16 | SCOPE_U16 );
	if( z & SIZE_B ) s |= ( SCOPE_S8 | SCOPE_U8 );
	return( s );
}

//
//	provide basic scoping information on a dword value.
//
static scope scope_value( number val ) {
	scope	s;

	s = 0;
	if( val < 0 ) {
		if( val >= MIN_S_BYTE ) s |= SCOPE_S8;
		if( val >= MIN_S_WORD ) s |= SCOPE_S16;
		if( val >= MIN_S_DWORD ) s |= SCOPE_S32;
	}
	else {
		if( val <= MAX_S_BYTE ) s |= SCOPE_S8;
		if( val <= MAX_U_BYTE ) s |= SCOPE_U8;
		if( val <= MAX_S_WORD ) s |= SCOPE_S16;
		if( val <= MAX_U_WORD ) s |= SCOPE_U16;
		if( val <= MAX_S_DWORD ) s |= SCOPE_S32;
		if( val <= MAX_U_DWORD ) s |= SCOPE_U32 | SCOPE_ADRS;
	}
	return( s );
}

//
//	This is the enumeration capturing any specific opcode
//	encoding actions that cannot captured in the table.
//
typedef enum {
	NO_FLAGS		= 00000,	// Nothing extra required
	FLAG_REV_ARG_1		= 00001,	// Reverse bit order of argument 1
	FLAG_COMP_SIZE		= 00002,	// The size bits must be complemented
	FLAG_USP_ONLY		= 00004,	// Only the USP control register allowed
	FLAG_8_16_REL		= 00010,	// Argument is an 8 or 16 bit PC relative address
	FLAG_16_REL		= 00020,	// Argument is a 16 bit PC relative address
	FLAG_68000		= 00040,	// 68010 Instructions
	FLAG_68010		= 00100,	// 68010 Instructions
	FLAG_68020		= 00200,	// 68020 Instructions
	FLAG_68030		= 00400,	// 68030 Instructions
	FLAG_68881		= 01000		// FPU Instructions
} opflags;

//
//	Define TWO classes:
//
//	o	The first captures a specific instance (one of possibly a
//		number) that an opcode mnemonic can be used.  These linked
//		together as a linked list.
//
//	o	The second to facilitate more rapid lookup/conversion from
//		an opcode mnemonic to the list of description records which
//		details the individual ways that mnemonic can be used. These
//		formulated into binary tree.
//	
//
typedef struct _opcode_ {
	//
	//	Mnemonic variant:
	//
	sizing		size;			// Is opcode sized?
	arg_type	arg1,			// The arguments to the opcode
			arg2;
	//
	//	Construction components.
	//
	word		basecode,		// The base 16 bit opcode
			auxcode;		// The start value for extended data
						// This is only non zero for the larger
						// opcodes
	int		sizebit,		// bit position of size code if used
			off1,			// The bit offsets for the arguments
			off2;
	//
	//	Capture any special instructions that this opcode
	//	requires.  This is normally 0, NO_FLAGS.
	//
	opflags		flags;
	//
	//	Next variant of this mnemonic.
	//
	struct _opcode_
			*next;
} opcode;
typedef struct _mnemonic_ {
	//
	//	The mnemonic name:
	//
	char		*name;			// The basic name of the opcode
	//
	//	The list of valid opcodes:
	//
	opcode		*opcodes;		// The list of varients
	//
	//	The tree search structure.
	//
	struct _mnemonic_
			*before,
			*after;
} mnemonic;


//
//	GENERAL SUPPORT ROUTINES
//	========================
//


//
//	Routine to do caseless string comparison between a EOS
//	terminated fixed string (with) and a buffer located
//	length delimited input string (check).
//
static int compare_word( char *check, int len, char *with ) {
	int	c, w;
	
	while( len-- ) {
		if(( c = toupper( *check++ )) != ( w = toupper( *with++ ))) return( c - w );
	}
	return( - *with );
}

//
//	Character to value conversion.
//
static int digit_value( char digit ) {
	if(( digit >= '0' )&&( digit <= '9' )) return( digit - '0' );
	if(( digit >= 'a' )&&( digit <= 'f' )) return(( digit - 'a' ) + 10 );
	if(( digit >= 'A' )&&( digit <= 'F' )) return(( digit - 'A' ) + 10 );
	return( ERROR );
}

//
//	MOTOROLA 680x0 INSTRUCTIONS
//	===========================
//

//
//	Define a data structure that contains the opcodes for the
//	Motorola 68000 instruction set.
//
//	This is a generated section of C source code produced by
//	the 'op68k' executed against the input data in the file
//	'ops68k.ops' and placed into the header file 'ops68k.h'.
//
//	The starting point for the assembly of a line of code
//	is in the symbol 'ROOT_NODE' which gives the name of the
//	object containing the first mnemonic record.  This will
//	probably need accessing as a pointer, thus: &ROOT_NODE.
//
#include "ops68k.h"

//
//	This is the routine which searches the mnemonic tree for
//	the start of an opcode list, and returns either a valid
//	pointer address or NULL on failure.
//
static opcode *find_opcode( char *find, int len ) {
	mnemonic *look;
	
	look = &ROOT_NODE;
	while( look != NULL ) {
		int	cmp;
		
		if(( cmp = compare_word( find, len, look->name )) == 0 ) {
			PRINT(( "Found opcode '%s'\n", look->name ));
			return( look->opcodes );
		}
		look = ( cmp < 0 )? look->before: look->after;
	}
	return( NULL );
}

//
//	OPCODE TABLE DISPLAY ROUTINES
//	=============================
//

//
//	For the assistance of debugging and extensions
//	the dump_opcodes routine (and friedns) will output
//	the whole table of recognised opcodes to the
//	standard output.
//
typedef struct {
	arg_type	arg;
	char		*desc;
} arg_desc;
static arg_desc arg_types[] = {
	{ EA_DREG,			"Dn"		},
	{ EA_AREG,			"An"		},
	{ EA_AREG_IND,			"(An)"		},
	{ EA_AREG_IND_INC,		"(An)+"		},
	{ EA_AREG_DEC_IND,		"-(An)"		},
	{ EA_AREG_IND_DISP,		"d16(An)"	},
	{ EA_AREG_IND_DISP_REG,		"d8(An,Rm.s)"	},
	{ EA_ABS_SHORT_IND,		"a16"		},
	{ EA_ABS_LONG_IND,		"a32"		},
	{ EA_PC_IND_DISP,		"d16(PC)"	},
	{ EA_PC_IND_DISP_REG,		"d8(PC,Rm.s)"	},
	{ EA_IMMEDIATE,			"#i"		},
	{ EA_STATUS_REG,		"SR"		},
	{ EA_FLAGS_REG,			"CCR"		},
	{ REGISTER_LIST,		"RegList"	},
	{ ABS_ADDRESS,			"adrs"		},
	{ NUM_DREG,			"Dn"		},
	{ NUM_AREG,			"An"		},
	{ NUM_AREG_DEC_IND,		"-(An)"		},
	{ NUM_AREG_IND_INC,		"(An)+"		},
	{ NUM_IMMEDIATE_3BIT,		"#0-7"		},
	{ NUM_IMMEDIATE_QUICK,		"#1-8"		},
	{ NUM_IMMEDIATE_5BIT,		"#0-31"		},
	{ NUM_IMMEDIATE_4,		"#0-15"		},
	{ NUM_IMMEDIATE_8,		"#i8"		},
	{ NUM_IMMEDIATE_16,		"#i16"		},
	{ CONTROL_REG,			"ControlReg"	},
	{ 0 }
};
static void _dump_argument( arg_type type ) {
	arg_desc	*p;
	bool		z;

	p = arg_types;
	z = FALSE;
	printf( "[" );
	while( p->arg ) {
		if( type & p->arg ) {
			printf(( z? "|%s": "%s" ), p->desc );
			z = TRUE;
		}
		p++;
	}
	printf( "]" );
}
static void _dump_opcodes( mnemonic *at ) {
	if( at ) {
		_dump_opcodes( at->before );
		for( opcode *op = at->opcodes; op != NULL; op = op->next ) {
			printf( "%s", at->name );
			if( op->size != NO_SIZE ) {
				printf( ".[" );
				if( op->size & SIZE_B	) printf( "B" );
				if( op->size & SIZE_W	) printf( "W" );
				if( op->size & SIZE_L	) printf( "L" );
				printf( "]" );
			}
			printf( "\t" );
			if( op->arg1 != NO_ARGUMENT ) {
				_dump_argument( op->arg1 );
			}
			if( op->arg2 != NO_ARGUMENT ) {
				printf( ", " );
				_dump_argument( op->arg2 );
			}
			printf( " -> %04X", op->basecode );
			if( op->auxcode ) printf( " %04X", op->auxcode );
			printf( "\n" );
		}
		_dump_opcodes( at->after );
	}
}
static void dump_opcodes( void ) {
	_dump_opcodes( &ROOT_NODE );
}

//

//	THE DATA OUTPUT SYSTEM
//	======================
//
//	A system of routines that provide a unified API for outputting
//	a range of data formats both computer and human accessible.
//

//
//	Define some generic API variables which the specific
//	output implementations can initialise and use as appropiate.
//
#define MAX_OUTPUT_BUFFER 16
static byte output_buffer[ MAX_OUTPUT_BUFFER ];
static int buffered_output = 0;
static dword buffered_address = 0;
static FILE *output_file = NULL;

//
//	This will be structured around a data structure containing
//	pointers to the routines which implement the individual
//	API routines.
//

typedef struct {
	void	FUNC( init_output )( char *source );
	void	FUNC( next_line )( int line, char *code );
	void	FUNC( set_address )( dword adrs );
	void	FUNC( set_start )( dword adrs );
	void	FUNC( add_byte )( byte data );
	void	FUNC( end_output )( void );
} output_api;

//
//	No Output
//	---------
//
//	A dummy output system which does not output
//	anything.
//
static void _null_init_output( char *source ) {
}
static void _null_next_line( int line, char *code ) {
}
static void _null_set_address( dword adrs ) {
}
static void _null_set_start( dword adrs ) {
}
static void _null_add_byte( byte data ) {
}
static void _null_end_output( void ) {
}

static output_api _null_output_api = {
	_null_init_output,
	_null_next_line,
	_null_set_address,
	_null_set_start,
	_null_add_byte,
	_null_end_output
};

//
//	The listing output system.
//	--------------------------
//
//	For source code listing these are reuqired.  The buffer is
//	allocated only if required.
//
//	Ensure MAX_SOURCE_OUTPUT_BUFFER <= MAX_OUTPUT_BUFFER
//
#define MAX_SOURCE_CODE_BUFFER 80
#define MAX_SOURCE_OUTPUT_BUFFER 6
static int _listing_this_line = 0;
static char _listing_source_code[ MAX_SOURCE_CODE_BUFFER+1 ];  // +1 for EOS

static void _listing_init_output( char *source ) {
	buffered_output = 0;
	buffered_address = 0;
	_listing_this_line = 0;
}
static void _listing_flush_output( void ) {
	int	i;
	
	//
	//	The output format will be:
	//
	//	address		%04X
	//	space
	//	data		%02X		Repeat to fixed limit
	//	source		|...		To end of line
	//
	if(( buffered_output == 0 )&&( _listing_this_line == 0 )) return;
	if( buffered_output ) {
		fprintf( output_file, DWORD_FORMAT" ", buffered_address );
		for( i = 0; i < buffered_output; fprintf( output_file, BYTE_FORMAT, output_buffer[ i++ ]));
		while( i++ < MAX_SOURCE_OUTPUT_BUFFER ) fprintf( output_file, "  " );
		buffered_address += buffered_output;
		buffered_output = 0;
	}
	else {
		fprintf( output_file, "         " );
		for( i = 0; i < MAX_SOURCE_OUTPUT_BUFFER; i++ ) fprintf( output_file, "  " );
	}
	if( _listing_this_line ) {
		fprintf( output_file, " |%4d|%s\n", _listing_this_line, _listing_source_code );
		_listing_this_line = 0;
	}
	else {
		fprintf( output_file, "\n" );
	}
}
static void _listing_next_line( int line, char *code ) {
	ASSERT( line > 0 );
	
	//
	//	Flush out anything we have pending..
	//
	_listing_flush_output();
	//
	//	Save this line for later..
	//
	strncpy( _listing_source_code, code, MAX_SOURCE_CODE_BUFFER );
	_listing_source_code[ MAX_SOURCE_CODE_BUFFER ] = EOS;
	_listing_this_line = line;
}
static void _listing_set_address( dword adrs ) {
	if(( buffered_address + buffered_output ) != adrs ) {
		if( buffered_output ) _listing_flush_output();
		buffered_address = adrs;
	}
}
static void _listing_set_start( dword adrs ) {
}
static void _listing_add_byte( byte data ) {
	output_buffer[ buffered_output++ ] = data;
	if( buffered_output >= MAX_SOURCE_OUTPUT_BUFFER ) _listing_flush_output();
}

static void _listing_end_output( void ) {
	_listing_flush_output();
}

static output_api _listing_output_api = {
	_listing_init_output,
	_listing_next_line,
	_listing_set_address,
	_listing_set_start,
	_listing_add_byte,
	_listing_end_output
};

//
//	The hexadecimal output system.
//	------------------------------
//
static void _hexadecimal_init_output( char *source ) {
	buffered_output = 0;
	buffered_address = 0;
}
static void _hexadecimal_next_line( int line, char *code ) {
}
static void _hexadecimal_flush_output( void ) {
	if( buffered_output ) {
		fprintf( output_file, DWORD_FORMAT, buffered_address );
		for( int i = 0; i < buffered_output; fprintf( output_file, " " BYTE_FORMAT, output_buffer[ i++ ]));
		fprintf( output_file, "\n" );
		buffered_address += buffered_output;
		buffered_output = 0;
	}
}
static void _hexadecimal_set_address( dword adrs ) {
	if(( buffered_address + buffered_output ) != adrs ) {
		if( buffered_output ) _hexadecimal_flush_output();
		buffered_address = adrs;
	}
}
static void _hexadecimal_set_start( dword adrs ) {
}
static void _hexadecimal_add_byte( byte data ) {
	output_buffer[ buffered_output++ ] = data;
	if( buffered_output >= MAX_OUTPUT_BUFFER ) _hexadecimal_flush_output();
}

static void _hexadecimal_end_output( void ) {
	_hexadecimal_flush_output();
}

static output_api _hexadecimal_output_api = {
	_hexadecimal_init_output,
	_hexadecimal_next_line,
	_hexadecimal_set_address,
	_hexadecimal_set_start,
	_hexadecimal_add_byte,
	_hexadecimal_end_output
};

//
//	The Motorola S record output system.
//	------------------------------------
//
static bool _motorola_have_start = FALSE;
static dword _motorola_start_address = FIRST_ADDRESS;
static dword _motorola_srec_count = 0;

static void _motorola_init_output( char *source ) {
	int	l;
	byte	s;
	
	buffered_output = 0;
	buffered_address = 0;
	_motorola_have_start = FALSE;
	_motorola_start_address = FIRST_ADDRESS;
	_motorola_srec_count = 0;
	//
	//	Send out header record.
	//
	//	For the moment This is not following the header format:
	//
	//		20 bytes	Module name
	//		2 bytes		Version
	//		2 bytes		Revision
	//		0-36 bytes	Comment
	//
	s = 0;
	l = strlen( source );
	fprintf( output_file, "S0%02X0000", l + 3 );	// 3 accounts for address '0000' + checksum 'nn'
	for( int i = 0; i < l; i++ ) {
		s += source[ i ];
		fprintf( output_file, "%02X", source[ i ]);
	}
	fprintf( output_file, "%02X\r\n", ~s & 0xff );
}
static void _motorola_next_line( int line, char *code ) {
}
static void _motorola_flush_output( void ) {
	if( buffered_output ) {
		byte	s;

		s = H( buffered_address ) + L( buffered_address ) + buffered_output;
		fprintf( output_file, "S3%02X%08X", 5 + buffered_output, buffered_address );
		for( int i = 0; i < buffered_output; i++ ) {
			s += output_buffer[ i ];
			fprintf( output_file, "%02X", output_buffer[ i ]);
		}
		fprintf( output_file, "%02X\r\n", ~s & 0xff );
		buffered_address += buffered_output;
		buffered_output = 0;
		_motorola_srec_count++;
	}
}
static void _motorola_set_address( dword adrs ) {
	if(( buffered_address + buffered_output ) != adrs ) {
		if( buffered_output ) _motorola_flush_output();
		buffered_address = adrs;
	}
}
static void _motorola_set_start( dword adrs ) {
	if( _motorola_have_start ) {
		fprintf( stderr, "Duplicate START address specified\n" );
	}
	else {
		_motorola_have_start = TRUE;
		_motorola_start_address = adrs;	
	}
}
static void _motorola_add_byte( byte data ) {
	output_buffer[ buffered_output++ ] = data;
	if( buffered_output >= MAX_OUTPUT_BUFFER ) _motorola_flush_output();
}

static void _motorola_end_output( void ) {
	byte	s;
	
	_motorola_flush_output();
	//
	//	The end records:
	//
	if( _motorola_srec_count <= MAX_U_WORD ) {
		s = H( _motorola_srec_count ) + L( _motorola_srec_count ) + 3;
		fprintf( output_file, "S503%04X%02X\r\n", _motorola_srec_count, ~s & 0xff );
	}
	else {
		s = DW2( _motorola_srec_count ) + DW1( _motorola_srec_count ) + DW0( _motorola_srec_count ) + 4;
		fprintf( output_file, "S604%06X%02X\r\n", ( _motorola_srec_count & 0xFFFFFF ), ~s & 0xff );
	}
	s = DW3( _motorola_start_address ) + DW2( _motorola_start_address ) + DW1( _motorola_start_address ) + DW0( _motorola_start_address ) + 5;
	fprintf( output_file, "S905%08X%02X\r\n", _motorola_start_address, ~s & 0xff );
}

static output_api _motorola_output_api = {
	_motorola_init_output,
	_motorola_next_line,
	_motorola_set_address,
	_motorola_set_start,
	_motorola_add_byte,
	_motorola_end_output
};

//
//	The Intel Hex record output system.
//	-----------------------------------
//
static dword _intel_linear_adrs = FIRST_ADDRESS;	// bottom word should always be 0x0000.
static dword _intel_start_address = FIRST_ADDRESS;	// Execution start address.

//
//	Output a DATA record.
//
static void _intel_00_record( word adrs, byte *buffer, int len ) {
	byte	s;

	PRINT(( "Output %d byte(s) at address "WORD_FORMAT".\n", len, adrs ));
	//
	//	len	HAdrs		LAdrs		Type
	s =	len +	H( adrs ) +	L( adrs ) +	0x00;
	//
	fprintf( output_file, ":"BYTE_FORMAT""WORD_FORMAT"00", len, adrs );
	for( int i = 0; i < len; i++ ) {
		s += buffer[ i ];
		fprintf( output_file, BYTE_FORMAT, buffer[ i ]);
	}
	fprintf( output_file, BYTE_FORMAT"\n", -s & 0xff );
}
//
//	Set a the LINEAR address top word.
//
static void _intel_04_record( word adrs ) {
	byte	s;

	PRINT(( "Set high word of linear address to "WORD_FORMAT".\n", adrs ));
	//	len	HAdrs	LAdrs	Type	HLinear		LLinear
	s =	2 +	0x00 +	0x00 +	0x04 +	H( adrs ) +	L( adrs );
	//
	fprintf( output_file, ":02000004"WORD_FORMAT""BYTE_FORMAT"\n", adrs, -s & 0xff );
}
//
//	Set the START execute from address.
//
static void _intel_05_record( dword adrs ) {
	byte	s;
	
	//	len	HAdrs	LAdrs	Type	Start_Address____
	s =	4 +	0x00 +	0x00 +	0x05 +	DW0( adrs ) +	DW1( adrs ) +	DW2( adrs ) +	DW3( adrs );
	//
	fprintf( output_file, ":04000005"DWORD_FORMAT""BYTE_FORMAT"\n", adrs, -s & 0xff );
}

static void _intel_init_output( char *source ) {
	buffered_output = 0;
	buffered_address = 0;
	_intel_linear_adrs = FIRST_ADDRESS;
	_intel_start_address = FIRST_ADDRESS;
	_intel_04_record( HW( _intel_linear_adrs ));
}
static void _intel_next_line( int line, char *code ) {
}
static bool _intel_flush_output( void ) {
	//
	//	Returns TRUE if there is still some data
	//	left in the buffer to be flushed.
	//
	if( buffered_output ) {
		dword	a;

		ASSERT( buffered_output <= MAX_OUTPUT_BUFFER );
		ASSERT( LW( _intel_linear_adrs ) == 0 );
		ASSERT( buffered_address >= _intel_linear_adrs );
		ASSERT( buffered_address <= ( _intel_linear_adrs | 0xFFFF ));

		//
		//	We need to be  mindful of "wrapping" the address from
		//	0xffff back to 0x0000.  Therefore we need to establish
		//	how many bytes there are between our current buffered_address
		//	and the address where the low word wraps to 0.  Also
		//	need to be mindful of the high word wrapping too.
		//
		a = (( _intel_linear_adrs | 0xFFFF ) - buffered_address ) + 1;
		
		if( buffered_output > a ) {
			//
			//	Need to output some of this line, then output
			//	a new linear address record.
			//
			_intel_00_record( LW( buffered_address ), output_buffer, a );
			buffered_address += a;
			buffered_output -= a;
			memcpy( output_buffer, output_buffer+a, buffered_output );

			ASSERT( buffered_address >= ( _intel_linear_adrs + 0x00010000 ));
			
			_intel_linear_adrs = buffered_address & 0xFFFF0000;
			_intel_04_record( HW( _intel_linear_adrs ));
			//
			//	Admit buffer not actually empty.
			//
			return( TRUE );
		}
		//
		//	All good, address changes kept inside the low word.
		//
		_intel_00_record( LW( buffered_address ), output_buffer, buffered_output );
		buffered_address += buffered_output;
		buffered_output = 0;
		//
		//	Edge case where the output record exactly meets the
		//	boundary to a new linear address range.
		//
		if( HW( buffered_address ) != HW( _intel_linear_adrs )) {
			_intel_linear_adrs = buffered_address & 0xFFFF0000;
			_intel_04_record( HW( _intel_linear_adrs ));
		}
	}
	//
	//	Done!
	//
	return( FALSE );
}
static void _intel_set_address( dword adrs ) {
	if(( buffered_address + buffered_output ) != adrs ) {
		while( _intel_flush_output());
		buffered_address = adrs;
		if( HW( buffered_address ) != HW( _intel_linear_adrs )) {
			_intel_linear_adrs = buffered_address & 0xFFFF0000;
			_intel_04_record( HW( _intel_linear_adrs ));
		}
	}
}
static void _intel_set_start( dword adrs ) {
	_intel_start_address = adrs;
}
static void _intel_add_byte( byte data ) {
	output_buffer[ buffered_output++ ] = data;
	if( buffered_output >= MAX_OUTPUT_BUFFER ) _intel_flush_output();
}

static void _intel_end_output( void ) {
	while( _intel_flush_output());
	_intel_05_record( _intel_start_address );
	fprintf( output_file, ":00000001FF\n" );
}

static output_api _intel_output_api = {
	_intel_init_output,
	_intel_next_line,
	_intel_set_address,
	_intel_set_start,
	_intel_add_byte,
	_intel_end_output
};

//
//	The output API entry points
//	---------------------------

//
//	This is the entry point into the output API, we defaullt to
//	the null output option to ensure something is available.
//
static output_api *output_routine = &_null_output_api;

static char *init_output( char *source ) {
	char	*suffix;

	suffix = NULL;
	switch( option_flags & DISPLAY_MASK ) {
		case DISPLAY_TEXT: {
			output_routine = &_hexadecimal_output_api;
			suffix = ".text";
			break;
		}
		case DISPLAY_INTEL: {
			output_routine = &_intel_output_api;
			suffix = ".hex";
			break;
		}
		case DISPLAY_MOTOROLA: {
			output_routine = &_motorola_output_api;
			suffix = ".srec";
			break;
		}
		case DISPLAY_LISTING: {
			output_routine = &_listing_output_api;
			suffix = ".text";
			break;
		}
		case DISPLAY_NOTHING: {
			output_routine = &_null_output_api;
			break;
		}
	}
	if(( suffix == NULL )||( option_flags & DISPLAY_STDOUT )) {
		output_file = stdout;
	}
	else {
		char	*fname, *dot;

		fname = STACK_N( char, ( strlen( source ) + strlen( suffix ) + 1 ));
		strcpy( fname, source );
		if(( dot = strrchr( fname, PERIOD )) != NULL ) *dot = EOS;
		strcat( fname, suffix );
		if(( output_file = fopen( fname, "w" )) == NULL ) {
			return( "Unable to open output file" );
		}
	}
	FUNC( output_routine->init_output )( source );
	return( NULL );
}
static void next_line( int line, char *code ) {
	FUNC( output_routine->next_line )( line, code );
}
static void set_address( dword adrs ) {
	FUNC( output_routine->set_address )( adrs );
}
static void set_start( dword adrs ) {
	FUNC( output_routine->set_start )( adrs );
}
static void add_byte( byte data ) {
	FUNC( output_routine->add_byte )( data );
}
static void end_output( void ) {
	FUNC( output_routine->end_output )();
	if(( output_file )&&(!( option_flags & DISPLAY_STDOUT ))) {
		fclose( output_file );
		output_file = NULL;
	}
}

//
//	GLOBALLY STATIC IDENTIFIER DATABASE
//	===================================
//
 
//
//	Define a basic type for tracking identifiers,
//	their context and values.
//
typedef struct _identifier_ {
	//
	//	What we know about an identifier.
	//
	char		*name;
	bool		defined,
			import,
			export;
	dword		value;
	scope		where;
	//
	//	Links to other identifiers.
	//
	struct _identifier_
			*before,
			*after;
} identifier;
static identifier *identifier_database = NULL;

//
// 	Identifier Management Code
//	--------------------------
//

//
//	This is the basic routine for finding/creating an
//	identifier record.  Any query for an identifier
//	that doesn't find one, creates an empty record
//	for it and returns that.
//
static identifier *locate_identifier( char *name ) {
	identifier	**adrs, *look;
	int		i;
	
	ASSERT( name != NULL );
	
	adrs = &identifier_database;
	while(( look = *adrs ) != NULL ) {
		if(( i = strcmp( name, look->name )) == 0 ) return( look );
		adrs = ( i < 0 )? &( look->before ): &( look->after );
	}
	//
	//	No record found so make one and return it.
	//
	PRINT(( "New identifier '%s'.\n", name ));
	look = NEW( identifier );
	look->name = strdup( name ); 
	look->defined = FALSE;
	look->import = FALSE;
	look->export = FALSE;
	look->value = 0;
	look->where = NO_SCOPE;
	look->before = NULL;
	look->after = NULL;
	*adrs = look;
	return( look );
}

static char *import_identifier( char *name ) {
	if( assembler_pass == DATA_GATHERING ) {
		identifier	*id;

		id = locate_identifier( name );

		ASSERT( id );

		if( id->defined ) return( "Identifier already defined" );
		if( id->import ) return( "Identifier already imported" );
		
		ASSERT( !id->export );

		id->defined = TRUE;
		id->import = TRUE;
		id->where = SCOPE_ADRS;
	}
	return( NULL );
}

static char *export_identifier( char *name ) {
	if( assembler_pass == DATA_VERIFICATION ) {
		identifier	*id;

		id = locate_identifier( name );

		ASSERT( id );

		if(!( id->defined )) return( "Identifier not defined" );
		if( id->import ) return( "Identifier is imported" );
		if( id->export )return( "Identifier already exported" );
		if(!( id->where & SCOPE_ADRS )) return( "Identifier not address" );
		
		id->export = TRUE;
	}
	return( NULL );
}

static bool get_ident_value( char *name, int len, scope *scope, number *value ) {
	identifier	*id;
	char		*str;
	
	ASSERT( name != NULL );
	ASSERT( len > 0 );
	ASSERT( value != NULL );
	
	str = SPACE( len + 1 );			// space on the stack
	memcpy( str, name, len );
	str[ len ] = EOS;
	
	id = locate_identifier( str );
	
	ASSERT( id != NULL );
	
	if( !id->defined ) {
		*value = 0;
		*scope = SCOPE_NUMERIC;
		return( assembler_pass == DATA_GATHERING );
	}
	
	*value = id->value;
	*scope = id->where;
	return( TRUE );
}

static char *set_ident_value( char *name, int len, scope from, dword value ) {
	identifier	*id;
	char		*str;
	
	ASSERT( name != NULL );
	ASSERT( len > 0 );
	
	str = SPACE( len + 1 );			// space on the stack
	memcpy( str, name, len );
	str[ len ] = EOS;
	
	id = locate_identifier( str );
	
	ASSERT( id != NULL );
	
	switch( assembler_pass ) {
		case DATA_GATHERING: {
			if( id->defined ) return( "Identifier redefined" );
			id->defined = TRUE;
			id->value = value;
			id->where = from;
			return( NULL );
		}
		case DATA_VERIFICATION: {
			//
			//	In the data verification step we allow
			//	the value of the identifier to be modified
			//	but its scope (and the fact that it is
			//	defined) must remain constant.
			//
			 
			ASSERT( id->defined );
			ASSERT( id->where == from );
			
			id->value = value;
			return( NULL );
		}
		case CODE_GENERATION: {
			//
			//	In this pass it all has to match
			//
			 
			ASSERT( id->defined );
			ASSERT( id->where == from );
			
			if( id->value != value ) return( "Identifier value inconsistent" );
			return( NULL );
		}
		default: {
			ABORT( "Invalid assembler pass" );
			break;
		}
	}
	return( NULL );
}

//
//	Conversion table giving human version of scope
//	information.
//
typedef struct {
	scope		contains;
	char		*desc;
} scope_desc;
static scope_desc scope_types[] = {
	{ SCOPE_S8,	"S8"	},
	{ SCOPE_U8,	"U8"	},
	{ SCOPE_S16,	"S16"	},
	{ SCOPE_U16,	"U16"	},
	{ SCOPE_S32,	"S32"	},
	{ SCOPE_U32,	"U32"	},
	{ SCOPE_ADRS,	"Adrs"	},
	{ SCOPE_TEXT,	"Text"	},
	{ SCOPE_DATA,	"Data"	},
	{ SCOPE_BSS,	"BSS"	},
	{ 0 }
};
//
//	Dump the identifiers record.
//
static void dump_identifiers_r( FILE *to, identifier *ptr ) {
	if( ptr != NULL ) {
		dump_identifiers_r( to, ptr->before );
		fprintf( to, "%20s ", ptr->name );
		if( ptr->defined ) {
			scope_desc	*s;

			if( ptr->import ) {
				fprintf( to, "= Import" );
			}
			else {
				fprintf( to, "= %6ld($"DWORD_FORMAT")", (long int)ptr->value, ptr->value );
				s = scope_types;
				while( s->contains ) {
					if( ptr->where & s->contains ) {
						fprintf( to, " %s", s->desc );
					}
					s++;
				}
				if( ptr->export ) fprintf( to, " Export" );
			}
			fprintf( to, "\n" );
		}
		else {
			fprintf( to, "Undefined\n" );
		}
		dump_identifiers_r( to, ptr->after );
	}
}
static void dump_identifiers( FILE *to ) {
	dump_identifiers_r( to, identifier_database );
}

//
// 	GLOBALLY STATIC SECTION MANAGEMENT
//	==================================
//
 
//
//	Enumerate the different sections the
//	assembler tracks.
//
typedef enum {
	TEXT_SECTION = 0,
	DATA_SECTION = 1,
	BSS_SECTION = 2
} section;
#define SECTIONS 3
#define SECTION_VALID(s)	(((s)>=TEXT_SECTION )&&((s)<=BSS_SECTION ))

//
//	Declare a structure which is used to track the different
//	sections which the assembly file chooses to use.
//
typedef struct _section_record_ {
	section		where;		// Where is this memory block?
	bool		relative,	// Is this a relative record?
			empty;		// Is this section empty (nothing allocated)?
	dword		start,		// What address does this start at?
			adrs,		// What is the next this section will use?
			finish;		// What is the last possible address of this section?
	struct _section_record_
			*next;
} section_record;

//
//	An array of 'addresses' which indicate the current
//	depth into a specific section, and what is the
//	current section we are operating in.
//
static section_record	*all_sections = NULL,
			**all_sections_tail = NULL,
			*recycle_sections = NULL,
			*section_ptr[ SECTIONS ],
			*current_section = NULL;
static bool		sections_locked = FALSE;

//
//	Return the current section as a scope value
//
static scope section_to_scope[ SECTIONS ] = { SCOPE_TEXT, SCOPE_DATA, SCOPE_BSS };
static scope section_scope( void ) {
	
	ASSERT( current_section );
	ASSERT( SECTION_VALID( current_section->where ));
	
	return( section_to_scope[ current_section->where ] | SCOPE_ADRS );
}

//
//	The section management routines
//
static void reset_sections( void ) {
	int		i;
	section_record	*p;

	switch( assembler_pass ) {
		//
		//	During the data gathering phase the sections
		//	are assumed to relative where all addresses
		//	within a section are an offset from an (as yet
		//	unspecified) absolute address.
		// 
		case DATA_GATHERING: {

			ASSERT( all_sections == NULL );
			ASSERT( all_sections_tail == NULL );
			ASSERT( recycle_sections == NULL );
			ASSERT( current_section == NULL );

			//
			//	Set up all new and empty relative sections.
			//
			all_sections = NULL;
			all_sections_tail = &all_sections;
			recycle_sections = NULL;
			sections_locked = FALSE;
			for( i = 0; i < SECTIONS; i++ ) {
				p = NEW( section_record );
				p->where = i;
				p->relative = TRUE;
				p->empty = TRUE;
				p->start = FIRST_ADDRESS;
				p->adrs = FIRST_ADDRESS;
				p->finish = LAST_ADDRESS;
				p->next = NULL;
				//
				*all_sections_tail = p;
				all_sections_tail = &( p->next );
				//
				section_ptr[ i ] = p;
			}
			//
			//	This is where we assume we start..
			//
			current_section = section_ptr[ TEXT_SECTION ];
			break;
		}
		//
		//	During the verification phase we should be
		//	able to 'replay' the section changes using the
		//	previously generated record.
		//
		//	We need to be prepared for changes in parameters
		//	as some section might be dynamically positioned
		//	and/or sized.
		//
		case DATA_VERIFICATION: {

			ASSERT( all_sections != NULL );
			ASSERT( all_sections_tail != NULL );
			ASSERT( recycle_sections == NULL );
			ASSERT( current_section != NULL );
			ASSERT( !sections_locked );

			//
			//	Find the init set of section records.  These
			//	must be the first records on the recycle list
			//	as these are in allocated section order.
			//
			recycle_sections = all_sections;
			all_sections = NULL;
			all_sections_tail = &all_sections;
			for( i = 0; i < SECTIONS; i++ ) {

				p = recycle_sections;
				recycle_sections = p->next;

				ASSERT( p != NULL );
				ASSERT(( p->where >= 0 )&&( p->where < SECTIONS ));
				ASSERT( p->relative );
				ASSERT( p->where == i );
				
				section_ptr[ i ] = p;
				p->adrs = p->start;
				p->next = NULL;
				//
				*all_sections_tail = p;
				all_sections_tail = &( p->next );
				//
			}
			current_section = section_ptr[ TEXT_SECTION ];
			break;
		}
		//
		//	After the data gathering and verification phases we
		//	should know exactly where each piece of section (be
		//	it code or data) is going to land.
		//
		//	Given this information it becomes possible to
		//	determine the nature of the memory map which the
		//	code and data needs to slot into.
		//
		case CODE_GENERATION: {

			ASSERT( all_sections != NULL );
			ASSERT( all_sections_tail != NULL );
			ASSERT( current_section != NULL );
			ASSERT( !sections_locked );

			//
			//	Code generation pass - lock all sections before
			//	doing anything else.
			//
			sections_locked = TRUE;
			//
			//	Reset the section again, but this time we lock
			//	things down by fixing the finish values to the
			//	last used address (if not empty).
			//
			recycle_sections = all_sections;
			all_sections = NULL;
			all_sections_tail = &all_sections;
			for( i = 0; i < SECTIONS; i++ ) {

				p = recycle_sections;
				recycle_sections = p->next;

				ASSERT( p != NULL );
				ASSERT( SECTION_VALID( p->where ));
				ASSERT( p->where == i );
				
				section_ptr[ i ] = p;
				if( !p->empty ) p->finish = p->adrs - 1;
				p->adrs = p->start;
				p->next = NULL;
				//
				*all_sections_tail = p;
				all_sections_tail = &( p->next );
				//
			}
			current_section = section_ptr[ TEXT_SECTION ];
			break;
		}
		default: {
			ABORT( "Invalid assembler pass" );
		}
	}
}

static void set_section( section sec ) {
	
	ASSERT( SECTION_VALID( sec ));
	
	current_section = section_ptr[ sec ];

	ASSERT( current_section );
}

static dword section_address( void ) {
	
	ASSERT( current_section );
	
	return( current_section->adrs );
}

static char *set_section_address( dword adrs ) {
	section_record	*p, *q,
			*prior,
			*follow;
	dword		prior_adrs,
			follow_adrs;
	
	ASSERT( current_section );

	//
	//	Enforce an alignment requirement on the
	//	provided address (before wading into
	//	actual verification).
	//
	if( adrs & 1 ) return( "Section address must be on a word boundary" );

	//
	//	We are starting a new section of addresses
	//	within the currently selected "section"
	//	(this is a direct response to an ORG
	//	directive).
	//
	//	We will need to create a new section record
	//	after we have determined if this clashes
	//	with other declared sections.
	//
	//	This also implies that the current record
	//	for this section type is closed so we move
	//	finish address back to the last used address
	//	(if the section is not empty).
	//
	if( !current_section->empty ) current_section->finish = current_section->adrs - 1;
	
	//
	//	Lots to do here.  We need to identify the sections which preceed
	//	and follow this address.Identify any other previous section which overlaps the requested
	//	address.
	//
	prior = NULL;
	prior_adrs = FIRST_ADDRESS;
	follow = NULL;
	follow_adrs = LAST_ADDRESS;
	for( p = all_sections; p; p = p->next ) {
		if(( !p->relative )&&( !p->empty )) {
			dword	la;

			la = p->adrs - 1;
			if(( adrs >= p->start )&&( adrs <= la )) {
				//
				//	Really, really need a better error system at this
				//	point.  It is planned and will be implemented.
				//
				return( "Cannot reset section address into existing section" );
			}
			if((( prior == NULL )||( la > prior_adrs ))&&( la < adrs )) {
				prior = p;
				prior_adrs = la;
			}
			if((( follow == NULL )||( p->start < follow_adrs ))&&( adrs < p->start )) {
				follow = p;
				follow_adrs = p->start - 1;
			}
		}
	}
	//
	//	So .. new address is not inside an existing section and
	//	we have a handle on the sections before and after.
	//
	//	If recycle is empty then we need to make a new record and
	//	fill out.  If recycle is not empty, then the next section
	//	record should be exactly the one we are looking for...
	//
	if( recycle_sections ) {
		q = recycle_sections;
		recycle_sections = q->next;

		ASSERT( q->where == current_section->where );

		if( sections_locked ) {
			//
			//	We are in code generation mode, the
			//	record located must be correct.
			//
			ASSERT( q->start == adrs );
			ASSERT( q->finish == follow_adrs );
			if( prior ) ASSERT( prior->finish == adrs - 1 );
			
			q->adrs = adrs;
		}
		else {
			//
			//	We are in verification phase.  It is
			//	possible that sections wiggle about.
			//
			q->start = adrs;
			q->adrs = adrs;
			q->finish = follow_adrs;
			if( prior ) prior->finish = adrs - 1;
		}
	}
	else {
		ASSERT( !sections_locked );
		
		q = NEW( section_record );
		q->where = current_section->where;
		q->relative = FALSE;
		q->empty = TRUE;
		q->start = adrs;
		q->adrs = adrs;
		q->finish = follow_adrs;
		if( prior ) prior->finish = adrs - 1;
	}
	//
	//	Add the record at q to the sections list and make it current.
	//
	q->next = NULL;
	//
	*all_sections_tail = q;
	all_sections_tail = &( q->next );
	//
	current_section = section_ptr[ current_section->where ] = q;
	//
	return( NULL );
}	

static char *extend_section( dword size ) {
	
	ASSERT( current_section );
	ASSERT( size >= 0 );

	if( size ) {
		//
		//	Obviously there's nothing to do unless
		//	size is non-zero.
		//
		if(( size - 1 ) <= ( current_section->finish - current_section->adrs )) {
			//
			//	We can safely move this forward.  The above check
			//	will allow the adrs value to pass the finish address
			//	by 1 location.  This is proper; 'finish' is not the first
			//	address outside the section, but rather it is the last
			//	address inside the section.  This should allow assembly
			//	code to be placed all the way to LAST_ADDRESS (inclusive).
			//
			current_section->adrs += size;
			current_section->empty = FALSE;
		}
		else {
			return( "Section cannot be extended past end address" );
		}
	}
	return( NULL );
}

static char *align_section_num( int num ) {

	ASSERT( current_section );
	ASSERT(( num == 1 )||( num == 2 )||( num == 4 ));

	//
	//	Word alignment for anything larger than a byte.
	//
	if(( num == 1 )||(( current_section->adrs & 1 ) == 0 )) return( NULL );
	return( extend_section( 1 ));
}
	
static char *align_section( sizing align ) {

	ASSERT( current_section );
	ASSERT(( align == SIZE_B )||( align == SIZE_W )||( align == SIZE_L ));

	switch( align ) {
		case SIZE_B: return( align_section_num( 1 ));
		case SIZE_W: return( align_section_num( 2 ));
		case SIZE_L: return( align_section_num( 4 ));
		default: ABORT( "Invalid alignment size" );
	}
	return( "Coding Error in align_section()" );
}

//
//	Analyse the whole set of sections the assmbler
//	has collected and determine if this is a valid
//	object for the "fixed" ouput formats (the only
//	ones available at the moment).
//
static bool reconcile_sections( FILE *to ) {
	dword		end_address;
	section_record	*p;
	
	//
	//	This routine arranges the sections into section number
	//	order in memory.  This is achieved (in an underhand way)
	//	by the section initialisation code which creates the first
	//	section records in section number order.
	//
	end_address = FIRST_ADDRESS;
	for( p = all_sections; p; p = p->next ) {
		if( p->relative ) {
			PRINT(( "Fix section %d to address " DWORD_FORMAT ".\n", (int)p->where, end_address ));
			p->relative = FALSE;
			p->start = end_address;
			end_address += p->adrs;
		}
	}
	if( option_flags & DISPLAY_SECTIONS ) {
		fprintf( to, "Section Table (unsorted):-\n" );
		fprintf( to, "Section\tAddress\n" );
		for( p = all_sections; p; p = p->next ) fprintf( to, "%d\t" DWORD_FORMAT "\n", (int)p->where, p->start );
	}
	return( TRUE );
}

//
//	TOKEN HANDLING
//	==============
//

//
//	Enumerate the list of tokens which the assembler will
//	recognise.
//
typedef enum {
	//
	//	This token value used to indicate an absence of a token id.
	//
	TOK_NONE = 0,
	//
	//	Symbols which are strictly not part of the syntax.
	//
	TOK_ERROR,			// Unrecognised symbolic element
	//
	//	Symbols which are complex and represent a class of
	//	syntax elements.
	//
	TOK_IDENTIFIER,			// A correctly structured name
	TOK_OPCODE,			// An opcode as located in the mnemonic lookup table
	TOK_AREG,			// An ADDRESS register
	TOK_DREG,			// A DATA register
	TOK_FPREG,			// An FPU register
	TOK_REGLIST,			// A register list bitmap
	TOK_NUMBER,			// An immediate value in hex, decimal or binary
	TOK_REAL,			// A real floating point number
	TOK_CHAR,			// An ASCII character in single quotes
	TOK_STRING,			// A series of ASCII characters in double quotes
	TOK_EXPRESSION,			// A SYNTHETIC token created by the expression
					// recognition code to enable simplified placement
					// of constants and constant expressions into the
					// source code
	//
	//	Register names which are outside the regular registers
	//
	TOK_PC,				// Program Counter
	TOK_SR,				// Whole status register
	TOK_CCR,			// Just the condition codes part of SR
	TOK_USP,			// The user mode stack pointer
	TOK_VBR,			// 68010: Vector Base Register
	TOK_SFC,			// 68010: Source Function Code
	TOK_DFC,			// 68010: Destination Function Code
	//
	//	The various keywords the assembler identifies
	//
	TOK_BYTE, TOK_WORD, TOK_LONG,	// The various sizing options
	TOK_TEXT, TOK_DATA, TOK_BSS,	// The various section names
	TOK_SECTION,			// Alternative section selection
	TOK_IMPORT, TOK_EXPORT,		// Declaring scope of identifiers
	TOK_ORG,			// Fix address (in current section)
	TOK_START,			// Set the execution start point.
	TOK_ALIGN,			// Align address (in current section) to multiple of argument
	TOK_EQU,			// Set label value
	TOK_END,			// Mark end of assembly source code
	TOK_DC, TOK_DS,			// Define data values and reserve space
	//
	//	Symbols which are individual characters
	//
	TOK_COLON, TOK_COMMA,		// Separators
	TOK_SEMICOLON,
	TOK_OPAREN, TOK_CPAREN,		// '(' and ')' symbols
	TOK_PLUS, TOK_MINUS,		// Mathematical Symbols
	TOK_MUL, TOK_DIV, TOK_MOD,
	TOK_AND, TOK_OR,		// Logical Symbols
	TOK_EOR, TOK_NOT,
	TOK_HASH,
	TOK_ADDRESS			// symbol for 'the current address'
} token_id;

//
//	Define a class used to capture a single token and any broken down
//	details that the token implies/contains.
//
typedef struct _token_ {
	token_id	id;		// What has been found
	char		*text;		// Where the token started in the buffer and
	int		len;		// How many characters the token occupied
	//
	//	The variable part of the token details, dependent on the
	// 	token ID.
	//
	union {
		word		regno;	// A register number
		struct {
			scope	contains;	// What we know about this value
			number	value;		// A numerical constant value
		} val;
		double		real;	// The value of a real number
		opcode		*op;	// The start of an opcode list
		char		*err;	// A static error message that might be some use
		struct _token_	*expr;	// Synthetic token argument
	} var;
	//
	//	Tokens are linked into lists.
	//
	struct _token_	*next;
} token;

//
//	Helper routine to provide 'indexing' down a token list
//
static token *token_n( token *from, int index ) {
	while(( index-- )&&( from != NULL )) {
		from = from->next;
	}
	return( from );
}

//
//	To simplify code we will use a table to check out single
//	character tokens.
//
typedef struct {
	char		symbol;
	token_id	id;
} token_char;
static token_char token_chars[] = {
	{ PLUS,			TOK_PLUS		},
	{ MINUS,		TOK_MINUS		},
	{ ASTERIX,		TOK_MUL			},
	{ SLASH,		TOK_DIV			},
	{ PERCENT,		TOK_MOD			},
	{ OPAREN,		TOK_OPAREN		},
	{ CPAREN,		TOK_CPAREN		},
	{ COLON,		TOK_COLON		},
	{ SEMICOLON,		TOK_SEMICOLON		},
	{ HASH,			TOK_HASH		},
	{ OPAREN,		TOK_OPAREN		},
	{ CPAREN,		TOK_CPAREN		},
	{ COMMA,		TOK_COMMA		},
	{ HAT,			TOK_EOR			},
	{ BANG,			TOK_NOT			},
	{ AMPERSAND,		TOK_AND			},
	{ BAR,			TOK_OR			},
	{ EOS }
};
static token_id find_symbol( char find ) {
	token_char *look;
	
	for( look = token_chars; look->symbol != EOS; look++ )
		if( find == look->symbol ) {
			PRINT(( "Found symbol '%c'.\n", look->symbol ));
			return( look->id );
		}
	return( TOK_NONE );
}

//
//	Define a mechanism for defining keywords, and the list of keywords.
//
typedef struct {
	char		*name;
	token_id	id;
} keyword;
static keyword dot_keywords[] = {
	{ "b",			TOK_BYTE		},
	{ "w",			TOK_WORD		},
	{ "l",			TOK_LONG		},
	{ NULL }
};
static keyword keywords[] = {
	{ "text",		TOK_TEXT		},
	{ "data",		TOK_DATA		},
	{ "bss",		TOK_BSS			},
	{ "section",		TOK_SECTION		},
	{ "org",		TOK_ORG			},
	{ "start",		TOK_START		},
	{ "align",		TOK_ALIGN		},
	{ "export",		TOK_EXPORT		},
	{ "xdef",		TOK_EXPORT		},
	{ "import",		TOK_IMPORT		},
	{ "xref",		TOK_IMPORT		},
	{ "equ",		TOK_EQU			},
	{ "end",		TOK_END			},
	{ "dc",			TOK_DC			},
	{ "ds",			TOK_DS			},
	//
	//	Register names that are not part of the
	//	normal register file.
	//
	{ "pc",			TOK_PC			},
	{ "sr",			TOK_SR			},
	{ "ccr",		TOK_CCR			},
	{ "usp",		TOK_USP			},
	{ "vbr",		TOK_VBR			},
	{ "sfc",		TOK_SFC			},
	{ "dfc",		TOK_DFC			},
	{ NULL }
};
static token_id find_keyword( keyword *table, char *find, int l ) {
	while( table->name != NULL ) {
		if( compare_word( find, l, table->name ) == 0 ) {
			return( table->id );
		}
		table++;
	}
	return( TOK_NONE );
}

//
//	Pick out a register name if presented.
//
static token_id is_register( char *s, int l, int *n ) {
	int	r;
	
	switch( l ) {
		case 2: {
			r = digit_value( s[ 1 ]);
			if(( r >= 0 )&&( r <= 7 )) {
				switch( s[ 0 ]) {
					case 'A':
					case 'a':{
						PRINT(( "Address register 'A%d'.\n", r ));
						*n = r;
						return( TOK_AREG );
					}
					case 'D':
					case 'd': {
						PRINT(( "Data register 'D%d'.\n", r ));
						*n = r;
						return( TOK_DREG );
					}
					default: {
						break;
					}
				}
			}
			//
			//	Special case register names:
			//
			if( strncasecmp( s, "sp", 2 ) == 0 ) {
				PRINT(( "SP register 'A7'.\n" ));
				*n = 7;
				return( TOK_AREG );
			}
			if( strncasecmp( s, "fp", 2 ) == 0 ) {
				PRINT(( "FP register 'A6'.\n" ));
				*n = 6;
				return( TOK_AREG );
			}
			break;
		}
		case 3: {
			if( strncasecmp( s, "fp", 2 ) == 0 ) {
				r = digit_value( s[ 2 ]);
				if(( r >= 0 )&&( r <= 7 )) {
					PRINT(( "FPU register 'FP%d'.\n", r ));
					*n = r;
					return( TOK_FPREG );
				}
			}
			break;
		}
		default: {
			//
			//	Anything else is definitely not a register
			//
			break;
		}
	}
	return( TOK_NONE );
}

//
//	A simple routine which 'looks ahead' from a supplied
//	pointer and returns the number of characters which
//	constitute a valid identifier.
//
static int find_identifier( char *look ) {
	int	l;
	
	l = 0;
	if( isalpha( *look ) || ( *look == UNDERSCORE )) {
		l = 1;
		look++;
		while( isalnum( *look ) || ( *look == UNDERSCORE )) {
			l++;
			look++;
		}
	}
	return( l );
}

//
//	The find string routine returns the number of characters
//	which form a character constant or character string not
//	including the terminating quote.
//
//	This is passed in the type of quote which marks the end
//	of the string.  This jumps the escape character ('\') and
//	the character that follows it.
//
// 	This routine does NO interpretation of what the captured
//	characters represent (ie '\n' -> 0x0A or '\t' -> 0x09 ).
//
static int find_quoted( char *from, char quote ) {
	int	l;
	char	c;

	l = 0;
	while(( c = *from++ ) != EOS ) {
		if( c == quote ) break;
		if( c == ESCAPE ) {
			if( *from++ == EOS ) break;
			l += 2;
		}
		else {
			l++;
		}
	}
	return( l );
}

//
//	Generalised number parsing routine.  Returns the number of
//	characters used.  On error the number of characters used
//	is negated (happens if an "out of base" digit is used
//	within the number).
//
static int parse_number( char *from, int base, number *val ) {
	number	s;
	int	l,
		v;
	bool	e;

	s = 0;
	l = 0;
	e = FALSE;
	while(( v = digit_value( *from )) >= 0 ) {
		if( v >= base ) {
			e = TRUE;
		}
		else {
			s = ( s * base ) + v;
		}
		from++;
		l++;
	}
	*val = s;
	PRINT(( "Numeric value %ld\n", s ));
	return( e? -l: l );
}

//
//	Find the next token off the string pointer (passed in by reference)
//	and fill in the token record appropriately.  Return TRUE if something
//	found, FALSE if the end of the string was encountered.
// 
//	This routine is a little fiddly and long, but does do the heavy lifting
//	with respect to identifying what has been sent for assembly.
//
static int next_token( char **ptr, token *tok ) {
	char		*s;
	token_id	i;
	int		l, n;

	//
	//	Set up...
	//
	memset( tok, 0, sizeof( token ));
	s = *ptr;
	//
	//	skip spaces..
	//
	while( isspace( *s )) s++;
	//
	//	at end of the string?
	//
	if( *s == EOS ) {
		*ptr = s;
		return( FALSE );
	}
	//
	//	Now try to work out what we are looking at...
	//
	switch( *s ) {
		case PERIOD: {
			//
			//	The start of a keyword or size option.
			//
			if(( l = find_identifier( s + 1 )) > 0 ) {
				if(( tok->id = find_keyword( dot_keywords, s+1, l )) != TOK_NONE ) {
					tok->text = s;
					tok->len = 1 + l;
					*ptr = s + tok->len;
				}
				else {
					//
					//	It is a badly formed dot keyword, convert to an
					//	error token with error message.
					//
					tok->id = TOK_ERROR;
					tok->text = s;
					tok->len = 1 + l;
					tok->var.err = "Unrecognised dot keyword";
					*ptr = s + tok->len;
				}
			}
			return( TRUE );
		}
		case DOLLAR: {
			//
			//	Traditional prefix for a hexadecimal number.
			//
			if(( l = parse_number( s + 1, 16, &( tok->var.val.value ))) > 0 ) {
				//
				//	an actual hexadecimal number..
				//
				tok->id = TOK_NUMBER;
				tok->var.val.contains = scope_value( tok->var.val.value );
				tok->text = s;
				tok->len = 1 + l;
				*ptr = s + tok->len;
			}
			else {
				if( l == 0 ) {
					//
					//	Its the shorthand for the address 'here'
					//
					tok->id = TOK_ADDRESS;
					tok->text = s;
					tok->len = 1;
					*ptr = s + 1;
				}
				else {
					//
					//	Its a badly formed number, convert to an
					//	error token with error message.
					//
					tok->id = TOK_ERROR;
					tok->text = s;
					tok->len = 1 - l;
					tok->var.err = "Malformed hexadecimal number";
					*ptr = s + tok->len;
				}
			}
			return( TRUE );
		}
		case QUOTE:
		case QUOTES: {
			//
			//	We treat a quoted character and a quoted string
			//	the same way in this code and only make a distinction
			//	when building the final token record.
			//
			char	q;

			q = *s++;
			l = find_quoted( s, q );
			if( s[ l ] != q ) {
				tok->id = TOK_ERROR;
				tok->text = s - 1;
				tok->len = l + 1;
				tok->var.err = ( q == QUOTES )? "String constant malformed": "Character constant malformed";
				*ptr = s + l;
			}
			else {
				if( q == QUOTE ) {
					tok->id = TOK_CHAR;
					//
					//	JEFF this is TEMPORARY
					//
					//	Should really code for a "normal" set
					//	of escape characters at this point.
					//
					tok->var.val.value = *s;
					tok->var.val.contains = scope_value( tok->var.val.value );
				}
				else {
					tok->id = TOK_STRING;
				}
				tok->text = s;
				tok->len = l;
				*ptr = s + ( l + 1 );
			}
			return( TRUE );
		}
		default: {
			//
			//	Finally those symbols that require a little
			//	analysis need finding.
			//
			//	Start with NUMBERS...
			//
			if( isdigit( *s )) {
				//
				//	Its a number, of some sort..
				//
				if( *s == ZERO ) {
					//
					//	Either Octal or C hexadecimal
					//
					if(( s[ 1 ] == 'x' )||( s[ 1 ] == 'X' )) {
						//
						//	Hexadecimal it is then..
						//
						if(( l = parse_number( s + 2, 16, &( tok->var.val.value ))) > 0 ) {
							//
							//	an actual hexadecimal number..
							//
							tok->id = TOK_NUMBER;
							tok->var.val.contains = scope_value( tok->var.val.value );
							tok->text = s;
							tok->len = 2 + l;
							*ptr = s + tok->len;
						}
						else {
							//
							//	Its a badly formed number, convert to an
							//	error token with error message.
							//
							tok->id = TOK_ERROR;
							tok->text = s;
							tok->len = 1 - l;
							tok->var.err = "Malformed hexadecimal number";
							*ptr = s + tok->len;
						}
					}
					//
					//	OK, Octal.
					//
					if(( l = parse_number( s, 8, &( tok->var.val.value ))) > 0 ) {
						//
						//	An actual Octal number
						//
						tok->id = TOK_NUMBER;
						tok->var.val.contains = scope_value( tok->var.val.value );
						tok->text = s;
						tok->len = l;
						*ptr = s + l;
					}
					else {
						//
						//	Its a badly formed number, convert to an
						//	error token with error message.
						//
						tok->id = TOK_ERROR;
						tok->text = s;
						tok->len = -l;
						tok->var.err = "Malformed octal number";
						*ptr = s + tok->len;
					}
				}
				else {
					//
					//	Finally, it must be a decimal number.
					//
					if(( l = parse_number( s, 10, &( tok->var.val.value ))) > 0 ) {
						//
						//	An actual Decimal number
						//
						tok->id = TOK_NUMBER;
						tok->var.val.contains = scope_value( tok->var.val.value );
						tok->text = s;
						tok->len = l;
						*ptr = s + l;
					}
					else {
						//
						//	Its a badly formed number, convert to an
						//	error token with error message.
						//
						tok->id = TOK_ERROR;
						tok->text = s;
						tok->len = -l;
						tok->var.err = "Malformed decimal number";
						*ptr = s + tok->len;
					}
				}
				//
				//	A number (or an error) has been created, so we can return
				// 	at this point.
				//
				return( TRUE );
			}
			//
			//	Final option, Identifiers (which include register names,
			//	opcode and assembler operations).
			//
			if(( l = find_identifier( s )) > 0 ) {
				//
				//	A register or possibly a register list?
				//
				if(( tok->id = is_register( s, l, &n )) != TOK_NONE ) {
					//
					//	Definitely a register, but this might be the
					//	start of a register list, but only if the next
					//	character is either a slash or a hyphen:
					//
					if((( tok->id == TOK_DREG )||( tok->id == TOK_AREG ))&&(( s[ l ] == SLASH )||( s[ l ] == MINUS ))) {
						char	c;
						int	m;
						bool	ok;
						
						//
						//	We are committed to this being a register
						//	list as we have a valid data/address register
						//	followed by one of the two continuation
						//	symbols.
						// 
						//	In the following code section data registers
						//	are numbered 0 to 7 and address registers
						//	8 to 15: this coincides withy their corresponding
						//	bit numbers in the register map bit array.
						//
						if( tok->id == TOK_AREG ) n += 8;
						tok->id = TOK_REGLIST;
						tok->var.regno = 1 << n;
						tok->text = s;
						tok->len = l;
						s += l;
						ok = TRUE;
						while( ok && (( *s == SLASH )||( *s == MINUS ))) {
							c = *s++;
							tok->len++;
							if(( l = find_identifier( s )) <= 0 ) {
								ok = FALSE;
								tok->id = TOK_ERROR;
								tok->var.err = "Register name missing in register list";
								break;
							}
							i = is_register( s, l, &m );
							s += l;
							tok->len += l;
							if(( i != TOK_DREG )||( i != TOK_AREG )) {
								ok = FALSE;
								tok->id = TOK_ERROR;
								tok->var.err = "Invalid register name in register list";
								break;
							}
							if( i == TOK_AREG ) m += 8;
							if( c == MINUS ) {
								// register range specification
								if( n >= m ) {
									ok = FALSE;
									tok->id = TOK_ERROR;
									tok->var.err = "Invalid register range in register list";
									break;
								}
								while( n < m ) {
									n++;
									tok->var.regno |= 1 << n;
								}
							}
							else {
								// just another register
								tok->var.regno |= 1 << m;
								n = m;
							}
						}
						*ptr = s;
					}
					else {
						//
						//	No, it's just an ordinary register.
						//
						tok->text = s;
						tok->len = l;
						tok->var.regno = n;
						*ptr = s + l;
					}
					return( TRUE );
				}
				//
				//	A keyword?
				//
				if(( tok->id = find_keyword( keywords, s, l )) != TOK_NONE ) {
					tok->text = s;
					tok->len = l;
					*ptr = s + l;
					return( TRUE );
				}
				//
				//	An actual opcode?
				//
				if(( tok->var.op = find_opcode( s, l )) != NULL ) {
					tok->id = TOK_OPCODE;
					tok->text = s;
					tok->len = l;
					*ptr = s + l;
					return( TRUE );
				}
				//
				//	So, it is just an identifier.
				//
				tok->id = TOK_IDENTIFIER;
				tok->text = s;
				tok->len = l;
				*ptr = s + l;
				return( TRUE );
			}
			//
			//	The symbols now remembering we have to build a couple of them
			//	by composing two symbols.
			//
			if(( i = find_symbol( *s )) != TOK_NONE ) {
				tok->id = i;
				tok->text = s;
				tok->len = 1;
				*ptr = s + 1;
				return( TRUE );
			}
			//
			//	Fall through to allow the catch all code to handle
			//
			break;
		}
	}
	//
	//	Getting here means this is probably an error, so build an error token.
	//
	tok->id = TOK_ERROR;
	tok->text = s;
	tok->len = 1;
	tok->var.err = "Unrecognised symbol";
	*ptr = s + 1;
	return( TRUE );
}

//
//	Routine only required for assisting in debugging code.
//
#ifdef ENABLE_DEBUG
static void dump_token( token *ptr ) {
	int	i;
	
	printf( "ID=%d\t'", ptr->id );
	for( i = 0; i < ptr->len; i++ ) printf( "%c", ptr->text[ i ]);
	printf( "'" );
	switch( ptr->id ) {
		case TOK_NUMBER: {
			printf( "\t%ld", ptr->var.val.value );
			break;
		}
		case TOK_ERROR: {
			printf( "\t%s", ptr->var.err );
			break;
		}
		case TOK_DREG: {
			printf( "\tD%d", ptr->var.regno );
			break;
		}
		case TOK_AREG: {
			printf( "\tA%d", ptr->var.regno );
			break;
		}
		case TOK_FPREG: {
			printf( "\tFP%d", ptr->var.regno );
			break;
		}
		case TOK_OPCODE: {
			int	i;
			opcode	*o;
			
			i = 0;
			for( o = ptr->var.op; o != NULL; o = o->next ) i++;
			printf( "\t%d ops", i );
			break;
		}
		default: {
			break;
		}
	}
	printf( "\n" );
}
#endif

//
// 	CONSTANT EXPRESSION PROCESSING
//	==============================
//

//
//	The following routines are the direct
//	implementation of the various operators.
//

//
//	Logical binary ops.
//
static char *op_eval_or( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left != NULL );
	ASSERT( right != NULL );

	node->var.val.contains = scope_value(( node->var.val.value = left->var.val.value | right->var.val.value )) & ~SCOPE_ADDRESS;
	return( NULL );
}
static char *op_eval_eor( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left != NULL );
	ASSERT( right != NULL );
	
	node->var.val.contains = scope_value(( node->var.val.value = left->var.val.value ^ right->var.val.value )) & ~SCOPE_ADDRESS;
	return( NULL );
}
static char *op_eval_and( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left != NULL );
	ASSERT( right != NULL );
	
	node->var.val.contains = scope_value(( node->var.val.value = left->var.val.value & right->var.val.value )) & ~SCOPE_ADDRESS;
	return( NULL );
}

//
//	Mathematical binary ops
//
static char *op_eval_add( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left != NULL );
	ASSERT( right != NULL );
	
	if( left->var.val.contains & SCOPE_ADDRESS ) {
		if( right->var.val.contains & SCOPE_ADDRESS ) return( "ADD invalid on address values" );
		node->var.val.contains = scope_value(( node->var.val.value = left->var.val.value + right->var.val.value ))|( left->var.val.contains & SCOPE_ADDRESS );
	}
	else {
		node->var.val.contains = scope_value(( node->var.val.value = left->var.val.value + right->var.val.value ))|( right->var.val.contains & SCOPE_ADDRESS );
	}
	return( NULL );
}
static char *op_eval_sub( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left != NULL );
	ASSERT( right != NULL );
	
	if(( left->var.val.contains & SCOPE_ADDRESS )!=( right->var.val.contains & SCOPE_ADDRESS )) return( "SUB cannot subtract dissimilar addresses" );
	node->var.val.contains = scope_value(( node->var.val.value = left->var.val.value - right->var.val.value )) & ~SCOPE_ADDRESS;
	return( NULL );
}
static char *op_eval_mul( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left != NULL );
	ASSERT( right != NULL );
	
	node->var.val.contains = scope_value(( node->var.val.value = left->var.val.value * right->var.val.value )) & ~SCOPE_ADDRESS;
	return( NULL );
}
static char *op_eval_div( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left != NULL );
	ASSERT( right != NULL );
	
	node->var.val.contains = scope_value(( node->var.val.value = left->var.val.value / right->var.val.value )) & ~SCOPE_ADDRESS;
	return( NULL );
}
static char *op_eval_mod( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left != NULL );
	ASSERT( right != NULL );
	
	node->var.val.contains = scope_value(( node->var.val.value = left->var.val.value % right->var.val.value )) & ~SCOPE_ADDRESS;
	return( NULL );
}

//
//	Unary operators
//
static char *op_eval_minus( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left == NULL );
	ASSERT( right != NULL );
	
	node->var.val.contains = scope_value(( node->var.val.value = -right->var.val.value )) & ~SCOPE_ADDRESS;
	return( NULL );
}
static char *op_eval_plus( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left == NULL );
	ASSERT( right != NULL );
	
	node->var.val.contains = scope_value(( node->var.val.value = right->var.val.value )) & ~SCOPE_ADDRESS;
	return( NULL );
}
static char *op_eval_not( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left == NULL );
	ASSERT( right != NULL );
	
	node->var.val.contains = scope_value(( node->var.val.value = !right->var.val.value )) & ~SCOPE_ADDRESS;
	return( NULL );
}

//
//	Atom evaluation routines.
//
static char *op_eval_null( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left == NULL );
	ASSERT( right == NULL );
	
	return( NULL );
}

static char *op_eval_ident( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left == NULL );
	ASSERT( right == NULL );
	
	if( !get_ident_value( node->text, node->len, &( node->var.val.contains ), &( node->var.val.value ))) return( "Invalid identifier value" );
	return( NULL );
}

static char *op_eval_address( token *node, token *left, token *right ) {
	
	ASSERT( node != NULL );
	ASSERT( left == NULL );
	ASSERT( right == NULL );
	
	node->var.val.value = section_address();
	node->var.val.contains = section_scope();
	return( NULL );
}

//
//	For each operator/atom an instance of the following
//	structure provide details on how it should be handled.
//
typedef struct {
	token_id	id;
	char		*FUNC( eval )( token *node, token *left, token *right );
} expr_operator;

//
//	This table defines all the tokens that can be used
//	to make up an expression and their roles and relative
//	importance.  Levels start at 0 (the lowest precedence)
//	and rise as high as they need to.
//
//	The relative levels are based on the precedence of C.
//
static expr_operator expr_ops_0[] = {
	{ TOK_OR,		op_eval_or	},
	{ TOK_NONE }
};
static expr_operator expr_ops_1[] = {
	{ TOK_EOR,		op_eval_eor	},
	{ TOK_NONE }
};
static expr_operator expr_ops_2[] = {
	{ TOK_AND,		op_eval_and	},
	{ TOK_NONE }
};
static expr_operator expr_ops_3[] = {
	{ TOK_PLUS,		op_eval_add	},
	{ TOK_MINUS,		op_eval_sub	},
	{ TOK_NONE }
};
static expr_operator expr_ops_4[] = {
	{ TOK_MUL,		op_eval_mul	},
	{ TOK_DIV,		op_eval_div	},
	{ TOK_MOD,		op_eval_mod	},
	{ TOK_NONE }
};
static expr_operator expr_ops_prefix[] = {
	{ TOK_MINUS,		op_eval_minus	},
	{ TOK_PLUS,		op_eval_plus	},
	{ TOK_NOT,		op_eval_not	},
	{ TOK_NONE }
};
static expr_operator expr_ops_atoms[] = {
	{ TOK_IDENTIFIER,	op_eval_ident	},
	{ TOK_NUMBER,		op_eval_null	},
	{ TOK_CHAR,		op_eval_null	},
	{ TOK_ADDRESS,		op_eval_address	},
	{ TOK_NONE }
};
#define EXPR_LEVELS 5
static expr_operator *expr_ops[ EXPR_LEVELS ] = {
	expr_ops_0,
	expr_ops_1,
	expr_ops_2,
	expr_ops_3,
	expr_ops_4
};

//
//	simple operator lookup
//
static expr_operator *find_expr_op( token_id id, expr_operator *list ) {
	
	ASSERT( list != NULL );
	
	while( list->id != TOK_NONE ) {
		if( list->id == id ) return( list );
		list++;
	}
	return( NULL );
}

//
//	This is the routine which, recursively, evaluates a
//	list of tokens into a constant result.
//
static int eval_expr( token *tok, int level, token **top ) {
	expr_operator	*p;
	int		i, l, m;
	token		*r, *s;
	char		*err;

	ASSERT( top != NULL );
	ASSERT( level >= 0 );
	ASSERT( level <= EXPR_LEVELS );

	//
	//	If there is no token, then this is an error.
	//
	if( tok == NULL ) return( 0 );
	//
	//	If level is equal to EXPR_LEVELS then we are in
	//	the "prefix and atomic" level of the expression
	//	code.
	//
	if( level == EXPR_LEVELS ) {
		//
		//	'()' sub-expression?
		//
		if( tok->id == TOK_OPAREN ) {
			PRINT(( "in (\n" ));
			tok = tok->next;
			if(( l = eval_expr( tok, 0, &r )) == 0 ) return( 0 );
			for( i = 0; i < l; i++ ) tok = tok->next;
			if( tok->id != TOK_CPAREN ) return( 0 );
			*top = r;
			PRINT(( "out )\n" ));
			return( l + 2 );
		}
		//
		//	Process prefix (if found) then some
		//	form of atom object.
		//
		if(( p = find_expr_op( tok->id, expr_ops_prefix )) != NULL ) {
			//
			//	Handle the prefix we have in hand
			//
			if(( l = eval_expr( tok->next, EXPR_LEVELS, &r )) > 0 ) {
				if(( err = FUNC( p->eval )( tok, NULL, r )) == NULL ) {
					*top =  tok;
					return( 1 + l );
				}
				PRINT(( "Eval prefix = '%s'\n", err ));
			}
			return( 0 );
		}
		//
		//	Return an atom (if found).
		//
		if(( p = find_expr_op( tok->id, expr_ops_atoms )) != NULL ) {
			if(( err = FUNC( p->eval )( tok, NULL, NULL )) == NULL ) {
				*top =  tok;
				return( 1 );
			}
			PRINT(( "Eval atom = '%s'\n", err ));
		}
		return( 0 );
	}
	//
	//	Processing infix notations so we need
	//	to recurse, collect infix and loop.
	//
	if(( l = eval_expr( tok, level+1, &r )) > 0 ) {
		//
		//	Something found, and it is at the address
		//	in r, having used l tokens.  The token we
		//	are now interested in is l tokens away.
		//
		for( i = 0; i < l; i++ ) tok = tok->next;
		//
		//	This is our initial value against which we
		//	roll in the new operator and argument pairs
		//	(left to right evaluation mode).
		//
		// 	Not finding an operator means we drop through
		// 	and return what ever we have found.
		//
		while(( tok != NULL )&&(( p = find_expr_op( tok->id, expr_ops[ level ])) != NULL )) {
			//
			//	Found an op at this level, so tokens after
			//	it contain the next argument that we need
			//	for evaluation.  Get the head of that tree
			//	in s containing m tokens.
			//
			if(( m = eval_expr( tok->next, level+1, &s )) == 0 ) return( 0 );
			//
			//	Evaluate and reshape so l and r still contain
			//	the proper result.
			//
			if(( err = FUNC( p->eval )( tok, r, s )) != NULL ) {
				PRINT(( "Eval error = '%s'\n", err ));
				return( 0 );
			}
			r = tok;					// Remember result
			l += 1 + m;					// extend token count
			tok = tok->next;				// Skip op
			for( i = 0; i < m; i++ ) tok = tok->next;	// Skip arg
		}
	}
	//
	//	We return whatever the content of l and r indicate.
	//
	*top = r;
	return( l );
}


//
// 	ARGUMENT PROCESSING CODE
//	========================
//

//
//	Define a structure in which we will gather the interpretation
// 	of an argument to facilitate the construction of a target
// 	machine code instruction.
//
typedef struct {
	//
	//	Capture the set of argument types this could be
	//
	arg_type	match;
	//
	//	All formal EA arguments fill in these fields.
	//
	word		adjust,		// adjustment to the opcode//
			extends;	// Number of words the opcode is extended//
	//
	//	The extension value or abstract argument value is here.
	//
	number		value;
	scope		contains;
} argument;

//
//	Data Register Direct
// 	--------------------
//
//	Syntax:		Dr
//	Symbols: 	1
// 	EA code:	000rrr		(binary)
//
#define EA_DATA_REG_DIRECT_TOKENS 1
static token_id ea_data_reg_direct[ EA_DATA_REG_DIRECT_TOKENS ] = { TOK_DREG };
static char *data_reg_direct( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*reg;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	reg = token_n( tokens, 0 );		// pointless, but consistent
	
	ASSERT( reg != NULL );
	ASSERT( reg->id == TOK_DREG );
	ASSERT( reg->var.regno >= 0 );
	ASSERT( reg->var.regno <= 7 );

	PRINT(( "Data Register Direct: D%d\n", reg->var.regno ));
	
	ptr->match = EA_DREG | NUM_DREG;
	ptr->adjust = 000 | reg->var.regno;	// (octal)
	ptr->extends = 0;
	ptr->value = reg->var.regno;
	
	return( NULL );
}

//
//	Address Register Direct
// 	-----------------------
// 
//	Syntax:		Ar
//	Symbols: 	1
// 	EA code:	001rrr		(binary)
//
#define EA_ADRS_REG_DIRECT_TOKENS 1
static token_id ea_adrs_reg_direct[ EA_ADRS_REG_DIRECT_TOKENS ] = { TOK_AREG };
static char *adrs_reg_direct( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*reg;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	reg = token_n( tokens, 0 );		// pointless, but consistent (see below)
	
	ASSERT( reg != NULL );
	ASSERT( reg->id == TOK_AREG );
	ASSERT( reg->var.regno >= 0 );
	ASSERT( reg->var.regno <= 7 );
	
	PRINT(( "Address Register Direct: A%d\n", reg->var.regno ));
	
	ptr->match = EA_AREG | NUM_AREG;
	ptr->adjust = 010 | reg->var.regno;	// (octal)
	ptr->extends = 0;
	ptr->value = reg->var.regno;
	
	return( NULL );
}

//
//	Address Register Indirect
// 	-------------------------
//
//	Syntax:		(Ar)
//	Symbols: 	3
// 	EA code:	010rrr		(binary)
//
#define EA_ADRS_REG_INDIRECT_TOKENS 3
static token_id ea_adrs_reg_indirect[ EA_ADRS_REG_INDIRECT_TOKENS ] = { TOK_OPAREN, TOK_AREG, TOK_CPAREN };
static char *adrs_reg_indirect( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*reg;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	reg = token_n( tokens, 1 );		// Makes sense now
	
	ASSERT( reg != NULL );
	ASSERT( reg->id == TOK_AREG );
	ASSERT( reg->var.regno >= 0 );
	ASSERT( reg->var.regno <= 7 );
	
	PRINT(( "Address Register Indirect: (A%d)\n", reg->var.regno ));
	
	ptr->match = EA_AREG_IND;
	ptr->adjust = 020 | reg->var.regno;	// (octal)
	ptr->extends = 0;
	ptr->value = 0;
	
	return( NULL );
}

//
// 	Address Register Indirect with Post Increment
// 	----------------------------------------------
// 
//	Syntax:		(Ar)+
// 	Symbols: 	4
// 	EA code:	011rrr		(binary)
//
#define EA_ADRS_REG_IND_INC_TOKENS 4
static token_id ea_adrs_reg_ind_inc[ EA_ADRS_REG_IND_INC_TOKENS ] = { TOK_OPAREN, TOK_AREG, TOK_CPAREN, TOK_PLUS };
static char *adrs_reg_ind_inc( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*reg;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	reg = token_n( tokens, 1 );
	
	ASSERT( reg != NULL );
	ASSERT( reg->id == TOK_AREG );
	ASSERT( reg->var.regno >= 0 );
	ASSERT( reg->var.regno <= 7 );
	
	PRINT(( "Address Register Indirect Post-Inc: (A%d)+\n", reg->var.regno ));
	
	ptr->match = EA_AREG_IND_INC | NUM_AREG_IND_INC;
	ptr->adjust = 030 | reg->var.regno;	// (octal)
	ptr->extends = 0;
	ptr->value = reg->var.regno;
	
	return( NULL );
}

//
//	Address Register Indirect with Pre Decrement
// 	--------------------------------------------
//
//	Syntax:		-(Ar)
// 	Symbols: 	4
// 	EA code:	100rrr		(binary)
//
#define EA_ADRS_REG_DEC_IND_TOKENS 4
static token_id ea_adrs_reg_dec_ind[ EA_ADRS_REG_DEC_IND_TOKENS ] = { TOK_MINUS, TOK_OPAREN, TOK_AREG, TOK_CPAREN };
static char *adrs_reg_dec_ind( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*reg;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	reg = token_n( tokens, 2 );
	
	ASSERT( reg != NULL );
	ASSERT( reg->id == TOK_AREG );
	ASSERT( reg->var.regno >= 0 );
	ASSERT( reg->var.regno <= 7 );
	
	PRINT(( "Address Register Indirect Pre-Dec: -(A%d)\n", reg->var.regno ));
	
	ptr->match = EA_AREG_DEC_IND | NUM_AREG_DEC_IND;
	ptr->adjust = 040 | reg->var.regno;	// (octal)
	ptr->extends = 0;
	ptr->value = reg->var.regno;
	
	return( NULL );
}

//
//	Address Register Indirect with Displacement
// 	-------------------------------------------
// 
//	Syntax:		d(Ar)
// 	Symbols: 	4
// 	EA code:	101rrr			(binary)
// 	Extension:	dddd dddd dddd dddd	Signed 16 bit displacement
//
#define EA_ADRS_REG_IND_DISP_TOKENS 4
static token_id ea_adrs_reg_ind_disp[ EA_ADRS_REG_IND_DISP_TOKENS ] = { TOK_EXPRESSION, TOK_OPAREN, TOK_AREG, TOK_CPAREN };
static char *adrs_reg_ind_disp( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*disp,
		*reg;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	disp = token_n( tokens, 0 );
	reg = token_n( tokens, 2 );
	
	ASSERT( disp != NULL );
	ASSERT( disp->id == TOK_EXPRESSION );
	ASSERT( disp->var.expr != NULL );
	
	ASSERT( reg != NULL );
	ASSERT( reg->id == TOK_AREG );
	ASSERT( reg->var.regno >= 0 );
	ASSERT( reg->var.regno <= 7 );
	
	PRINT(( "Address Register Indirect Displacement: disp(A%d)\n", reg->var.regno ));
	
	ptr->match = EA_AREG_IND_DISP;	
	ptr->adjust = 050 | reg->var.regno;	// (octal)
	ptr->extends = 1;
	ptr->value = disp->var.expr->var.val.value;
	ptr->contains = disp->var.expr->var.val.contains;

	if(!( ptr->contains & SCOPE_S16 )) return( "Signed word displacement out of range" );

	PRINT(( "disp = %ld\n", ptr->value ));
	
	return( NULL );
}

//
//	Address Register Indirect with Index
//	------------------------------------
// 
//	Syntax:		d(Ar,Dn.W)
//			d(Ar,Dn.L)
//			d(Ar,An.W)
//			d(Ar,An.L)
// 	Symbols: 	7
// 	EA code:	110rrr			(binary)
//	Extension:	0nnn 0000 dddd dddd	Dn.W plus Signed 8 bit displacement
//			0nnn 1000 dddd dddd	Dn.L plus Signed 8 bit displacement
//			1nnn 0000 dddd dddd	An.W plus Signed 8 bit displacement
//			1nnn 1000 dddd dddd	An.L plus Signed 8 bit displacement
//
#define EA_ADRS_REG_IND_REG_TOKENS 7
static token_id ea_adrs_reg_ind_idw[ EA_ADRS_REG_IND_REG_TOKENS ] = { TOK_EXPRESSION, TOK_OPAREN, TOK_AREG, TOK_COMMA, TOK_DREG, TOK_WORD, TOK_CPAREN };
static token_id ea_adrs_reg_ind_idl[ EA_ADRS_REG_IND_REG_TOKENS ] = { TOK_EXPRESSION, TOK_OPAREN, TOK_AREG, TOK_COMMA, TOK_DREG, TOK_LONG, TOK_CPAREN };
static token_id ea_adrs_reg_ind_iaw[ EA_ADRS_REG_IND_REG_TOKENS ] = { TOK_EXPRESSION, TOK_OPAREN, TOK_AREG, TOK_COMMA, TOK_AREG, TOK_WORD, TOK_CPAREN };
static token_id ea_adrs_reg_ind_ial[ EA_ADRS_REG_IND_REG_TOKENS ] = { TOK_EXPRESSION, TOK_OPAREN, TOK_AREG, TOK_COMMA, TOK_AREG, TOK_LONG, TOK_CPAREN };
static char *fill_out_index( argument *ptr, token *tokens ) {
	token	*disp, *reg, *size;
	dword	d;
	scope	s;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	disp = token_n( tokens, 0 );
	reg = token_n( tokens, 4 );
	size = token_n( tokens, 5 );

	ASSERT( disp != NULL );
	ASSERT( disp->id == TOK_EXPRESSION );
	ASSERT( disp->var.expr != NULL );

	ASSERT( reg != NULL );
	ASSERT(( reg->id == TOK_DREG )||( reg->id == TOK_AREG ));
	ASSERT(( reg->var.regno >= 0 )&&( reg->var.regno <= 7 ));
	
	ASSERT( size != NULL );
	ASSERT(( size->id == TOK_WORD )||( size->id == TOK_LONG ));

	ptr->extends = 1;
	d = disp->var.expr->var.val.value;
	s = disp->var.expr->var.val.contains;

	if(!( s & SCOPE_S8 )) return( "Signed byte displacement out of range" );
	
	ptr->value = 	(( reg->id  == TOK_DREG )? 0x0000: 0x8000 )
			 | (( size->id == TOK_WORD )? 0x0000: 0x0800 )
			 | ( reg->var.regno << 12 )
			 | ( d & 0xFF );
	ptr->contains =	NO_SCOPE;

	PRINT(( "disp = "DWORD_FORMAT"\n", d ));
	PRINT(( "index = %c%d.%c\n", (( reg->id  == TOK_DREG )? 'D': 'A' ), reg->var.regno, (( size->id == TOK_WORD )? 'W': 'L' )));

	return( NULL );
}
static char *areg_ind_disp_reg( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*reg;

	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );

	reg = token_n( tokens, 2 );
	
	ASSERT( reg != NULL );
	ASSERT( reg->id == TOK_AREG );
	ASSERT( reg->var.regno >= 0 );
	ASSERT( reg->var.regno <= 7 );
	
	PRINT(( "Address Register Indirect Index: disp(A%d,index)\n", reg->var.regno ));
	
	ptr->match = EA_AREG_IND_DISP_REG;	
	ptr->adjust = 060 | reg->var.regno;	// (octal)
	
	return( fill_out_index( ptr, tokens ));
}

//
//	Short or Long Absolute Addressing
//	---------------------------------
// 
// 	Symbols:	1
// 
// 	Short Absolute Addressing:
//
//	Syntax:		x
// 	EA code:	111000			(binary)
// 	Extension:	xxxx xxxx xxxx xxxx	16 bit address sign extended to 32 bits
// 
// 	Long Absolute Addressing:
// 
//	Syntax:		x
// 	EA code:	111001			(binary)
// 	Extension:	xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx
// 
//	Syntactically this is the same as a branch or jump to another section
//	of code or a subroutine address.  The aim here is to capture all of
// 	these possibilities in the one structure.
//
#define EA_ABSOLUTE_TOKENS 1
static token_id ea_absolute[ EA_ABSOLUTE_TOKENS ] = { TOK_EXPRESSION };
static char *absolute( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*num;
	dword	adrs;
	scope	contains;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	num = token_n( tokens, 0 );
	
	ASSERT( num != NULL );
	ASSERT( num->id == TOK_EXPRESSION );
	ASSERT( num->var.expr != NULL );
	
	adrs = num->var.expr->var.val.value;
	contains = num->var.expr->var.val.contains;
	
	if( contains & SCOPE_S16 ) {

		PRINT(( "Short Absolute "DWORD_FORMAT"\n", adrs ));
		
		ptr->match = EA_ABS_SHORT_IND | ABS_ADDRESS;
		ptr->adjust = 070;		// (octal)
		ptr->extends = 1;
	}
	else {
		PRINT(( "Long Absolute "DWORD_FORMAT"\n", adrs ));
		
		ptr->match = EA_ABS_LONG_IND | ABS_ADDRESS;
		ptr->adjust = 071;		// (octal)
		ptr->extends = 2;
	}
	ptr->value = adrs;
	return( NULL );
}

//
//	Program Counter Indirect with Displacement
// 	------------------------------------------
// 
// 	Symbols:	4
// 	EA code:	111010			(binary)
// 	Extension:	dddd dddd dddd dddd	Signed 16 bit displacement
//
#define EA_PC_IND_DISP_TOKENS 4
static token_id ea_pc_ind_disp[ EA_PC_IND_DISP_TOKENS ] = { TOK_EXPRESSION, TOK_OPAREN, TOK_PC, TOK_CPAREN };
static char *pc_ind_disp( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*disp;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );

	disp = token_n( tokens, 0 );
	
	ASSERT( disp != NULL );
	ASSERT( disp->id == TOK_EXPRESSION );
	ASSERT( disp->var.expr != NULL );

	PRINT(( "Program Counter Indirect with Displacement: disp(PC)\n" ));
	
	ptr->match = EA_PC_IND_DISP;	
	ptr->adjust = 072;		// (octal)
	ptr->extends = 1;
	ptr->value = disp->var.expr->var.val.value;
	ptr->contains = disp->var.expr->var.val.contains;
	
	if(!( ptr->contains | SCOPE_S16 )) return( "Invalid signed word value" );

	PRINT(( "disp = %ld", ptr->value ));
	return( NULL );
}

//
//	Program Counter Indirect with Index
// 	-----------------------------------
// 
// 	Symbols:	7
// 	EA code:	111011			(binary)
//	Extension:	0nnn 0000 dddd dddd	Dn.W plus Signed 8 bit displacement
//			0nnn 1000 dddd dddd	Dn.L plus Signed 8 bit displacement
//			1nnn 0000 dddd dddd	An.W plus Signed 8 bit displacement
//			1nnn 1000 dddd dddd	An.L plus Signed 8 bit displacement
//
#define EA_PC_IND_REG_TOKENS 7
static token_id ea_pc_ind_idw[ EA_PC_IND_REG_TOKENS ] = { TOK_EXPRESSION, TOK_OPAREN, TOK_PC, TOK_COMMA, TOK_DREG, TOK_WORD, TOK_CPAREN };
static token_id ea_pc_ind_idl[ EA_PC_IND_REG_TOKENS ] = { TOK_EXPRESSION, TOK_OPAREN, TOK_PC, TOK_COMMA, TOK_DREG, TOK_LONG, TOK_CPAREN };
static token_id ea_pc_ind_iaw[ EA_PC_IND_REG_TOKENS ] = { TOK_EXPRESSION, TOK_OPAREN, TOK_PC, TOK_COMMA, TOK_AREG, TOK_WORD, TOK_CPAREN };
static token_id ea_pc_ind_ial[ EA_PC_IND_REG_TOKENS ] = { TOK_EXPRESSION, TOK_OPAREN, TOK_PC, TOK_COMMA, TOK_AREG, TOK_LONG, TOK_CPAREN };
static char *pc_ind_disp_reg( sizing sizes, bool source, argument *ptr, token *tokens ) {
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );

	PRINT(( "Program Counter Indirect with Index: disp(PC,index)\n" ));
	
	ptr->match = EA_PC_IND_DISP_REG;	
	ptr->adjust = 073;		// (octal)
	
	return( fill_out_index( ptr, tokens ));
}

//
// 	Immediate Mode
// 	--------------
// 
// 	Only valid if specified as the source component
//
// 	Symbols:	2
//	EA code:	111100						(binary)
// 	Extension:	.... .... xxxx xxxx				8 bit immediate value
// 			xxxx xxxx xxxx xxxx				16 bit immediate value
// 			xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx		32 bit immediate value
//
#define EA_IMMEDIATE_TOKENS 2
static token_id ea_immediate[ EA_IMMEDIATE_TOKENS ] = { TOK_HASH, TOK_EXPRESSION };
static char *immediate( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*imm;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	imm = token_n( tokens, 1 );

	ASSERT( imm != NULL );
	ASSERT( imm->id == TOK_EXPRESSION );
	ASSERT( imm->var.expr != NULL );

	PRINT(( "Immediate value: #imm\n" ));
	
	ptr->match = EA_IMMEDIATE;
	ptr->adjust = 074;		// (octal)
	ptr->extends = ( sizes & SIZE_L )? 2: 1;

	//
	//	Collect the value calculated
	//
	ptr->value = imm->var.expr->var.val.value;
	ptr->contains = imm->var.expr->var.val.contains;

	//
	//	Add other immediate categories based on value.
	//
	if(( ptr->value >= 0 )&&( ptr->value <= 7 ))	ptr->match |= NUM_IMMEDIATE_3BIT;
	if(( ptr->value >= 1 )&&( ptr->value <= 8 ))	ptr->match |= NUM_IMMEDIATE_QUICK;
	if(( ptr->value >= 0 )&&( ptr->value <= 31 ))	ptr->match |= NUM_IMMEDIATE_5BIT;
	if( ptr->contains & ( SCOPE_S8 | SCOPE_U8 ))	ptr->match |= NUM_IMMEDIATE_8;
	if( ptr->contains & ( SCOPE_S16 | SCOPE_U16 ))	ptr->match |= NUM_IMMEDIATE_16;

	PRINT(( "imm = %ld\n", ptr->value ));
	return( NULL );
}

//
//	Status Register and Condition Codes Register
// 	--------------------------------------------
// 
// 	Only valid if specified as the destination component
// 
//	Symbols:	1
// 	EA code:	111100		(binary)
//
#define EA_STATUS_TOKENS 1
static token_id ea_sr[ EA_STATUS_TOKENS ] = { TOK_SR };
static token_id ea_ccr[ EA_STATUS_TOKENS ] = { TOK_CCR };
static char *sr_ccr_reg_direct( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*reg;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	reg = token_n( tokens, 0 );
	
	ASSERT( reg != NULL );
	ASSERT(( reg->id == TOK_SR )||( reg->id == TOK_CCR ));
	
	if( source ) return( "SR/CCR must be a destination argument" );

	PRINT((( reg->id == TOK_SR )? "Status Register\n": "Condition Codes Register\n" ));
	
	ptr->match = ( reg->id == TOK_SR )? EA_STATUS_REG: EA_FLAGS_REG;
	ptr->adjust = 074;
	ptr->extends = 0;
	
	return( NULL );
}

//
//	Any of the Control Registers
//	----------------------------
//
//	This is one of the USP, VBR, SFC or DFC registers.  While
//	initially arriving in the 68010, the USP was partially
//	available in the 68000, though not through the more
//	orthogonal 68010 mechanism.
//
//	Symbols:	1
// 	EA code:	Not Applicable
// 	Extension:	xxxx xxxx xxxx
//
// 	The 12 bit number gives the specification for the control
//	register being referenced.  The following table capture the
//	value values:
//
// 		Binary		Register	CPU
//		------		--------	---
//		0x000		SFC		68010
//		0x001		DFC		68010
//		0x800		USP		68000/68010
//		0x801		VBR		68010
//
#define EA_CONTROL_TOKENS 1
static token_id ea_sfc[ EA_CONTROL_TOKENS ] = { TOK_SFC };
static token_id ea_dfc[ EA_CONTROL_TOKENS ] = { TOK_DFC };
static token_id ea_usp[ EA_CONTROL_TOKENS ] = { TOK_USP };
static token_id ea_vbr[ EA_CONTROL_TOKENS ] = { TOK_VBR };
static char *sr_control_reg_direct( sizing sizes, bool source, argument *ptr, token *tokens ) {
	token	*reg;
	
	ASSERT( ptr != NULL );
	ASSERT( tokens != NULL );
	
	reg = token_n( tokens, 0 );
	
	ASSERT( reg != NULL );
	ASSERT(( reg->id == TOK_SFC )||( reg->id == TOK_DFC )||( reg->id == TOK_USP )||( reg->id == TOK_VBR ));

	ptr->match = CONTROL_REG;
	ptr->adjust = 0;
	ptr->extends = 1;
	switch( reg->id ) {
		case TOK_SFC: {
			PRINT(( "SFC Control Register\n" ));
			ptr->extends = 0x000;
			break;
		}
		case TOK_DFC: {
			PRINT(( "DFC Control Register\n" ));
			ptr->extends = 0x001;
			break;
		}
		case TOK_USP: {
			PRINT(( "USP Control Register\n" ));
			ptr->extends = 0x800;
			break;
		}
		case TOK_VBR: {
			PRINT(( "VBR Control Register\n" ));
			ptr->extends = 0x801;
			break;
		}
		default: {
			ABORT( "Invalid control register" );
			break;
		}
	}
	return( NULL );
}



//
//	Define a set of rules which outline how a valid opcode argument is
//	constructed and the set of arg_type bit values which capture that
//	combination.
//
typedef struct {
	int		len;
	token_id	*desc;
	//
	//	The provided function translates the source code tokens provided
	//	into the argument data structure returns NULL on success or an
	// 	address of an error message on failure.
	// 
	// 		source		TRUE if working on the source argument
	// 		ptr		Where the consolidated information should be placed
	//		tokens		The source code tokens forming this argument
	//
	char		*FUNC( encode )( sizing sizes, bool source, argument *ptr, token *tokens );
} arg_syntax;

//
//	The sequenced rules for matching opcode arguments that use the
//	above arrays, but essentially in longest array first order.
//
//	This is achieved through the manual intervention of the programmer
//	ensuring this array is in this order.  I will add debugging code
//	to verify this condition when compiled with debugging enabled.
//
static arg_syntax ea_search_list[] = {
	//
	//	Effective Addresses with 7 syntactic components
	//
	{ EA_ADRS_REG_IND_REG_TOKENS,	ea_adrs_reg_ind_idw,	areg_ind_disp_reg	},
	{ EA_ADRS_REG_IND_REG_TOKENS,	ea_adrs_reg_ind_idl,	areg_ind_disp_reg	},
	{ EA_ADRS_REG_IND_REG_TOKENS,	ea_adrs_reg_ind_iaw,	areg_ind_disp_reg	},
	{ EA_ADRS_REG_IND_REG_TOKENS,	ea_adrs_reg_ind_ial,	areg_ind_disp_reg	},
	{ EA_PC_IND_REG_TOKENS,		ea_pc_ind_idw,		pc_ind_disp_reg		},
	{ EA_PC_IND_REG_TOKENS,		ea_pc_ind_idl,		pc_ind_disp_reg		},
	{ EA_PC_IND_REG_TOKENS,		ea_pc_ind_iaw,		pc_ind_disp_reg		},
	{ EA_PC_IND_REG_TOKENS,		ea_pc_ind_ial,		pc_ind_disp_reg		},
	//
	//	EA with 4 components
	//
	{ EA_ADRS_REG_IND_DISP_TOKENS,	ea_adrs_reg_ind_disp,	adrs_reg_ind_disp	},
	{ EA_PC_IND_DISP_TOKENS,	ea_pc_ind_disp,		pc_ind_disp		},
	{ EA_ADRS_REG_IND_INC_TOKENS,	ea_adrs_reg_ind_inc,	adrs_reg_ind_inc	},
	{ EA_ADRS_REG_DEC_IND_TOKENS,	ea_adrs_reg_dec_ind,	adrs_reg_dec_ind	},
	//
	//	EA with 3 components
	//
	{ EA_ADRS_REG_INDIRECT_TOKENS,	ea_adrs_reg_indirect,	adrs_reg_indirect	},
	//
	//	EA with 2 components
	//
	{ EA_IMMEDIATE_TOKENS,		ea_immediate,		immediate		},
	//
	//	EA as a single token
	//
	{ EA_ABSOLUTE_TOKENS,		ea_absolute,		absolute		},
	{ EA_DATA_REG_DIRECT_TOKENS,	ea_data_reg_direct,	data_reg_direct		},
	{ EA_ADRS_REG_DIRECT_TOKENS,	ea_adrs_reg_direct,	adrs_reg_direct		},
	{ EA_STATUS_TOKENS,		ea_sr,			sr_ccr_reg_direct	},
	{ EA_STATUS_TOKENS,		ea_ccr,			sr_ccr_reg_direct	},
	//
	//	Control Registers
	//
	{ EA_CONTROL_TOKENS,		ea_sfc,			sr_control_reg_direct	},
	{ EA_CONTROL_TOKENS,		ea_dfc,			sr_control_reg_direct	},
	{ EA_CONTROL_TOKENS,		ea_usp,			sr_control_reg_direct	},
	{ EA_CONTROL_TOKENS,		ea_vbr,			sr_control_reg_direct	},
	//
	//	End of list
	//
	{ 0 }
};

//
//	Search through the argument look up table (above) and find
//	a match returning its record address, or NULL.
//
static arg_syntax *find_argument( token *input ) {
	arg_syntax	*look;
	token		*test;
	
#ifdef ENABLE_DEBUG
	//
	//	Including table verification code.
	//
	int		last_len;
	last_len = 999;			// Just needs to be larger than any possible token sequence.
#endif

	for( look = ea_search_list; look->len; look++ ) {
		int	i;
		bool	ok;

#ifdef ENABLE_DEBUG
		ASSERT( look->len <= last_len );
		last_len = look->len;
#endif
		test = input;
		ok = TRUE;
		for( i = 0; i < look->len; i++ ) {
			if(( test == NULL )||( look->desc[ i ] != test->id )) {
				ok = FALSE;
				break;
			}
			test = test->next;
		}
		if( ok ) return( look );
	}
	return( NULL );
}	

//
// 	LINE COMPOSING ROUTINES
//	=======================
//
//	A single line of assembler is condensed into the following
//	data structure.
//
typedef struct {
	token		*label,
			*action;
	sizing		size;
	//
	//	Set 'instruction' TRUE for assembly language instructions
	//	FALSE for assembler directives.
	//
	int		instruction;
	union {
		//
		//	68k assembly arguments placed in here
		//
		struct {
			int		args;
			argument	arg[ INSTRUCTION_ARGS ];
		} instruction;
		//
		//	All assembler directives gather their arguments
		//	in the 'var.assembler_op.' section.
		//
		struct {
			int	args;
			token	*arg[ DIRECTIVE_ARGS ];
		} assembler_op;
	} var;
} input;

//
// 	DIRECTIVE PROCESSING CODE
//	=========================
//

//
//	Just not written yet
//
static char *dir_not_written( input *inp ) {
	return( "Placeholder set for this directive" );
}

//
//	Change current section
//
static char *dir_set_section( input *inp ) {

	ASSERT( inp != NULL );
	
	if( inp->label != NULL ) return( "Label invalid on section change" );
	if( inp->size != NO_SIZE ) return( "Section change does not support sizes" );

	if( inp->action->id == TOK_SECTION ) {
		token	*v;
		int	s;

		if( inp->var.assembler_op.args != 1 ) return( "Missing argument for section directive" );
		v = inp->var.assembler_op.arg[ 0 ];

		ASSERT( v != NULL );

		switch( v->id ) {
			case TOK_EXPRESSION: {

				ASSERT( v != NULL );
				
				v = v->var.expr;
				s =(int)( v->var.val.value );
				if(( s < 0 )||( s > 2 )) return( "Section change argument is out of range: 0, 1 or 2" );
				set_section( s );
				break;
			}
			case TOK_TEXT: {
				set_section( TEXT_SECTION );
				break;
			}
			case TOK_DATA: {
				set_section( DATA_SECTION );
				break;
			}
			case TOK_BSS: {
				set_section( BSS_SECTION );
				break;
			}
			default: {
				return( "Section directive requires numeric argument or section name" );
			}
		}
	}
	else {
	
		if( inp->var.assembler_op.args != 0 ) return( "Section names have no arguments" );

		switch( inp->action->id ) {
			case TOK_TEXT: {
				set_section( TEXT_SECTION );
				break;
			}
			case TOK_DATA: {
				set_section( DATA_SECTION );
				break;
			}
			case TOK_BSS: {
				set_section( BSS_SECTION );
				break;
			}
			default: {
				ABORT( "Invalid Section Id" );
				break;
			}
		}
	}
	return( NULL );
}

//
//	Change section address.
//
//	This is an implicit change from a relative addressing mode to
//	absolute section addressing.  This affects only this section,
//	though this might have consequences for the final code generation.
//
static char *dir_set_address( input *inp ) {
	token	*v;
	dword	a;
	char	*e;

	if( inp->size != NO_SIZE ) return( "Section address (ORG) does not support sizes" );
	if( inp->var.assembler_op.args != 1 ) return( "Incorrect argument count for section address (ORG)" );

	v = inp->var.assembler_op.arg[ 0 ];

	ASSERT( v != NULL );
	
	if( v->id != TOK_EXPRESSION ) return( "Section address (ORG) argument is a single number" );
	v = v->var.expr;

	ASSERT( v != NULL );
	
	a = v->var.val.value;
	
	if(( e = set_section_address( a )) != NULL ) return( e );

	set_address( a );
	
	if( inp->label != NULL ) return( set_ident_value( inp->label->text, inp->label->len, section_scope(), section_address()));
	
	return( NULL );
}

//
//	Set execution address.
//
//	This is an implicit change from a relative addressing mode to
//	absolute section addressing.  This affects only this section,
//	though this might have consequences for the final code generation.
//
static char *dir_set_start( input *inp ) {
	token	*v;
	dword	a;

	if( inp->size != NO_SIZE ) return( "Start address (START) does not support sizes" );
	if( inp->var.assembler_op.args != 1 ) return( "Incorrect argument count for start address (START)" );

	v = inp->var.assembler_op.arg[ 0 ];

	ASSERT( v != NULL );
	
	if( v->id != TOK_EXPRESSION ) return( "Start address (START) argument is an address" );
	v = v->var.expr;

	ASSERT( v != NULL );

	if(!( a = v->var.val.contains & SCOPE_TEXT )) return( "Start address (START) argument is a TEXT address" );

	//
	//	Really ought to validate the supplied address.
	//
	set_start( v->var.val.value );
	
	if( inp->label != NULL ) return( set_ident_value( inp->label->text, inp->label->len, section_scope(), section_address()));
	
	return( NULL );
}

//
//	Define constant memory content
//
static char *dir_defn_const( input *inp ) {
	token	*t;
	char	*err;
	int	a, b, c;

	if(( section_scope() & SCOPE_DATA ) == 0 ) return( "Define constant (DC) only valid in data section" );
	if(( c = get_size( inp->size )) == 0 ) return( "Define constant (DC) requires size specification" );
	
	if( inp->label != NULL ) if(( err = set_ident_value( inp->label->text, inp->label->len, section_scope(), section_address())) != NULL ) return( err );
	
	if( inp->var.assembler_op.args < 1 ) return( "Define constant (DC) requires data values" );

	align_section( inp->size );
	
	for( a = 0; a < inp->var.assembler_op.args; a++ ) {
		t = inp->var.assembler_op.arg[ a ];

		ASSERT( t != NULL );
		
		switch( t->id ) {
			case TOK_STRING: {
				//
				//	roll out the content of the string
				//
				for( b = 0; b < t->len; b++ ) PRINT(( "string[%d]='%c'\n", b, t->text[ b ]));
				extend_section( t->len * c );
				break;
			}
			case TOK_EXPRESSION: {
				//
				//	Evaluate the argument and output that value
				//
				extend_section( c );
				break;
			}
			default: {
				ABORT( "Invalid constant argument ");
				break;
			}
		}
	}
	return( NULL );
}

//
//	Define empty memory content.
//
static char *dir_defn_space( input *inp ) {
	return( "ds: Not implemented" );
}

//
//	Import label from extern source.
//
static char *dir_import( input *inp ) {
	token	*t;
	char	*i;
	
	ASSERT( inp != NULL );
	ASSERT( inp->action != NULL );
	ASSERT( inp->action->id == TOK_IMPORT );
	ASSERT(!( inp->instruction ));

	if( inp->label != NULL ) return( "IMPORT cannot be labelled" );
	if( inp->var.assembler_op.args != 1 ) return( "IMPORT requires 1 argument" );
	t = inp->var.assembler_op.arg[ 0 ];
	
	ASSERT( t != NULL );

	if( t->id != TOK_EXPRESSION ) return( "Invalid argument to IMPORT" );
	t = t->var.expr;
	
	if(( t == NULL )||( t->id != TOK_IDENTIFIER )) return( "Invalid argument to IMPORT" );

	i = SPACE( t->len + 1 );
	strncpy( i, t->text, t->len );
	i[ t->len ] = EOS;

	PRINT(( "Importing identifier '%s'.\n", i ));

	return( import_identifier( i ));
}

//
//	Export label to outside code.
//
static char *dir_export( input *inp ) {
	token	*t;
	char	*i;
	
	ASSERT( inp != NULL );
	ASSERT( inp->action != NULL );
	ASSERT( inp->action->id == TOK_EXPORT );
	ASSERT(!( inp->instruction ));

	if( inp->label != NULL ) return( "EXPORT cannot be labelled" );
	if( inp->var.assembler_op.args != 1 ) return( "EXPORT requires 1 argument" );
	t = inp->var.assembler_op.arg[ 0 ];
	
	ASSERT( t != NULL );

	if( t->id != TOK_EXPRESSION ) return( "Invalid argument to EXPORT" );
	t = t->var.expr;
	
	if(( t == NULL )||( t->id != TOK_IDENTIFIER )) return( "Invalid argument to EXPORT" );

	i = SPACE( t->len + 1 );
	strncpy( i, t->text, t->len );
	i[ t->len ] = EOS;

	PRINT(( "Exporting identifier '%s'.\n", i ));

	return( export_identifier( i ));
}

//
//	Set a label value.
//
static char *dir_set_label( input *inp ) {
	token	*t;
	scope	s;
	
	ASSERT( inp != NULL );
	ASSERT( inp->action != NULL );
	ASSERT( inp->action->id == TOK_EQU );
	ASSERT(!( inp->instruction ));

	if( inp->label == NULL ) return( "No label specified for EQU" );
	if( inp->var.assembler_op.args != 1 ) return( "EQU requires 1 argument" );
	t = inp->var.assembler_op.arg[ 0 ];
	
	ASSERT( t != NULL );
	
	if( t->id != TOK_EXPRESSION ) return( "Invalid argument to EQU" );
	t = t->var.expr;

	ASSERT( t != NULL );

	//
	//	Set s to the size of all scopes we can accept as this
	//	identifier.  Default to word if no size provided.
	//
	if(( s = scope_size( inp->size )) == NO_SCOPE ) s = scope_size( SIZE_W );

	if(( t->var.val.contains & s ) == 0 ) return( "Constant value out of scope for label" );
	
	return( set_ident_value( inp->label->text, inp->label->len, t->var.val.contains, t->var.val.value ));
}

//
//	Define a table containing the directives the assembler
//	understands, and the routines which implement them.
//
typedef struct {
	token_id	id;
	char		*FUNC( action )( input *inp );
} directive;
static directive directive_list[] = {
	{ TOK_TEXT,	dir_set_section		},
	{ TOK_DATA,	dir_set_section		},
	{ TOK_BSS,	dir_set_section		},
	{ TOK_SECTION,	dir_set_section		},
	{ TOK_ORG,	dir_set_address		},
	{ TOK_START,	dir_set_start		},
	{ TOK_ALIGN,	dir_not_written		},
	{ TOK_EQU,	dir_set_label		},
	{ TOK_END,	dir_not_written		},
	{ TOK_DC,	dir_defn_const		},
	{ TOK_DS,	dir_defn_space		},
	{ TOK_IMPORT,	dir_import		},
	{ TOK_EXPORT,	dir_export		},
	{ TOK_NONE }
};

//
//	Simple find a directive routine.
//
static directive *find_directive( token_id find ) {
	directive	*look;

	for( look = directive_list; look->id != TOK_NONE; look++ ) {
		if( look->id == find ) {
			return( look );
		}
	}
	return( NULL );
}

//
//	Directive processing.
//
static char *process_directive( input *inp ) {
	directive	*dir;
	
	ASSERT( inp != NULL );
	ASSERT(!( inp->instruction ));
	ASSERT( inp->action != NULL );

	if(( dir = find_directive( inp->action->id )) == NULL ) return( "Directive not implemented" );
	return( FUNC( dir->action )( inp ));
}

//
// 	INSTRUCTION PROCESSING CODE
//	===========================
//

//
//	Define the variable which holds the set of mc68xxx chips which
//	this run of the assembler will consider valid instructions.
//
//	Valid bits (combined with OR):
//
// 		FLAG_68000	<- Default value
//		FLAG_68010	
//		FLAG_68020	
//		FLAG_68030	
//		FLAG_68881	
//
static opflags target_cpu = FLAG_68000;


//
//	Instruction processing.
//
static char *process_instruction( input *inp ) {
	char		*err;
	opcode		*look;
	arg_type	a1, a2;
	int		j;

	ASSERT( inp != NULL );
	ASSERT( inp->instruction );
	ASSERT( inp->action != NULL );
	ASSERT( inp->action->id == TOK_OPCODE );
	ASSERT( inp->action->var.op != NULL );

	//
	//	Are we in the right section?
	//
	if(!( section_scope() & SCOPE_TEXT )) return( "Incorrect section for instructions" );
	
	//
	//	As this is an instruction we can deal with the
	//	label (if present) immediately as it applies to the
	//	instruction being processed.
	//
	if(( inp->label != NULL )&&(( err = set_ident_value( inp->label->text, inp->label->len, section_scope(), section_address())) != NULL )) return( err );
	//
	//	Determine the argument combination we have..
	//
	switch( inp->var.instruction.args ) {
		case 0: {
			a1 = NO_ARGUMENT;
			a2 = NO_ARGUMENT;
			break;
		}
		case 1: {
			a1 = inp->var.instruction.arg[ 0 ].match;
			a2 = NO_ARGUMENT;
			break;
		}
		case 2: {
			a1 = inp->var.instruction.arg[ 0 ].match;
			a2 = inp->var.instruction.arg[ 1 ].match;
			break;
		}
		default: {
			ABORT( "Invalid number of arguments" );
			break;
		}
	}
	//
	//	Now we identify the correct opcode we are assembling.
	//
	for( look = inp->action->var.op; look != NULL; look = look->next ) {
		int	s;

		switch( look->size ) {
			case NO_SIZE: {
				//
				//	If the opcode definition says no size then
				//	there must be no size.
				//
				s = ( inp->size == NO_SIZE );
				break;
			}
			case SIZE_B: {
				//
				//	If the opcode definition says byte only then
				//	there can be either no size or it must be a byte.
				//
				s = (( inp->size == NO_SIZE )||( inp->size == SIZE_B ));
				break;
			}
			case SIZE_W: {
				//
				//	If the opcode definition says word only then
				//	there can be either no size or it must be a word.
				//
				s = (( inp->size == NO_SIZE )||( inp->size == SIZE_W ));
				break;
			}
			case SIZE_L: {
				//
				//	If the opcode definition says long only then
				//	there can be either no size or it must be a long.
				//
				s = (( inp->size == NO_SIZE )||( inp->size == SIZE_L ));
				break;
			}
			default: {
				//
				//	Finally the supplied size must be one of the
				//	opcode supported sizes.
				//
				//	However, if the supplied size is empty and the
				//	opcode has a selection of sizes we pick word
				//	(if it is an option).
				//
				if(( inp->size == NO_SIZE )&&( look->size & SIZE_W )) {
					inp->size = SIZE_W;
					s = TRUE;
				}
				else {
					s = (( look->size & inp->size ) != 0 );
				}
				break;
			}
		}
		//
		//	Do the argument(s) match the opcode description?
		//
		if(( s )	&&((( look->arg1 == NO_ARGUMENT )&&( a1 == NO_ARGUMENT ))||( look->arg1 & a1 ))
				&&((( look->arg2 == NO_ARGUMENT )&&( a2 == NO_ARGUMENT ))||( look->arg2 & a2 ))) {
			//
			//	Yes, break out of the loop.
			//
			break;
		}
	}
	//
	//	Found?  Error if not.
	//
	if( look == NULL ) return( "Invalid instruction and argument combination" );
	
	//
	//	Are we allowed to use this instruction?
	//
	if(( look->flags & target_cpu ) == 0 ) return( "Instruction not valid for target CPU" );
	
	//
	//	We can code generate now using the data encoded in the
	//	opcode record pointed to by look.
	//
	// 	For the moment calculate j to be the size of the opcode,
	//	we will count words.  There is always at least one word:
	//
	j = 1;
	
	//
	//	Identify the instruction flags which indicate very specific
	//	handling when converting from assembly to machine code.
	//
	if( look->flags & FLAG_8_16_REL ) {
		//
		//	Relative branch instructions with either 8 or
		//	16 bit offsets need special consideration to
		//	get them right.
		//
		number	dist;
		scope	contains;

		//
		//	The following calculation has an emdedded hidden
		//	error.  It calculates the potential branch offset
		//	using the address of the opcode plus two.  This is
		//	good for testing an 8 bit offset, but will need
		//	re doing for 16 bit offsets..
		//
		dist = inp->var.instruction.arg[ 0 ].value - ((number)section_address() + 2 );
		PRINT(( "Branch offset (8 bit testing) = " NUMBER_FORMAT "\n", dist ));
		contains = scope_value( dist );
		if( contains & SCOPE_S8 ) {
			//
			//	Offset can be placed in low order byte
			//	of opcode itself.  No extra words required:
			//
			j += 0;
			
			//
			//	Code Generation..
			//
			if( assembler_pass == CODE_GENERATION ) {
				//
				//	Top/First byte is the actual branch instruction
				//	followed by the distance to the target instruction.
				//
				add_byte( H( look->basecode ));
				add_byte( L( dist ));
			}
		}
		else {
			//
			//	Getting here means we have (probably) a sixteen bit
			//	displacement.  The value in dist is wrong at this point
			//	and needs redoing:
			//
			dist = inp->var.instruction.arg[ 0 ].value - ((number)section_address() + 4 );
			PRINT(( "Branch offset (16 bit testing) = "NUMBER_FORMAT"\n", dist ));
			contains = scope_value( dist );
			if( contains & SCOPE_S16 ) {
				//
				//	Offset needs to be placed into one extra
				//	extended word.
				//
				j += 1;
				
				//
				//	Code Generation..
				//
				if( assembler_pass == CODE_GENERATION ) {
					//
					//	Top/First byte is the actual branch instruction
					//	second byte is 0, indicating 16-bit relative branch
					//	is following.
					//
					add_byte( H( look->basecode ));
					add_byte( 0 );
					add_byte( H( dist ));
					add_byte( L( dist ));
				}
			}
			else {
				//
				//	Conceptually there is an issue here (relative branch
				//	out of range), but it is only an issue if it has not
				//	resolved itself by the time we get to code generation.
				//
				if( assembler_pass == CODE_GENERATION ) {
					return( "Branch offset exceeds signed word range" );
				}
				//
				//	We will assume this will resolve itself next time
				//	round and use a "worst case" senario using an extra
				//	word.
				//
				j += 1;
			}
		}
	}
	else {
		if( look->flags & FLAG_16_REL ) {
			//
			//	Relative branch instructions with only 16
			//	bit offsets only need range checking
			//	applied in code generation.
			//
			number	dist;
			scope	contains;

			//
			//	When calculating the offset we deduct four to
			//	allow for the offset (opcode plus word offset).
			//
			dist = (dword)inp->var.instruction.arg[ 0 ].value - ((number)section_address() + 4 );
			PRINT(( "Branch offset (bit) = " NUMBER_FORMAT "\n", dist ));
			contains = scope_value( dist );

			//
			//	Here is the mandatory offset word
			//
			j += 1;
			
			//
			//	Code Generation..
			//
			if( assembler_pass == CODE_GENERATION ) {
				if(!( contains & SCOPE_S16 )) {
					//
					//	This is an issue now we are in code generation
					//	mode.
					//
					  return( "Branch offset exceeds signed word range" );
				}
				//
				//	Top/First byte is the actual branch instruction
				//	second byte is 0, indicating 16-bit relative branch.
				//
				add_byte( H( look->basecode ));
				add_byte( L( look->basecode ));
				add_byte( H( dist ));
				add_byte( L( dist ));
			}
		}
		else {
			//
			//	Processing for all "normal" instructions starts here.
			//
			//	For all remaining instructions we calculate the
			//	instructions size using a combination of the data
			//	in the input record and the opcode record.
			//
			if( inp->var.instruction.args > 0 ) {
				if( look->off1 != IGNORE ) {
					if( IS_AUX( look->off1 )) j += 1;
					j += inp->var.instruction.arg[ 0 ].extends;
				}
			}
			if( inp->var.instruction.args > 1 ) {
				if( look->off2 != IGNORE ) {
					if( IS_AUX( look->off2 )) j += 1;
					j += inp->var.instruction.arg[ 1 ].extends;
				}
			}
			//
			//	If we are performing code generation then there are
			//	a whole pile of additional steps that need to be
			//	completed.
			//
			//	We will use the output of the above code segment (the
			//	value in j) as a cross reference and consistency check.
			//	Both sections of code should always calculate the same
			//	numer of words in the instruction.
			//
			if( assembler_pass == CODE_GENERATION ) {
				int	k;	// generator equivalent to j.
				word	op,	// mandatory first opcode
					auxop;	// optional additional opcode
				bool	aux;	// Flag; got auxop?
				int	ext;	// How many words are we extending?
				dword	arg;	// value for argument extension.

				//
				//	Now we decode the opcode record to construct the
				//	actual machine code indicated by the instruction.
				//
				k = 1;
				op = look->basecode;
				aux = FALSE;
				auxop = 0;
				ext = 0;
				arg = 0;
				
				//
				//	Fix the operator SIZE bits.
				//
				switch( look->size ) {
					case NO_SIZE:
					case SIZE_B:
					case SIZE_W:
					case SIZE_L: {
						//
						//	All these options have no sizing bits
						//	so we drop out leaving the opcode unchanged.
						//
						break;
					}
					case SIZE_WL: {
						//
						//	Just the word/long option.  Only 1 bit
						//	will need updating if the size specified
						//	is Long.
						//
						op |= (( inp->size & SIZE_L )? 1: 0 ) << look->sizebit;
						break;
					}
					case SIZE_BWL: {
						word	s;
						//
						//	The full set of options, we need to manually
						//	pick out the right bits.
						//
						//
						switch( inp->size ) {
							case SIZE_B: {
								s = 0;	// bits '00'
								break;
							}
							case SIZE_W: {
								s = 1;	// bits '01'
								break;
							}
							case SIZE_L: {
								s = 2;	// bits '10'
								break;
							}
							default: {
								//
								//	This could default to word, bits '01'
								//	but this case should not happen.  Abort.
								//
								s = 1;
								ABORT( "Invalid input size" );
								break;
							}
						}
						op |= s << look->sizebit;
						break;
					}
					default: {
						//
						//	All other cases are rubbish and
						//	should not appear in the assembler.
						//
						ABORT( "Invalid instruction size specification" );
						break;
					}
				}

				//
				//	Argument 1.
				//
				if( inp->var.instruction.args > 0 ) {
					int	e;
					
					//
					//	Handle possible argument to opcode.
					//
					if(( e = inp->var.instruction.arg[ 0 ].extends )) {

						ASSERT( ext == 0 );

						k += ( ext = e );
						arg = inp->var.instruction.arg[ 0 ].value;
					}
					//
					//	If we are not ignoring an argument nor
					//	appending it then there is stuff to do.
					//		
					if(( look->off1 != IGNORE )&&( look->off1 != APPEND )) {
						//
						//	op code modifying argument.
						//
						if( IS_AUX( look->off1 )) {

							ASSERT( !aux );
							
							aux = TRUE;
							auxop = look->auxcode | ( inp->var.instruction.arg[ 0 ].adjust << AUX_BIT( look->off1 ));
							k += 1;
						}
						else {
							op |= inp->var.instruction.arg[ 0 ].adjust << look->off1;
						}
					}
				}

				//
				//	Argument 2.
				//
				if( inp->var.instruction.args > 1 ) {
					int	e;
					
					//
					//	Handle possible argument to opcode.
					//
					if(( e = inp->var.instruction.arg[ 1 ].extends )) {

						ASSERT( ext == 0 );

						k += ( ext = e );
						arg = inp->var.instruction.arg[ 1 ].value;
					}
					//
					//	If we are not ignoring an argument nor
					//	appending it then there is stuff to do.
					//		
					if(( look->off2 != IGNORE )&&( look->off2 != APPEND )) {
						//
						//	op code modifying argument.
						//
						if( IS_AUX( look->off2 )) {

							ASSERT( !aux );
							
							aux = TRUE;
							auxop = look->auxcode | ( inp->var.instruction.arg[ 1 ].adjust << AUX_BIT( look->off2 ));
							k += 1;
						}
						else {
							op |= inp->var.instruction.arg[ 1 ].adjust << look->off2;
						}
					}
				}

				//
				//	Emit k words of machine code.
				//
				ASSERT( k == j );

				//
				//	Ouput the opcode and auxiliary opcode if
				//	required.
				//
				add_byte( H( op ));
				add_byte( L( op ));
				if( aux ) {
					add_byte( H( auxop ));
					add_byte( L( auxop ));
				}
				//
				//	Now append any argument data if specified.
				//
				switch( ext ) {
					case 0: {
						//
						//	Nothing to add so nothign to do.
						//
						break;
					}
					case 1: {
						//
						//	Just append the word value.
						//
						add_byte( H( arg ));
						add_byte( L( arg ));
						break;
					}
					case 2: {
						//
						//	Add the long values.
						//
						add_byte( DW0( arg ));
						add_byte( DW1( arg ));
						add_byte( DW2( arg ));
						add_byte( DW3( arg ));
						break;
					}
					default: {
						ABORT( "Invalid opcode extension in code generator" );
						break;
					}
				}
				//
				//	And that is this instruction done.
				//
			}
		}
	}

	PRINT(( "Opcode is %d words.\n", j ));

	//
	//	Move on the section address, remembering to
	//	convert length in words to bytes.
	//
	extend_section( j << 1 );
	
	return( NULL );
}

//
// 	MAIN ASSEMBLER ROUTINES
//	=======================
//
//	Routine takes an input line of assembler, breaks it down
//	into components, identifies the nature of the input line
//	and calls appropriate routines to handle.
//
// 	Return NULL on OK, or an error string on fault.
//
static char *process_input( int line, char *buf ) {
	token		*head,
			**tail,
			*tok,
			*top;
	int		len;
	input		*inp;
	directive	*dir;
	
	//
	//	Build a stack memory based copy of the input line.
	//
	tail = &head;
	len = 0;
	tok = STACK( token );
	while( next_token( &buf, tok )) {
		
#ifdef ENABLE_DEBUG
		if( option_flags & DISPLAY_DEBUG ) dump_token( tok );
#endif

		//
		//	If this is an error token then we return the error message
		//
		if( tok->id == TOK_ERROR ) return( tok->var.err );
		
		//
		//	If we find a comment symbol then that is the end
		//	of the line as far as the assembler is concerned.
		//
		if((( tok->id == TOK_MUL )&&( len == 0 ))||( tok->id == TOK_SEMICOLON )) break;
		//
		//	Pin on the end of the token list
		//
		*tail = tok;
		tail = &( tok->next );
		len++;
		tok = STACK( token );
	}
	*tail = NULL;
	//
	//	Now analyse the line to see what we get.  The line is (should)
	//	be formatted in the following ways:
	//
	//		[ label [ : ]]	opcode [ .size ] [ argument [ , argument ]* ]
	//
	// 	We start analysis by pointing a pointer to the head of the list and
	//	clearing our consolidated line record.
	//
	//
	//	Start at the top of the list and check for empty line?
	//
	tail = &head;
	if(( tok = *tail ) == NULL ) return( NULL );
	//
	//	So look into what we have.
	//
	inp = STACK( input );
	inp->label = NULL;
	inp->action = NULL;
	inp->size = NO_SIZE;
	inp->instruction = FALSE;
	//
	//	Option label (with optional colon)?
	//
	if( tok->id == TOK_IDENTIFIER ) {
		inp->label = tok;
		tok = *( tail = &( tok->next ));
		if(( tok != NULL )&&( tok->id == TOK_COLON )) tok = *( tail = &( tok->next ));
	}
	//
	//	A line with just a label is valid, just try setting the
	// 	identifier appropriately.  It's neither an instruction
	//	or a directive.
	//
	if( tok == NULL ) {
		if( inp->label != NULL ) return( set_ident_value( inp->label->text, inp->label->len, section_scope(), section_address()));
		return( NULL );
	}
	//
	//	The next token is an opcode or assembler directive
	//
	if( tok->id == TOK_OPCODE ) {
		inp->action = tok;
		inp->instruction = TRUE;
		inp->var.instruction.args = 0;
		//
		//	End of instruction?
		//
		if(( tok = *( tail = &( tok->next ))) == NULL ) return( process_instruction( inp ));
		//
		//	Do we have explicit sizing?
		//
		switch( tok->id ) {
			case TOK_BYTE: {
				inp->size = SIZE_B;
				tok = *( tail = &( tok->next ));
				break;
			}
			case TOK_WORD: {
				inp->size = SIZE_W;
				tok = *( tail = &( tok->next ));
				break;
			}
			case TOK_LONG: {
				inp->size = SIZE_L;
				tok = *( tail = &( tok->next ));
				break;
			}
			default: {
				//
				//	An unspecified size could be any size
				//
				break;
			}
		}
		//
		//	Gather up the arguments
		//
		while( tok != NULL ) {
			arg_syntax	*a;
			int		i;
			char		*e;

			//
			//	Bail out if we have filled in all possible opcode arguments
			//
			if( inp->var.instruction.args == INSTRUCTION_ARGS ) return( "Too many arguments for valid opcode" );
			//
			//	Have a peek into the list of tokens and determine
			//	if we need to attempt some expression evaluation.
			//
			if( tok->id == TOK_HASH ) {
				//
				//	This *always* precedes some sort of numerical values
				//
				if(( len = eval_expr( tok->next, 0, &top )) == 0 ) return( "Invalid immediate expression" );
				len++;
				while( len-- ) tok = tok->next;
				head = STACK( token );
				head->id = TOK_EXPRESSION;
				head->var.expr = top;
				head->next = tok;
				tok = *tail;
				tok->next = head;
			}
			else {
				//
				//	Failing an explicitly specified immediate value all
				//	assembler arguments which have a constant have it as
				//	the first component.  So *attempt* to evaluate the
				//	token list.
				//
				if(( len = eval_expr( tok, 0, &top )) > 0 ) {
					while( len-- ) tok = tok->next;
					head = STACK( token );
					head->id = TOK_EXPRESSION;
					head->var.expr = top;
					head->next = tok;
					*tail = head;
					tail = &( head->next );
					*tail = tok;
					tok = head;
				}
			}
			//
			//	Now find out what type of argument has been presented.
			// 
			if(( a = find_argument( tok )) == NULL ) return( "Unrecognised argument form" );
			if(( e = FUNC( a->encode )( inp->size, ( inp->var.instruction.args == 0 ), &( inp->var.instruction.arg[ inp->var.instruction.args ]), tok )) != NULL ) return( e );
			//
			//	Increment the argument counter and skip past the tokens
			//	that were used by this argument.
			//
			inp->var.instruction.args++;
			for( i = 0; i < a->len; i++ ) tok = *( tail = &( tok->next ));
			//
			//	Do we have an "inter-argument" comma?
			//
			if( tok != NULL ) {
				if( tok->id != TOK_COMMA ) return( "Comma missing before new argument" );
				if(( tok = *( tail = &( tok->next ))) == NULL ) return( "Argument missing after comma" );
			}
		}
		//
		//	Arguments gathered so process the instruction.
		//
		return( process_instruction( inp ));
	}

	//
	//	Is this a directive?
	//
	if(( dir = find_directive( tok->id )) != NULL ) {
		inp->action = tok;
		inp->instruction = FALSE;
		inp->var.assembler_op.args = 0;
		//
		//	End of instruction?
		//
		if(( tok = *( tail = &( tok->next ))) == NULL ) return( process_directive( inp ));
		//
		//	Do we have explicit sizing?
		//
		switch( tok->id ) {
			case TOK_BYTE: {
				inp->size = SIZE_B;
				tok = *( tail = &( tok->next ));
				break;
			}
			case TOK_WORD: {
				inp->size = SIZE_W;
				tok = *( tail = &( tok->next ));
				break;
			}
			case TOK_LONG: {
				inp->size = SIZE_L;
				tok = *( tail = &( tok->next ));
				break;
			}
			default: {
				//
				//	An unspecified size could be any size
				//
				break;
			}
		}
		//
		//	Gather up the arguments.  These are not instruction arguments
		//	and are more freely formed.
		//
		while( tok != NULL ) {			
			if( inp->var.assembler_op.args == DIRECTIVE_ARGS ) return( "Too many arguments for a directive" );
			//
			//	We have tokens outstanding and we have the
			//	ability to store an additional argument to
			//	the directive.
			//
			//	There are two possible sorts of arguments:
			//
			//	"Strings" and Expressions.
			//
			switch( tok->id ) {
				case TOK_TEXT:
				case TOK_DATA:
				case TOK_BSS:
				case TOK_STRING: {
					//
					//	A "String" is a single token followed
					//	by (hopefully) a comma.
					//
					inp->var.assembler_op.arg[ inp->var.assembler_op.args++ ] = tok;
					tok = *( tail = &( tok->next ));
					break;
				}
				default: {
					//
					//	This should be an expression of some
					//	sort.
					//
					if(( len = eval_expr( tok, 0, &top )) == 0 ) return( "Invalid constant expression" );
					while( len-- ) tok = tok->next;
					head = STACK( token );
					head->id = TOK_EXPRESSION;
					head->var.expr = top;
					head->next = tok;
					*tail = head;
					tail = &( head->next );
					inp->var.assembler_op.arg[ inp->var.assembler_op.args++ ] = head;
					break;
				}
			}
			//
			//	Do we have an "inter-argument" comma?
			//
			if( tok != NULL ) {
				if( tok->id != TOK_COMMA ) return( "Comma missing before new argument" );
				if(( tok = *( tail = &( tok->next ))) == NULL ) return( "Argument missing after comma" );
			}
		}
		//
		//	Arguments gathered so process the instruction.
		//
		return( process_directive( inp ));
	}
	//
	//	Getting here means the opcode does not make sense.
	//
	return( "Unrecognised instruction or directive" );
}

//
//	Process an open file in a specific pass mode.
//
static int pass_file( FILE *source ) {
	char	buffer[ BUFFER ], *msg;
	int	line, errors;

	errors = 0;
	line = 0;
	while( fgets( buffer, BUFFER, source )) {
		//
		//	light pre-processing
		//
		buffer[ strlen( buffer )-1 ] = EOS;
		line++;
		if( assembler_pass == CODE_GENERATION ) next_line( line, buffer );
		//
		//	Output line for debuggin purposes.
		//
		PRINT(( "%4d|%s\n", line, buffer ));
		//
		//	What do we have?
		//
		if(( msg = process_input( line, buffer )) != NULL ) {
			printf( "%4d|%s\n    |%s\n", line, buffer, msg );
			errors++;
		}
	}
	return( errors );
}
//
//	Implement a simplistic argument processing system
//
typedef struct {
	char		*name, *help;
	int		set, reset;
	opflags		target_cpu;
} option;
static option options[] = {
	{	"--68000",		"\t\tTarget mc68000 CPU",		OPTION_68000,		CPU_MASK,	FLAG_68000 },
	{	"--68008",		"\t\tTarget mc68008 CPU",		OPTION_68008,		CPU_MASK,	FLAG_68000 },
	{	"--68010",		"\t\tTarget mc68010 CPU",		OPTION_68010,		CPU_MASK,	FLAG_68000 | FLAG_68010	},
	{	"--68020",		"\t\tTarget mc68020 CPU",		OPTION_68020,		CPU_MASK,	FLAG_68000 | FLAG_68010 | FLAG_68020 },
	{	"--68030",		"\t\tTarget mc68030 CPU",		OPTION_68030,		CPU_MASK,	FLAG_68000 | FLAG_68010 | FLAG_68020 | FLAG_68030 | FLAG_68881 },
	{	"--68881",		"\t\tTarget mc68881 FPU",		OPTION_68881,		CPU_MASK,	FLAG_68881 },
	{	"--68882",		"\t\tTarget mc68882 FPU",		OPTION_68881,		CPU_MASK,	FLAG_68881 },
	{	"--hexadecimal",	"\tOutput text hexadecimal values",	DISPLAY_TEXT,		DISPLAY_MASK,	0 },
	{	"--intel",		"\t\tOutput Intel Hex format data",	DISPLAY_INTEL,		DISPLAY_MASK,	0 },
	{	"--motorola",		"\tOutput Motorola S records",		DISPLAY_MOTOROLA,	DISPLAY_MASK,	0 },
	{	"--listing",		"\tDisplay an assembly listing",	DISPLAY_LISTING,	DISPLAY_MASK,	0 },
	{	"--no-output",		"\tDo not output any code",		DISPLAY_NOTHING,	DISPLAY_MASK,	0 },
	{	"--stdout",		"\tSend output to console",		DISPLAY_STDOUT,		0,		0 },
	{	"--symbols",		"\tOutput Symbol table",		DISPLAY_SYMBOLS,	0,		0 },
	{	"--sections",		"\tOutput consolidated sections",	DISPLAY_SECTIONS,	0,		0 },
	{	"--dump-opcodes",	"\tDisplay op-codes table",		DISPLAY_OPCODES,	0,		0 },
	{	"--help",		"\t\tDisplay this help information",	DISPLAY_HELP,		0,		0 },
#ifdef ENABLE_DEBUG
	{	"--debug",		"\t\tEnable additonal debugging output",DISPLAY_DEBUG,		0,		0 },
#endif
	{	NULL													  }
};

//
//	Handed the whole argument list so look for the filename
//	and return its index (while gathering the arguments on the way).
//
//	Return 0 or -ve number if the program should exit as a result of
//	this processing.
//
static int parse_arguments( int argc, char **argv ) {
	int	a;
	option	*o;

	//
	//	Step through arguments..
	//
	for( a = 1; a < argc; a++ ) {
		if( strncmp( argv[ a ], "--", 2 ) == 0 ) {
			for( o = options; o->name != NULL; o++ ) {
				if( strcmp( argv[ a ], o->name ) == 0 ) break;
			}
			if( o->name != NULL ) {
				option_flags = ( option_flags & ~o->reset ) | o->set;
				target_cpu |= o->target_cpu;
			}
			else {
				fprintf( stderr, "Unrecognised option '%s'.\n", argv[ a ]);
				return( -1 );
			}
		}
		else break;
	}
	//
	//	Analyse what we found..
	//
	if( option_flags & DISPLAY_HELP ) {
		printf( "Usage: %s [ {options} ] {filename}\nOptions:-\n", argv[ 0 ]);
		for( o = options; o->name != NULL; o++ ) printf( "\t%s%s\n", o->name, o->help );
		return( 0 );
	}
	if( option_flags & DISPLAY_OPCODES ) {
		printf( "Op-codes dump:\n" );
		dump_opcodes();
		return( 0 );
	}
	if(( option_flags & DISPLAY_MASK ) == 0 ) {
		printf( "Output format required.\n" );
		return( -2 );
	}
	if(( option_flags & CPU_MASK ) == 0 ) {
		printf( "Default CPU to 68000.\n" );
		option_flags |= OPTION_68000;
	}
	return(( a >= argc )? 0: a );
}

//
//	It all starts here:
//
int main( int argc, char *argv[]) {
	FILE	*source;
	int	file, err, a;
	char	*msg;

	//
	//	Start with the arguments
	//
	if(( file = parse_arguments( argc, argv )) <= 0 ) return( -file );
	if(( source = fopen( argv[ file ], "r" )) == NULL ) {
		fprintf( stderr, "Unable to open file '%s', error '%m'.\n", argv[ file ]);
		return( 2 );
	}
	
	//
	//	Data gather pass
	//
	PRINT(( "------- DATA GATHERING -------\n" ));

	assembler_pass = DATA_GATHERING;
	reset_sections();
	if(( err = pass_file( source ))) {
		fprintf( stderr, "Pass 1: %d errors.\n", err );
		if( option_flags & DISPLAY_SYMBOLS ) dump_identifiers( stdout );
		return( 1 );
	}

	//
	//	Data verification pass(s)
	//
	a = MAXIMUM_VERIFICATIONS;
	do {
		a -= 1;
		rewind( source );

		PRINT(( "------- DATA VERIFICATION -------\n" ));

		assembler_pass = DATA_VERIFICATION;
		reset_sections();
		if(( err = pass_file( source ))) fprintf( stderr, "Pass 2: %d errors.\n", err );
	} while( err && a );

	//
	//	Did we come out a success?
	//
	if( err ) {
		//
		//	No.
		//
		fprintf( stderr, "Pass 2: Verification failed, %d errors.\n", err );
		if( option_flags & DISPLAY_SYMBOLS ) dump_identifiers( stdout );
		return( 2 );
	}

	//
	//	Code Generation pass.
	//
	if( !reconcile_sections( stdout )) {
		fprintf( stderr, "Unable to reconcile section block addresses"  );
		err = 1;
	}
	else {
		//
		//	Generate the output data.
		//
		if(( msg = init_output( argv[ file ]))) {
			//
			//	Failed to initialise output system.
			//
			fprintf( stderr, "Pass 3: Initialisation of output format failed: %s.\n", msg );
			err = 2;
		}
		else {
			rewind( source );
			
			PRINT(( "------- CODE GENERATION -------\n" ));
			
			assembler_pass = CODE_GENERATION;
			reset_sections();
			if(( err = pass_file( source ))) {
				fprintf( stderr, "Pass 3: %d errors.\n", err );
			}
			end_output();
		}
	}

	//
	//	Finally output the symbols.
	//
	if( option_flags & DISPLAY_SYMBOLS ) dump_identifiers( stdout );
	
	return( err );
}

//
//	EOF
//
