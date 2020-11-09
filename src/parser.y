%{
#include <stdio.h>
#include "context.h"
#include "arglist.h"
#include "pipeline.h"

int yylex();
%}

%define parse.error verbose
%define api.token.prefix {TKN_}

%code requires { 
#include "arglist.h" 
#include "pipeline.h"
}
%union {
	int intval;
	char *strval;
	arglist_t *arglist;
	pipeline_t *pipeline;
}

%destructor { free($$); } 				<strval>
%destructor { arglist_destruct($$); }	<arglist>

%token			EOL			"end of line"
%token			SEMIC		";"
%token			VERT		"|"
%token  		GT			">"
%token<strval>	WORD		"word"

%type			<arglist>		simple_command
%type			<arglist>		argument_list_opt
%type			<arglist>		argument_list
%type			<pipeline>		pipeline
%type			<intval>		sublist
%type			<intval>		list

%start list

%%

simple_command: WORD argument_list_opt	{ 	
											$$ = $2;
											arglist_add($2, $1);
										}
			  ;

argument_list_opt: %empty				{ $$ = arglist_construct(); }
                 | argument_list		{ $$ = $1; }
				 ;

argument_list: WORD						{ 
											$$ = arglist_construct();
											arglist_add($$, $1);
										}
             | WORD argument_list		{
			 								$$ = $2;
											arglist_add($$, $1);
										}
             ;

pipeline: simple_command				{ /* the last command in the pipeline */
											$$ = pipeline_construct();
											pipeline_add($$, $1);
										}				
        | simple_command VERT pipeline	{ /* other commands in the pipeline */
											$$ = $3;
											pipeline_add($$, $1);
										}
        ;

sublist: pipeline						{ 
											$$ = pipeline_exec($1);
											pipeline_destruct($1);
										}
       ;

list: %empty							{ }
	| sublist							{ $$ = $1; }
    | sublist SEMIC list				{ $$ = $3; }
	| sublist EOL list					{ $$ = $3; }
	;

%%

int
main(void)
{
	return yyparse();
}

