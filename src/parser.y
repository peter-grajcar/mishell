%{
#include <stdio.h>
#include "context.h"
#include "arglist.h"

int yylex();
%}

%define parse.error verbosed
%define api.token.prefix {TKN_}

%code requires { #include "arglist.h" }
%union {
	int intval;
	char *strval;
	arglist_t *arglist;
}

%destructor { free($$); } 				<strval>
%destructor { arglist_destruct($$); }	<arglist>

%token			EOL			"end of line"
%token			SEMIC		";"
%token			VERT		"|"
%token  		GT			">"
%token<strval>	WORD		"word"
%token			ECHO		"echo"

%type			<intval>		simple_command
%type			<intval>		pipeline
%type			<intval>		sublist
%type			<intval>		list
%type			<arglist>		argument_list_opt
%type			<arglist>		argument_list

%start list

%%

simple_command: WORD argument_list_opt	{ 	printf("exec(%s", $1);
											
											if ($2) {
												arglist_item_t *item;
												SLIST_FOREACH(item, $2, link) {
													printf(" %s", item->arg);
												}
												arglist_destruct($2);
											}

											printf(")\n");

											free($1);
										}
              | ECHO argument_list_opt	{ }
			  ;

argument_list_opt: %empty				{ $$ = NULL; }
                 | argument_list		{ $$ = $1; }
				 ;

argument_list: WORD						{ $$ = arglist_construct(); arglist_add($$, $1); }
             | WORD argument_list		{ $$ = $2; arglist_add($$, $1); }
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

