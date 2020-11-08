#include <stdio.h>
#include <stdarg.h>
#include "context.h"

static int lineno;

void
ctx_init(void)
{
	lineno = 0;
}

int
ctx_line(void)
{
	return (lineno);
}

int
ctx_incline(void)
{
	return (lineno++);
}

void
yyerror(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	fprintf(stderr, "error:%d: ", lineno);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
}
