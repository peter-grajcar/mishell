#include "arglist.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/*
 * Constructs a new argument list.
 *
 * @return an empty argument list.
 */
arglist_t *
arglist_construct(void)
{
	arglist_t *list = malloc(sizeof (arglist_t));
	if (!list)
		return (NULL);
	list->argc = 0;
	STAILQ_INIT(&list->argv);
	return (list);
}

/*
 * Adds a new argument to the beginning of the argument list.
 *
 * @param list an argument list.
 * @param arg a new argument to add.
 * @return 0 if success, otherwise error code
 * @retval ENOMEM could not allocate argument list item.
 */
int
arglist_add(arglist_t *list, char *arg)
{
	arglist_item_t *item = malloc(sizeof (arglist_item_t));
	if (!item)
		return (ENOMEM);
	item->arg = arg;
	list->argc++;
	STAILQ_INSERT_HEAD(&list->argv, item, link);
	return (0);
}

/*
 * Returns an array representation of the argument list. The list is null
 * terminated.
 *
 * @return a null terminated array of arguments.
 * @retval NULL could not allocate memory for the array.
 */
char **
arglist_as_array(arglist_t *list)
{
	arglist_item_t *item;

	char **arr = malloc((list->argc + 1) * sizeof (char *));
	if (!arr)
		return (NULL);

	int i = 0;
	STAILQ_FOREACH(item, &list->argv, link) {
		arr[i] = item->arg;
		++i;
	}
	arr[list->argc] = NULL;

	return (arr);
}

/*
 * Destructs an argument list. Call this function to safely free all the memory
 * associated with arglist.
 *
 * @param list a list to destruct.
 */
void
arglist_destruct(arglist_t *list)
{
	arglist_item_t *item = STAILQ_FIRST(&list->argv);
	while (item) {
		arglist_item_t *item_next = STAILQ_NEXT(item, link);
		free(item->arg);
		free(item);
		item = item_next;
	}
	free(list);
}
