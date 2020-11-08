%{
#include <stdio.h>
#include "context.h"

int yylex();
%}

%define parse.error detailed
%define api.token.prefix {TKN_}

%union {
	int intval;
	char *strval;
}

%destructor { free($$); } 	<strval>

%token			EOL			"end of line"
%token			SEMIC		";"
%token			VERT		"|"
%token  		GT			">"
%token<strval>	WORD		"word"
%token			ECHO		"echo"

%type			<intval>	simple_command
%type			<intval>	pipeline
%type			<intval>	sublist
%type			<intval>	list

%start list

%%

simple_command: WORD argument_list_opt	{ printf("exec(%s)\n", $1); }
              | ECHO argument_list_opt	{ }
			  ;

argument_list_opt: %empty				{ }
                 | argument_list		{ }
				 ;

argument_list: WORD						{ printf("\t%s\n", $1); }
             | WORD argument_list		{ printf("\t%s\n", $1); }
             ;

pipeline: simple_command				{ }				
        | simple_command VERT pipeline	{ }
        ;

sublist: pipeline						{ }
       ;

list: %empty							{ }
	| sublist							{ }
    | sublist SEMIC list				{ }
	| sublist EOL list					{ }
	;

%%

int
main(void)
{
	return yyparse();
}

