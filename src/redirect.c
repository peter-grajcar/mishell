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
			redirection->input_file = file;
			break;
		case REDIRECT_OUTPUT_WRITE:
		case REDIRECT_OUTPUT_APPEND:
			redirection->output_file = file;
			redirection->output_mode = mode;	
	}
}

/*
 *
 */
void
redirection_destruct(redirection_t *redirection)
{
	free(redirection->input_file);
	free(redirection->output_file);
	free(redirection);
}
