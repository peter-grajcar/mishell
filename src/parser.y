%{
#include <stdio.h>
#include "context.h"
#include "arglist.h"
#include "pipeline.h"
#include "redirect.h"

int yylex();
%}

%define parse.error verbose
%define api.token.prefix {TKN_}

%code requires {
#include "arglist.h"
#include "pipeline.h"
#include "redirect.h"
}
%union {
	int intval;
	char *strval;
	command_t *command;
	pipeline_t *pipeline;
	struct {
	arglist_t *arglist;
	redirection_t *redirection;
	} command_suffix;
	redirection_t *redirection;
	redirect_output_mode_t output_mode;
}

%destructor 	{
			free($$);
		} <strval>
%destructor 	{
			arglist_destruct($$.arglist);
			redirection_destruct($$.redirection);
		} <command_suffix>
%destructor 	{
			command_destruct($$);
		} <command>
%destructor 	{
			pipeline_destruct($$);
		} <pipeline>
%destructor 	{
			redirection_destruct($$);
		} <redirection>

%token			EOL	"end of line"
%token			SEMIC	";"
%token			VERT	"|"
%token<output_mode>	GT	">"
%token<output_mode>	DGT	">>"
%token<output_mode>	LT	"<"
%token<strval>		WORD	"word"

%type<command>		simple_command
%type<command_suffix>	argument_list_opt
%type<command_suffix>	argument_list
%type<redirection>	redirection_list_opt
%type<redirection>	redirection_list
%type<output_mode>	redirection_mode
%type<pipeline>		pipeline

%start complex_command

%%

simple_command
	: redirection_list_opt WORD redirection_list_opt argument_list_opt
	{
		arglist_add($4.arglist, $2);
		$3 = redirection_merge($1, $3);
		$4.redirection = redirection_merge($3, $4.redirection);
		$$ = command_construct($4.arglist, $4.redirection);
	}
	;

argument_list_opt
	: %empty
	{
		$$.arglist = arglist_construct(); $$.redirection = NULL;
	}
	| argument_list
	{
		$$ = $1;
	}
	;

argument_list
	: WORD redirection_list_opt
	{
		$$.arglist = arglist_construct();
		$$.redirection = $2;
		arglist_add($$.arglist, $1);
	}
	| WORD redirection_list_opt argument_list
	{
		arglist_add($3.arglist, $1);
		$$.arglist = $3.arglist;
		$$.redirection = redirection_merge($2, $3.redirection);
	}
	;

redirection_list
	: redirection_mode WORD
			{
			$$ = redirection_construct($2, $1);
			}
	| redirection_list redirection_mode WORD
	{
		$$ = $1;
		redirection_add($1, $3, $2);
	}
	;

redirection_list_opt
	: %empty
	{
		$$ = NULL;
	}
	| redirection_list
	{
		$$ = $1;
	}
	;

redirection_mode
	: LT
	| GT
	| DGT
	;

pipeline
	: simple_command
	{ /* the last command in the pipeline */
		$$ = pipeline_construct();
		pipeline_add($$, $1);
	}
	|	simple_command VERT pipeline
	{ /* other commands in the pipeline */
		$$ = $3;
		pipeline_add($$, $1);
	}
	;

sublist
	: pipeline
	{
		ctx_retval = pipeline_exec($1);
		pipeline_destruct($1);
	}
	;

list
	: %empty
	| sublist
	| list separator sublist
	;

complex_command
	: list
	| list separator
	;

separator
	: newlines
	| SEMIC newlines_opt
	;

newlines_opt
	: %empty
	| newlines
	;

newlines
	: EOL
	| newlines EOL
	;

%%

