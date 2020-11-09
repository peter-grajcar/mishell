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
%token			DGT			">>"
%token			LT			"<"
%token<strval>	WORD		"word"

%type			<arglist>		simple_command
%type			<arglist>		argument_list_opt
%type			<arglist>		argument_list
%type			<pipeline>		pipeline
%type			<intval>		sublist
%type			<intval>		list

%start complex_command

%%

simple_command: WORD argument_list_opt redirection_list_opt	{ 	
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

redirection_list: redirection_type WORD						{ free($2); }
				| redirection_list redirection_type WORD 	{ free($3); }
				;

redirection_list_opt: %empty
					| redirection_list
					;

redirection_type: LT
				| GT
				| DGT
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
    | list separator sublist			{ $$ = $3; }
	;

complex_command: list
	   		   | list separator
	   		   ;

separator: newlines
         | SEMIC newlines_opt
		 ;

newlines_opt: %empty
            | newlines
			;

newlines: EOL
        | newlines EOL
		;

%%

