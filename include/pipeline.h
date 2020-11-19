#ifndef PIPELINE_H
#define	PIPELINE_H

#include <unistd.h>
#include <sys/queue.h>
#include "arglist.h"
#include "redirect.h"

struct command {
	pid_t pid;
	int retval;
	arglist_t *args;
	redirection_t *redirect;
	int *pipe_in;
	int *pipe_out;
	TAILQ_ENTRY(command) link;
};

typedef struct command command_t;
typedef TAILQ_HEAD(pipeline, command) pipeline_t;

command_t *command_construct(arglist_t *args, redirection_t *redirect);
void command_destruct(command_t *cmd);

pipeline_t *pipeline_construct(void);
int pipeline_add(pipeline_t *pipeline, command_t *cmd);
int pipeline_exec(pipeline_t *pipeline);
void pipeline_destruct(pipeline_t *pipeline);

#endif
