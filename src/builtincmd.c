#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "builtincmd.h"
#include "context.h"

static int builtincmd_exit(char *argv[])
{
	exit(ctx_retval);
}

static int builtincmd_cd(char *argv[])
{
	int ret = 0;
	if (!argv[1]) {
		//setenv("OLDPWD", getenv("PWD"), 1);
		//ret = setenv("PWD", getenv("HOME"), 1);
	}
	return (ret);
}

static int builtincmd_pwd(char *argv[])
{
	printf("%s\n", getenv("PWD"));
	return (0);
}

/*
 *
 * @param cmd_name
 * @return
 * @retval NULL
 */
builtincmd_t
get_builtin_command(const char *cmd_name)
{
	builtincmd_t cmd;
	cmd.exec_as_child = 0;
	cmd.exec = NULL;

	if (strcmp(cmd_name, COMMAND_EXIT) == 0) {	
		cmd.exec_as_child = 0;
		cmd.exec = &builtincmd_exit;
		return (cmd);
	}
	if (strcmp(cmd_name, COMMAND_CD) == 0) {
		cmd.exec_as_child = 0;
		cmd.exec = &builtincmd_cd;
		return (cmd);
	}
	if (strcmp(cmd_name, COMMAND_PWD) == 0) {
		cmd.exec_as_child = 1;
		cmd.exec = &builtincmd_pwd;
		return (cmd);
	}

	return (cmd);
}
