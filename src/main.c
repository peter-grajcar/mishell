#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <err.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "context.h"

// defined in context.h
int ctx_retval;

// flex variables
FILE *yyin;
typedef struct yybufferstate *YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

sigjmp_buf sigint_buf;
void signal_handler(int signo);

int
main(int argc, char *argv[])
{
	YY_BUFFER_STATE buffer;

	int opt;
	while ((opt = getopt(argc, argv, "c:")) != -1) {
		switch (opt) {
		case 'c':
			buffer = yy_scan_string(optarg);
			yyparse();
			yy_delete_buffer(buffer);
			return (ctx_retval);
		}
	}

	if (optind < argc) {
		yyin = fopen(argv[optind], "r");
		yyparse();
		return (ctx_retval);
	}

	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		err(1, "cannot set signal handler");
	}

	while (1) {
		while (sigsetjmp(sigint_buf, 1) != 0)
			;

		char *input = readline("mish$ ");

		// Exit on EOF
		if (!input) {
			printf("\n");
			exit(ctx_retval);
		}

		buffer = yy_scan_string(input);
		yyparse();
		yy_delete_buffer(buffer);
		free(input);
	}

	return (0);
}

void
signal_handler(int signo)
{
	if (signo == SIGINT) {
		write(1, "\n", 1);
		siglongjmp(sigint_buf, 2);
	}
}
