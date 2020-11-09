#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "pipeline.h"

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
 * @retval ENOMEM could not allocate memory for command_t structure.
 */
int
pipeline_add(pipeline_t *pipeline, arglist_t *arglist)
{
	command_t *cmd = malloc(sizeof (command_t));
	if (!cmd)
		return (ENOMEM);
	cmd->args = arglist;
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
	command_t *cmd;
	TAILQ_FOREACH(cmd, pipeline, link) {
		printf("exec(");
		arglist_item_t *arg;
		STAILQ_FOREACH(arg, cmd->args, link) {
			printf("%s ", arg->arg);
		}
		printf(")\n");
	}
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
		arglist_destruct(cmd->args);
		free(cmd);
		cmd = cmd_next;
	}
	free(pipeline);
}
