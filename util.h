#ifndef __UTIL_H__
#define __UTIL_H__
#include "cmd_node.h"
#define INS_MODE_BEF 0
#define INS_MODE_AFT 1
#define INS_MODE_BOTH 2

void str_split(char* str, const char* tok, char*** res, int* count);
int is_match(const char* str, char* regex);
void str_replace_one_world(char** str, char match, char to_replace);
int count_char_num(char* str, char c);

// TODO: fix it
// void insert_char_to_match(char** str, char match, char to_insert, int mode);

int str_ends_with(char* str, char c);
int str_starts_with(char* str, char c);
void dbg_print_cmd_list(cmd_node_t* list);
#endif
