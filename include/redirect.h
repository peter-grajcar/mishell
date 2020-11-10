#ifndef REDIRECT_H
#define REDIRECT_H

enum redirect_output_mode {
	REDIRECT_OUTPUT_NONE,
	REDIRECT_OUTPUT_WRITE,
	REDIRECT_OUTPUT_APPEND
};

struct redirection {
	char *input_file;
	char *output_file;
	enum redirect_output_mode output_mode;
};

typedef enum redirect_output_mode redirect_output_mode_t;
typedef struct redirection redirection_t;

redirection_t *redirection_construct(char *file, redirect_output_mode_t mode);
void redirection_add(redirection_t *redirection, char *file, redirect_output_mode_t mode);
void redirection_destruct(redirection_t *redirection);

#endif
