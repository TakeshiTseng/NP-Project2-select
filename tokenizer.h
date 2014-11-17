#ifndef __PARSER_H__
#define __PARSER_H__
#include "token.h"

char* _p_buffer;
int _source_fd;

int next_token(char** token_string);
void init_source_fd(int _sfd);
token_node_t* get_token_list();

#endif
