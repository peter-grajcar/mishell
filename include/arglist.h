#ifndef	ARGLIST_H
#define	ARGLIST_H

#include <sys/queue.h>

struct arglist_item {
	char *arg;
	STAILQ_ENTRY(arglist_item) link;
};

struct arglist {
	int argc;
	STAILQ_HEAD(arglist_argv, arglist_item) argv;
};

typedef struct arglist_item arglist_item_t;
typedef struct arglist arglist_t;

arglist_t *arglist_construct(void);
int arglist_add(arglist_t *list, char *arg);
char **arglist_as_array(arglist_t *list);
void arglist_destruct(arglist_t *list);


#endif
