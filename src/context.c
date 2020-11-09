#include <stdio.h>
#include <stdarg.h>
#include "context.h"

static int lineno;

/*
 * Initialises line number to zero.
 */
void
ctx_init(void)
{
	lineno = 0;
}

/*
 * Returns the current line number.
 *
 * @return current line number.
 */
int
ctx_line(void)
{
	return (lineno);
}

/*
 * Increments the currrent line number.
 *
 * @return incremented line number.
 */
int
ctx_incline(void)
{
	return (lineno++);
}

/*
 * Prints lexer and parser error messages. This function is required by bison.
 *
 * @param fmt format.
 */
void
yyerror(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	fprintf(stderr, "error:%d: ", lineno);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
}
