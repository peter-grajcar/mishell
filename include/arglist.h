#ifndef ARGLIST_H
#define ARGLIST_H

#include <sys/queue.h>

struct arglist_item {
	char *arg;
	STAILQ_ENTRY(arglist_item) link;
};

typedef struct arglist_item arglist_item_t;
typedef STAILQ_HEAD(arglist, arglist_item) arglist_t;

arglist_t *arglist_construct(void);
int arglist_add(arglist_t *list, char *arg);
void arglist_destruct(arglist_t *list);

#endif
