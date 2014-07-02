/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
static void comment(void);
int commentDepth = 0;
%}

/*
 * Define names for regular expressions here.
 */
D   			[0-9]
NZ  			[1-9]
SL  			[a-z]
CL  			[A-Z]  
L   			[a-zA-Z_]
A   			[a-zA-Z_0-9]
WS  			[ \t\v\n\f\r]
TRUE 			t(?i:rue)
FALSE 			f(?i:alse)

STR_CONST       "{L}({A}|{WS}*)"
INT_CONST       0|{NZ}{D}*
TYPEID          {CL}{A}*
OBJECTID        ({SL}|_){A}*

DARROW  		=>
ASSIGN			<-
LE				<=
%x				COMMENT
%%
{INT_CONST}		{ cool_yylval.symbol = inttable.add_string(yytext); return (INT_CONST); }
{TRUE}			{ cool_yylval.boolean = 1; return (BOOL_CONST); }
{FALSE}			{ cool_yylval.boolean = 0; return (BOOL_CONST); }

 /*
  *  Nested comments
  */
<INITIAL,COMMENT>"(*"		{ BEGIN(COMMENT); commentDepth++; }
"--".*      { /* consume //commnet */ }

<INITIAL,COMMENT>"*)" { 
	commentDepth--;
	if(commentDepth == 0) {
		BEGIN(INITIAL);
	} else if(commentDepth < 0){
		cool_yylval.error_msg="Unmatched *)";
		return (ERROR);
	} 
}
<COMMENT><<EOF>>	{ cool_yylval.error_msg="EOF in comment"; return (ERROR); }
<COMMENT>. { /* do nothing */ }

 /*
  *  The multiple-character operators.
  */
{DARROW}    { return (DARROW); }
{LE}        { return (LE); }
{ASSIGN}    { return (ASSIGN); }

 /*
  *  may not neccessary
  */
";"         { return ';'; }
"{"         { return '{'; }
"}"         { return '}'; }
","         { return ','; }
":"         { return ':'; }
"="         { return '='; }
"("         { return '('; }
")"         { return ')'; }
"["         { return '['; }
"]"         { return ']'; }
"."         { return '.'; }
"&"         { return '&'; }
"!"         { return '!'; }
"~"         { return '~'; }
"-"         { return '-'; }
"+"         { return '+'; }
"*"         { return '*'; }
"/"         { return '/'; }
"%"         { return '%'; }
"<"         { return '<'; }
">"         { return '>'; }
"^"         { return '^'; }
"|"         { return '|'; }
"?"         { return '?'; }
\n			{ ++curr_lineno; }

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */

"class"     { return (CLASS); }
"else"      { return (ELSE); }
"fi"        { return (FI); }
"if"        { return (IF); }
"in"        { return (IN); }
"inherits"  { return (INHERITS); }
"let"       { return (LET); }
"loop"      { return (LOOP); }
"pool"      { return (POOL); }
"then"      { return (THEN); }
"while"     { return (WHILE); }
"case"      { return (CASE); }
"esac"      { return (ESAC); }
"of"        { return (OF); }
"new"       { return (NEW); }
"isvoid"    { return (ISVOID); }
"not"       { return (NOT); }

{TYPEID}		{ cool_yylval.symbol = idtable.add_string(yytext); return (TYPEID); }
{OBJECTID}		{ cool_yylval.symbol = idtable.add_string(yytext); return (OBJECTID); }
 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */
{WS}        { /*do nothing*/ }

%%

static void comment(void)
{
  int c;
  while( (c = yylex()) != 0 ) {
    if( c == '*'){
      while( (c = yylex()) == '*' ) ;
      if (c == '/')
        return;
      if (c == 0)
        break;
      
    }
  }
  //deal with error 
  //return (ERROR);
  cool_yylval.error_msg = "EOF in comment";
}
