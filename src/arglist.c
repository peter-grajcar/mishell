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
	STAILQ_INIT(list);
	return (list);
}

int
arglist_add(arglist_t *list, char *arg)
{
	arglist_item_t *item = malloc(sizeof (arglist_item_t));
	if (!item)
		return (ENOMEM);
	item->arg = arg;
	STAILQ_INSERT_HEAD(list, item, link);
	return (0);
}

void
arglist_destruct(arglist_t *list)
{
	arglist_item_t *item = STAILQ_FIRST(list);
	while (item) {
		arglist_item_t *item_next = STAILQ_NEXT(item, link);
		free(item->arg);
		free(item);
		item = item_next;
	}
	free(list);
}
