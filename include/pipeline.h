#ifndef PIPELINE_H
#define PIPELINE_H

#include <unistd.h>
#include <sys/queue.h>
#include "arglist.h"

struct command {
	pid_t pid;
	arglist_t* args;
	int *pipe_in;
	int *pipe_out;
	LIST_ENTRY(command) link;	
};

typedef struct command command_t;
typedef LIST_HEAD(pipeline, command) pipeline_t;

pipeline_t *pipeline_construct(void);
int pipeline_add(pipeline_t *pipeline, arglist_t *cmd);
int pipeline_execute(pipeline_t *pipeline);
void pipeline_destruct(pipeline_t *pipeline);

#endif
