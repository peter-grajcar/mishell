%{
#include <stdio.h>
#include "parser.tab.h"
#include "context.h"
%}

%option noinput nounput
%option noyywrap

WHITESPACE	[ \t\r]

%x COMMENT
%%
%{
ctx_init();
%}

#				BEGIN(COMMENT);

<COMMENT>\n		{
					ctx_incline();	
					BEGIN(INITIAL);
				}

<COMMENT>.		/* ignore */

echo			return TKN_ECHO;

[^\n\r\t |;>]+	{
					yylval.strval = strdup(yytext);
					return TKN_WORD;
				}

{WHITESPACE}+	/* skip whitespaces */

\n				{
					ctx_incline();
					return TKN_EOL;
				}

;				return TKN_SEMIC;

\|				return TKN_VERT;

>				return TKN_GT;			

.				yyerror("unexpected character '%s'\n", yytext);

%%

