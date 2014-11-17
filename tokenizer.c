#include <string.h>
#include "tokenizer.h"
#include <unistd.h>
#include "util.h"
#include "token.h"
#include <stdlib.h>
#include <regex.h>

token_node_t* _tok_list = NULL;
void _read_and_parse();
int _get_type(char* str);

void init_source_fd(int _sfd) {
    _source_fd = _sfd;
    _p_buffer = malloc(sizeof(char) * 10010);
}

void _read_and_parse() {
    char _tp_buffer[10010];
    bzero(_tp_buffer, 10010);
    bzero(_p_buffer, 10010); // clean up before read.

    // FIXME !!
    int _n_bytes = 0;
    // int _tn_bytes = read(_source_fd, _tp_buffer, 10010);
    // XXX: change to stdin
    int _tn_bytes = read(0, _tp_buffer, 10010);
    while(_tn_bytes != 0) {
        memcpy(&_p_buffer[_n_bytes], _tp_buffer, _tn_bytes);
        _n_bytes += _tn_bytes;
        if(_tp_buffer[_tn_bytes - 1] == '\n') {
            break;
        }
        _tn_bytes = read(_source_fd, _tp_buffer, 10010);
    }

    if(_n_bytes <= 0) { // end of stream or error
        // TODO: need to handle error
        token_node_t* new_node = malloc(sizeof(token_node_t));
        new_node->token_str = "";
        new_node->type = FEOF;
        insert_node(&_tok_list, new_node);
        return;
    }

    // need to add a space before new line "\n"
    str_replace_one_world(&_p_buffer, '\r', ' ');

    // TODO: fix it
    // _p_buffer = insert_char_to_match(_p_buffer, '\n', ' ', INS_MODE_BOTH);

    char** _result;
    int _num_of_token;
    str_split(_p_buffer, " ", &_result, &_num_of_token);
    int _c;
    for(_c=0; _c<_num_of_token; _c++) {
        token_node_t* new_node = malloc(sizeof(token_node_t));
        char* tmp_str = _result[_c];
        int type;
        int new_line_pos = -1;
        if(str_starts_with(tmp_str, '\n') && strcmp(tmp_str, "\n") != 0) {
            new_line_pos = 1;

            token_node_t* new_line_tok = malloc(sizeof(token_node_t));
            new_line_tok->type = NEW_LINE;
            new_line_tok->token_str = "\n";
            new_line_tok->next_node = NULL;
            insert_node(&_tok_list, new_line_tok);
            tmp_str = (tmp_str+1);
        }
        if(str_ends_with(tmp_str, '\n') && strcmp(tmp_str, "\n") != 0) {
            tmp_str[strlen(tmp_str)-1] = '\0'; // remove new line
            new_line_pos = 2;
        }

        type = _get_type(tmp_str);
        // set token string to node
        new_node->token_str = malloc(sizeof(char) * (strlen(tmp_str) + 1));
        strcpy(new_node->token_str, tmp_str);
        new_node->type = type;
        new_node->next_node = NULL;
        insert_node(&_tok_list, new_node);

        if(new_line_pos == 2) {
            token_node_t* new_line_tok = malloc(sizeof(token_node_t));
            new_line_tok->type = NEW_LINE;
            new_line_tok->token_str = "\n";
            new_line_tok->next_node = NULL;
            insert_node(&_tok_list, new_line_tok);
        }
    }
}

int _get_type(char* str) {
    if(is_match(str, "\\|[[:digit:]]+")) {
        return PIPE_N;
    } else if(is_match(str, "\\|")) {
        return PIPE;
    } else if(is_match(str, ">")) {
        return PASS;
    } else if(is_match(str, "\n")) {
        return NEW_LINE;
    } else {
        return CMD;
    }
}


int next_token(char** token_string) {

    // if all tokens are processed
    // get next line
    if(_tok_list == NULL) {
        _read_and_parse();
    }

    token_node_t* _token_node = pull_node(&_tok_list);

    // get token string
    char* tmp_str = _token_node->token_str;
    *token_string = malloc(sizeof(char) * (strlen(tmp_str) + 1));
    strcpy(*token_string, tmp_str);

    // get token type
    int _type = _token_node->type;

    _token_node->next_node = NULL;
    free(_token_node);

    return _type;
}

token_node_t* get_token_list() {
    if(_tok_list == NULL) {
        _read_and_parse();
    }
    return _tok_list;
}
