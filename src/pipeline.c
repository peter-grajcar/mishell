#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <sys/wait.h>
#include "pipeline.h"
#include "builtincmd.h"

/*
 *
 *
 * @param args
 * @param redirect
 * @return
 * @retval NULL
 */
command_t *
command_construct(arglist_t *args, redirection_t *redirect)
{
	command_t *cmd = malloc(sizeof (command_t));
	if (!cmd)
		return (NULL);
	cmd->pid = -1;
	cmd->retval = 0;
	cmd->args = args;
	cmd->redirect = redirect;
	return (cmd);
}
/*
 *
 *
 * @param cmd
 */
void
command_destruct(command_t *cmd)
{
	arglist_destruct(cmd->args);
	if (cmd->redirect)
		redirection_destruct(cmd->redirect);
	free(cmd);
}

/*
 * Constructs a new pipeline structure.
 *
 * @return new pipeline.
 */
pipeline_t *
pipeline_construct(void)
{
	pipeline_t *pipeline = malloc(sizeof (pipeline_t));
	if (!pipeline)
		return (NULL);
	TAILQ_INIT(pipeline);
	return (pipeline);
}

/*
 * Adds a new command to the beginning of the pipeline.
 *
 * @param arglist simple command.
 * @return 0 if succesful, otherwise error code.
 */
int
pipeline_add(pipeline_t *pipeline, command_t *cmd)
{
	TAILQ_INSERT_HEAD(pipeline, cmd, link);
	return (0);
}

/*
 * Executes given pipeline.
 *
 * @return pipeline return code.
 */
int
pipeline_exec(pipeline_t *pipeline)
{
	int left[2] = {0, 1};
	int right[2];
	int retval;

	command_t *cmd;
	TAILQ_FOREACH(cmd, pipeline, link) {
		int in_fd, out_fd;
		char **argv = arglist_as_array(cmd->args);

		if (TAILQ_NEXT(cmd, link)) {
			pipe(right);
		} else {
			right[0] = 0;
			right[1] = 1;
		}

		builtincmd_t builtin_cmd = get_builtin_command(argv[0]);

		// note that commands which are not executed as children
		// cannot output anything!
		if (builtin_cmd.exec && !builtin_cmd.exec_as_child) {
			cmd->retval = builtin_cmd.exec(argv);
			free(argv);
			continue;
		} else {
			switch (cmd->pid = fork()) {
				case -1:
					/* TODO: error message */
					return (1);
				case 0:
					if (cmd->redirect && cmd->redirect->in_file) {
						in_fd = open(cmd->redirect->in_file, O_RDONLY);
						if (in_fd == -1) {
							errx(1,
								"cannot open file: %s\n",
								cmd->redirect->in_file);
						}
						close(0);
						dup(in_fd);
						close(in_fd);
					} else if (left[0] != 0) {
						close(0);
						dup(left[0]);
						close(left[0]);
						close(left[1]);
					}

					if (cmd->redirect && cmd->redirect->out_file) {
						out_fd = open(cmd->redirect->out_file, O_WRONLY | O_CREAT | (cmd->redirect->output_mode == REDIRECT_OUTPUT_APPEND ? O_APPEND : 0), 0664);
						if (out_fd == -1) {
							errx(1,
								"cannot open file: %s\n",
								cmd->redirect->out_file);
						}
						close(1);
						dup(out_fd);
						close(out_fd);
					} else if (right[1] != 1) {
						close(1);
						dup(right[1]);
						close(right[0]);
						close(right[1]);
					}

					if (builtin_cmd.exec) {
						retval = builtin_cmd.exec(argv);
					} else {
						retval = execvp(argv[0], argv);
						if (errno == ENOENT)
							errx(127, "command not found: %s", argv[0]);
						if (errno == EACCES)
							errx(1, "access denied: %s", argv[0]);
					}
					exit(retval);
			}
		}


		// do not close stdout stdin
		if (left[0] != 0)
			close(left[0]);
		if (left[1] != 1)
			close(left[1]);
		left[0] = right[0];
		left[1] = right[1];

		free(argv);
	}

	TAILQ_FOREACH(cmd, pipeline, link) {
		if (cmd->pid != -1) {
			waitpid(cmd->pid, &retval, 0);
			cmd->retval = retval;
		} else {
			// non exec_as_child commands set their
			// return value in command structure
			retval = cmd->retval;
		}
	}

	if (WIFEXITED(retval))
		return (WEXITSTATUS(retval));

	return (0);
}

/*
 * Destructs pipeline. Call this function to safely free all the memory
 * allocated by the pipeline and associated structures.
 *
 * @param pipeline pipeline to destruct.
 */
void
pipeline_destruct(pipeline_t *pipeline)
{
	command_t *cmd;
	cmd = TAILQ_FIRST(pipeline);
	while (cmd != NULL) {
		command_t *cmd_next = TAILQ_NEXT(cmd, link);
		command_destruct(cmd);
		cmd = cmd_next;
	}
	free(pipeline);
}
