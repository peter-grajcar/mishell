%{
#include <stdio.h>
#include "parser.tab.h"
#include "context.h"
%}

%option noinput nounput
%option noyywrap

WORD_CHAR	[^ \t\r\n'"\\`$)(><;&|]
WHITESPACE	[ \t\r]

%x COMMENT
%x SQUOTE
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

'				BEGIN(SQUOTE);

<SQUOTE>[^']*	yymore();	

<SQUOTE>'		{
					BEGIN(INITIAL);
					yylval.strval = strdup(yytext);
					yylval.strval[yyleng - 1] = '\0';
					return TKN_WORD;
				}

{WORD_CHAR}+	{
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

>>				return TKN_DGT;

>				return TKN_GT;			

\<				return TKN_LT;

.				yyerror("unexpected character '%s'\n", yytext);

%%

