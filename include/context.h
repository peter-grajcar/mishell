#ifndef CONTEXT_H
#define CONTEXT_H

extern int ctx_retval;

void ctx_init(void);
int ctx_line(void);
int ctx_incline(void);

void yyerror(const char *fmt, ...);

#endif
