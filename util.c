#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "./cmd_node.h"

void str_split(char* str, const char* tok, char*** res, int* count) {

    // To avoid Segmentation fault we need to convert to char array....
    // Maybe I need to write a new strtok function?
    char buff[10001];
    strncpy(buff, str, 10001);

    // some var for split
    int _count = 0;
    char* _res[5000];
    char* tmp_str;

    tmp_str = strtok(buff, tok);

    while(tmp_str != NULL) {
        int _len = strlen(tmp_str) + 1;
        _res[_count] = malloc(sizeof(char)*_len);
        strcpy(_res[_count], tmp_str);
        _count++;
        tmp_str = strtok(NULL, tok);
    }

    *count = _count;
    *res = _res;

}

int is_match(const char* str, char* regex_str) {
    regex_t regex;
    int res;
    const int nmatch = 1;
    regmatch_t pmatch[nmatch];

    // compile regex
    if(regcomp(&regex, regex_str, REG_EXTENDED) != 0) {
        // if there contains an error
        regfree(&regex);
        return -1;
    }

    // check match
    res = regexec(&regex, str, nmatch, pmatch, 0);

    // free regex data
    regfree(&regex);

    // return result
    if(res == REG_NOMATCH){
        return 0;// no match
    } else {
        return 1;// match
    }

}

void str_replace_one_world(char** str, char match, char to_replace) {
    int c=0;
    while((*str)[c] != '\0') {
        if((*str)[c] == match) {
            (*str)[c] = to_replace;
        }
        c++;
    }
}

int count_char_num(char* str, char c) {
    int count = 0;
    while(*str != '\0') {
        if(*str == c) {
            count++;
        }
        str++;
    }
    return count;
}

int str_ends_with(char* str, char c) {
    int len = strlen(str);
    return str[len-1] == c;
}

int str_starts_with(char* str, char c) {
    return str[0] == c;
}

void dbg_print_cmd_list(cmd_node_t* list) {
    cmd_node_t* tmp = list;
    while(tmp != NULL) {
        printf("%s\t", tmp->cmd);
        char** args = tmp->args;
        int c;
        for(c=0; args[c] != NULL; c++) {
            printf("%s\t", args[c]);
        }
        if(tmp->pipe_to_file) {
            printf("%s\t", tmp->filename);
        }
        printf("%d\n", tmp->pipe_count);
        tmp = tmp->next_node;
    }

}


/*
void insert_char_to_match(char** str, char match, char to_insert, int mode) {
    size_t num_of_match = (size_t)count_char_num(str, match);
    size_t new_size;
    if(mode == INS_MODE_BEF) {
        new_size = strlen(str) + num_of_match + 1;
    } else if(mode == INS_MODE_AFT) {
        new_size = strlen(str) + num_of_match + 1;
    } else {
        new_size = strlen(str) + num_of_match * 2 + 1;
    }

    // TODO: fix it!
    char* new_tmp_str = malloc(sizeof(char));
    int pos = 0;
    int n_pos = 0;
    while(str[pos] != '\0') {
        if(str[pos] == match) {
            if(mode == INS_MODE_BEF) {
                new_tmp_str[n_pos++] = to_insert;
                new_tmp_str[n_pos++] =  str[pos];
            } else if(mode == INS_MODE_AFT) {
                new_tmp_str[n_pos++] =  str[pos];
                new_tmp_str[n_pos++] = to_insert;
            } else {
                new_tmp_str[n_pos++] = to_insert;
                new_tmp_str[n_pos++] =  str[pos];
                new_tmp_str[n_pos++] = to_insert;
            }
        } else {
            new_tmp_str[n_pos++] =  str[pos];
        }
        pos++;
    }
    new_tmp_str[n_pos] = '\0';
    return new_tmp_str;
}
*/
