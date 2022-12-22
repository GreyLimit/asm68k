/*
 *	The asm68k opcode table pre-processor.
 */

#include <stdio.h>
#include <alloca.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

/*
 *	Dumb buffer size.
 */
#define BUFFER 200

/*
 *	Wrap up malloc().
 */
#define NEW(t)	((t *)malloc(sizeof(t)))

/*
 *	Other dumb stuff.
 */
#define ERROR	(-1)
#define TRUE	(0==0)
#define FALSE	(0==1)
#define EOS	'\0'
#define COMMA	','
#define NL	'\n'
#define COMMENT	'#'

/*
 *	This makes bold assumptions about how the target
 *	program is going to use the output data.  There is
 *	no testing or linkage between this program and the
 *	final program using the output of this program.
 *
 *	To minimise the actual linkage, this program makes
 * 	the following assumptions about the input:
 *
 *	Each row is a comma separated list of values.
 *
 *	The number of values and content is totally down to
 *	the input file.
 *
 *	The first value, while NOT provided in doubles quotes
 *	is assumed to be the key value, and is used to
 *	build and sort the lookup tree.
 */

/*
 *	Maximum number of arguments on a single line, after
 *	the key value.
 */
#define MAX_ARGS	10

/*
 *	Define the name and opcode templates.
 */
#define TEMPLATE_BUFFER	20
#define NAME_TEMPLATE	"mnemonic_%03d"
#define OPCODE_TEMPLATE	"opcode_%03d"

/*
 *	Record used to keep a single line of input.
 */
typedef struct _opcode_ {
	char		*key;			/* Key for this record. */
	int		args;			/* Number of following args */
	char		*arg[ MAX_ARGS ];	/* The arguments */
	/*
	 *	Linkage pointers
	 */
	struct _opcode_	*next,			/* records with same key. */
			*before,		/* records before this key */
			*after,			/* records after this key */
			*list;			/* Initial build list. */
	/*
	 *	cross reference index.
	 */
	int		number;			/* Unique ref number */
} opcode;

/*
 *	Output the names tree and the opcodes lists.
 */
static opcode *generate_names( opcode *node ) {
	if( node == NULL ) return( NULL );

	node->before = generate_names( node->before );
	node->after = generate_names( node->after );
	
	printf( "static mnemonic " NAME_TEMPLATE " = { \"%s\", &" OPCODE_TEMPLATE, node->number, node->key, node->number );
	if( node->before ) {
		printf( ", &" NAME_TEMPLATE, node->before->number );
	}
	else {
		printf( ", NULL" );
	}
	if( node->after ) {
		printf( ", &" NAME_TEMPLATE, node->after->number );
	}
	else {
		printf( ", NULL" );
	}
	printf( " };\n" );
	return( node );
}

static opcode *generate_opcodes_list( opcode *node ) {
	int	a;
	
	if( node == NULL ) return( NULL );
	
	node->next = generate_opcodes_list( node->next );
	
	printf( "static opcode " OPCODE_TEMPLATE " = {", node->number );
	for( a = 0; a < node->args; a++ ) {
		if( a ) printf( "," );
		printf( " %s", node->arg[ a ]);
	}
	if( node->next ) {
		printf( ", &" OPCODE_TEMPLATE " };\n", node->next->number );
	}
	else {
		printf( ", NULL };\n" );
	}
	return( node );
}

static opcode *generate_opcodes( opcode *node ) {
	opcode	*look;

	if( node == NULL ) return( NULL );
	node->before = generate_opcodes( node->before );
	node->after = generate_opcodes( node->after );
	return( generate_opcodes_list( node ));
}

/*
 *	return number of items in list.
 */
static int listlen( opcode *list ) {
	int 	i;

	i = 0;
	while( list != NULL ) {
		i++;
		list = list->list;
	}
	return( i );
}

/*
 *	Convert a list into a tree.
 */
static opcode *maketree( opcode *list ) {
	opcode	*look, *node;
	int	len;

	if(( len = listlen( list )) <= 1 ) return( list );
	
	len = len / 2 - 1;
	
	for( look = list; len-- > 0; look = look->list );
	
	node = look->list;
	look->list = NULL;
	node->after = maketree( node->list );
	node->before = maketree( list );
	node->list = NULL;
	
	return( node );
}

/*
 *	A simple merge list.
 */
static opcode *mergelist( opcode *a, opcode *b ) {
	opcode	*head, **tail;

	tail = &head;
	while(( a != NULL )&&( b != NULL )) {
		if( strcmp( a->key, b->key ) < 0 ) {
			*tail = a;
			tail = &( a->list );
			a = a->list;
		}
		else {
			*tail = b;
			tail = &( b->list );
			b = b->list;
		}
	}
	while( a != NULL ) {
		*tail = a;
		tail = &( a->list );
		a = a->list;
	}
	while( b != NULL ) {
		*tail = b;
		tail = &( b->list );
		b = b->list;
	}
	*tail = NULL;
	return( head );
}

/*
 *	A simple list sort.
 */
static opcode *sortlist( opcode *list ) {
	opcode	*a, *b, *c;
	int	i;

	if( list == NULL ) return( NULL );
	if( list->list == NULL ) return( list );
	a = NULL;
	b = NULL;
	i = TRUE;
	while(( c = list ) != NULL ) {
		list = list->list;
		if(( i = !i )) {
			c->list = a;
			a = c;
		}
		else {
			c->list = b;
			b = c;
		}
	}
	return( mergelist( sortlist( a ), sortlist( b )));	
}

/*
 *	Convert simple list into tree and lists sorted and balanced.
 */
static opcode *organise( opcode *list ) {
	opcode	*find;

	/*
	 *	Go through list and move all duplicate mnemonics
	 * 	to a common record with the duplicates hanging off
	 *	the next link.
	 */
	for( find = list; find != NULL; find = find->list ) {
		opcode	**adrs, *look;
		
		adrs = &( find->list );
		while(( look = *adrs ) != NULL ) {
			if( strcmp( find->key, look->key ) == 0 ) {
				look->next = find->next;
				find->next = look;
				*adrs = look->list;
				look->list = NULL;
			}
			else {
				 adrs = &( look->list );
			 }
		 }
	}
	/*
	 *	Now sort the list of unique mnemonic record, linked
	 *	with the list pointer
	 */
	list = sortlist( list );
	/*
	 *	Make into a tree.
	 */
	return( maketree( list ));
}

/*
 *	Convert a line of data and return a pointer to an completed
 *	but unlinked opcode record, or NULL on error
 */
static opcode *convert( char *str, int line ) {
	opcode	*p;
	int	i;

	p = NEW( opcode );
	memset( p, 0, sizeof( opcode ));
	p->number = line;

	for( i = 0; ( i <= MAX_ARGS )&&( str != NULL ); i++ ) {
		char	*c, *e, *f;

		if(( c = strchr( str, COMMA )) != NULL ) *c++ = EOS;
		while( isspace( *str )) str++;
		for( e = str; *e != EOS; e++ ) {
			if( isspace( *e )) {
				for( f = e; ( *f != EOS )&&( isspace( *f )); f++ );
				if( *f == EOS ) {
					*e = EOS;
					break;
				}
			}
		}
		p->args = i;
		if( i ) {
			p->arg[ i-1 ] = strdup( str );
		}
		else {
			char	*s;

			for( s = str; *s != EOS; s++ ) if( islower( *s )) *s = toupper( *s );
			p->key = strdup( str );
		}
		str = c;
	}
	return( p );
}

/*
 *	No arguments, process input and produces output.
 */
int main( int argc, char *argv[]) {
	char	buffer[ BUFFER ];
	
	opcode	*head, **tail;

	int	line;

	line = 0;
	tail = &head;
	while( fgets( buffer, BUFFER, stdin ) != NULL ) {
		opcode	*p;
		char	*s;

		line++;
		for( s = buffer; ( *s != EOS )&&( *s != NL )&&( isspace( *s )); s++ );
		if(( *s != COMMENT )&&( *s != EOS )&&( *s != NL )) {
			if(( p = convert( s, line )) != NULL ) {
				*tail = p;
				tail = &( p->list );
			}
			else {
				fprintf( stderr, "Error on line %d.\n", line );
				return( 1 );
			}
		}
	}
	*tail = NULL;

	head = generate_names( generate_opcodes( organise( head )));

	printf( "#define ROOT_NODE " NAME_TEMPLATE "\n", head->number );
	
	return( 0 );
}

/*
 *	EOF
 */
