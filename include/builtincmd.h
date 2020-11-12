#ifndef BUILTINCMD_H
#define BUILTINCMD_H

#define	COMMAND_CD		"cd"
#define	COMMAND_PWD		"pwd"
#define	COMMAND_EXIT	"exit"

struct builtincmd {
	int exec_as_child;
	int (*exec)(char *argv[]);
};

typedef struct builtincmd builtincmd_t;

builtincmd_t get_builtin_command(const char *cmd_name);

#endif
