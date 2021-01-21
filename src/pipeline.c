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
 * Constructs a new command.
 *
 * @param args command arguments.
 * @param redirect file redirection information.
 * @return newly created command.
 * @retval NULL could not allocate memory for the command.
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
 * Destroys command. Use this function to safely free the allocated memory by
 * the command structure.
 *
 * @param cmd command to destroy.
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
 * Opens a file for output redirection.
 *
 * @param redirection file redirection information.
 */
static int
open_redirection_out(redirection_t *redirect)
{
	if (!redirect || !redirect->out_file)
		return (-1);

	int flags =  O_WRONLY | O_CREAT;
	if (redirect->output_mode == REDIRECT_OUTPUT_APPEND)
		flags |= O_APPEND;

	int fd = open(redirect->out_file, flags, 0664);

	return (fd);
}

/*
 * Opens a file for input redirection.
 *
 * @param redirection file redirection information.
 */
static int
open_redirection_in(redirection_t *redirect)
{
	if (!redirect || !redirect->in_file)
		return (-1);

	int fd = open(redirect->in_file, O_RDONLY);

	return (fd);
}

/*
 * Executes given pipeline.
 *
 * @return pipeline return code.
 */
int
pipeline_exec(pipeline_t *pipeline)
{
	// file descriptors of the left and the right side of a command in the
	// pipeline
	int left[2] = {0, 1};
	int right[2];
	// pipeline return value
	int retval;

	command_t *cmd;
	TAILQ_FOREACH(cmd, pipeline, link) {
		// redirection file descriptors
		int in_fd = 0, out_fd = 1;

		char **argv = arglist_as_array(cmd->args);

		if (TAILQ_NEXT(cmd, link)) {
			pipe(right);
		} else {
			// the last command in the pipeline
			right[0] = 0;
			right[1] = 1;
		}

		builtincmd_t builtin_cmd = get_builtin_command(argv[0]);

		// execute builtin command which cannot be run as a child
		// process
		if (builtin_cmd.exec && !builtin_cmd.exec_as_child) {
			// set input file descriptor
			in_fd = open_redirection_in(cmd->redirect);
			if (in_fd == -1)
				in_fd = left[0];
			// set output file descriptor
			out_fd = open_redirection_out(cmd->redirect);
			if (out_fd == -1)
				out_fd = right[1];

			cmd->retval = builtin_cmd.exec(argv, in_fd, out_fd);

			free(argv);

			continue;
		}
		cmd->pid = fork();
		if (cmd->pid == -1) {
			err(1, "could not fork a new process");
			return (1);
		} else if (cmd->pid == 0) {
			// child process

			// sets input file descriptor
			if (cmd->redirect && cmd->redirect->in_file) {
				in_fd = open_redirection_in(cmd->redirect);
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

			// sets output file descriptor
			if (cmd->redirect && cmd->redirect->out_file) {
				out_fd = open_redirection_out(cmd->redirect);
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
				exit(retval);
			}

			retval = execvp(argv[0], argv);
			if (errno == ENOENT)
				errx(127, "command not found: %s", argv[0]);
			if (errno == EACCES)
				errx(1, "access denied: %s", argv[0]);

			exit(retval);
			// end of child process
		}

		// close file descriptors on the left side but do not close
		// stdout and stdin
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
