#include <stdlib.h>
#include "redirect.h"

/*
 *
 */
redirection_t *
redirection_construct(char *file, redirect_output_mode_t mode)
{
	redirection_t *redirection = malloc(sizeof (redirection_t));
	if (!redirection)
		return (NULL);
	redirection->in_file = NULL;
	redirection->out_file = NULL;
	redirection->output_mode = REDIRECT_OUTPUT_NONE;
	redirection_add(redirection, file, mode);
	return (redirection);
}

/*
 *
 */
void
redirection_add(redirection_t *redirection, char *file, redirect_output_mode_t mode)
{
	switch (mode) {
		case REDIRECT_OUTPUT_NONE:
			redirection->in_file = file;
			break;
		case REDIRECT_OUTPUT_WRITE:
		case REDIRECT_OUTPUT_APPEND:
			redirection->out_file = file;
			redirection->output_mode = mode;	
	}
}

/*
 *
 */
void
redirection_destruct(redirection_t *redirection)
{
	if (redirection->in_file)
		free(redirection->in_file);
	if (redirection->out_file)
		free(redirection->out_file);
	free(redirection);
}
