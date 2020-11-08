#include "arglist.h"
#include <stdlib.h>
#include <string.h>

arglist_t *
arglist_construct(void)
{
	arglist_t *list = malloc(sizeof (arglist_t));
	SLIST_INIT(list);
	return (list);
}

void
arglist_add(arglist_t *list, const char *arg)
{
	arglist_item_t *item = malloc(sizeof (arglist_item_t));
	item->arg = strdup(arg);
	SLIST_INSERT_HEAD(list, item, link);
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
