#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include "builtincmd.h"
#include "context.h"

static int builtincmd_exit(char *argv[], ...)
{
	exit(ctx_retval);
}

static int builtincmd_cd(char *argv[], ...)
{
	char *path = argv[1];

	va_list vargs;
	va_start(vargs, argv);
	va_arg(vargs, int); // fd_in is not used
	int fd_out = va_arg(vargs, int);
	va_end(vargs);

	if (argv[1] && argv[2]) {
		err(1, "cd: too many arguments");
		return (1);
	}

	if (!path) {
		path = getenv("HOME");
		if (!path) {
			err(0, "cd: $HOME not set");
			return (0);
		}
	}

	if (strcmp(path, "-") == 0) {
		path = getenv("OLDPWD");
		if (!path) {
			err(0, "cd: $OLDPWD not set");
			return (0);
		}
		char *tmp = strdup(getenv("PWD"));
		setenv("PWD", path, 1);

		// write new pwd
		write(fd_out, path, strlen(tmp));
		write(fd_out, "\n", 1);

		if (!tmp) {
			err(1, "cd: could not set $OLDPWD");
			return (1);
		}

		setenv("OLDPWD", tmp, 1);
		free(tmp);

		return (0);
	}

	struct stat path_stat;
	int status = stat(path, &path_stat);

	if (status == -1) {
		err(1, "cd: %s", path);
		return (1);
	}

	if (!S_ISDIR(path_stat.st_mode)) {
		err(1, "cd: %s: Not a directory", path);
		return (1);
	}

	if (getenv("PWD"))
		setenv("OLDPWD", getenv("PWD"), 1);

	int retval = setenv("PWD", path, 1);

	return (retval);
}

static int builtincmd_pwd(char *argv[], ...)
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
