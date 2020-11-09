#include "arglist.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>

arglist_t *
arglist_construct(void)
{
	arglist_t *list = malloc(sizeof (arglist_t));
	if (!list)
		return (NULL);
	SLIST_INIT(list);
	return (list);
}

int
arglist_add(arglist_t *list, char *arg)
{
	arglist_item_t *item = malloc(sizeof (arglist_item_t));
	if (!item)
		return (ENOMEM);
	item->arg = arg;
	SLIST_INSERT_HEAD(list, item, link);
	return (0);
}

void
arglist_destruct(arglist_t *list)
{
	while (!SLIST_EMPTY(list)) {
		arglist_item_t *item = SLIST_FIRST(list);
		SLIST_REMOVE_HEAD(list, link);
		free(item->arg);
		free(item);
	}
	free(list);
}
