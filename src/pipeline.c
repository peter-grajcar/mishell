#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "pipeline.h"

pipeline_t *
pipeline_construct(void)
{
	pipeline_t *pipeline = malloc(sizeof (pipeline_t));
	if (!pipeline)
		return (NULL);
	TAILQ_INIT(pipeline);
	return (pipeline);
}

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
