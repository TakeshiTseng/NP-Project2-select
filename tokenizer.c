#include <string.h>
#include "tokenizer.h"
#include <unistd.h>
#include "util.h"
#include "token.h"
#include <stdlib.h>
#include <regex.h>

token_node_t* _tok_list = NULL;
int _read_line(char* buff);
void _read_and_parse();
int _get_type(char* str);
void _handle_yell(char _p_buffer[]);
void _handle_tell(char _p_buffer[]);

void init_source_fd(int _sfd) {
    _source_fd = _sfd;
    // _p_buffer = malloc(sizeof(char) * 10010);
}

int _read_line(char* res) {
    int n_byte = 0;
    int r = 1;
    char buf;
    while(r == 1 && buf != '\n') {
        r = read(0, &buf, 1);
        res[n_byte] = buf;
        n_byte += r;
    }
    return n_byte;
}

void _read_and_parse() {
    char _p_buffer[30000];
    bzero(_p_buffer, 30000); // clean up before read.

    int _n_bytes = _read_line(_p_buffer);
    last_line = malloc(_n_bytes + 1);
    bzero(last_line, _n_bytes + 1);
    strcpy(last_line, _p_buffer);
    last_line[_n_bytes - 2] = '\0';

    if(_n_bytes <= 0) { // end of stream or error
        token_node_t* new_node = malloc(sizeof(token_node_t));
        new_node->token_str = "";
        new_node->type = FEOF;
        insert_node(&_tok_list, new_node);
        return;
    }

    // need to add a space before new line "\n"
    str_replace_one_world(_p_buffer, '\r', ' ');

    // XXX: add special token: yell, tell
    // because name will not contains space,
    // let name as normal command

    if(is_match(_p_buffer, "yell .*")) {
        _handle_yell(_p_buffer);
        return;
    } else if(is_match(_p_buffer, "tell .*")) {
        _handle_tell(_p_buffer);
        return;
    }

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
    } else if(is_match(str, ">[[:digit:]]+")) {
        return PIPE_TO_USR;
    } else if(is_match(str, "\n")) {
        return NEW_LINE;
    } else if(is_match(str, ">")) {
        return PASS;
    } else if(is_match(str, "<[[:digit:]]+")) {
        return PIPE_FROM_USR;
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

void _handle_yell(char _p_buffer[]) {
    token_node_t* yell_node = malloc(sizeof(token_node_t));
    yell_node->type = CMD;
    yell_node->token_str = "yell";
    yell_node->next_node = NULL;

    insert_node(&_tok_list, yell_node);

    // split message
    char message[10000];
    bzero(message, 10000);
    strcpy(message, (_p_buffer + 5));
    // remove last space and new line
    int _i = strlen(message) - 1;
    while(message[_i] == '\n' || message[_i] == ' ') {
        message[_i--] = '\0';
    }

    token_node_t* msg_node = malloc(sizeof(token_node_t));
    msg_node->type = CMD;
    msg_node->token_str = message;
    msg_node->next_node = NULL;

    insert_node(&_tok_list, msg_node);
    token_node_t* new_line_tok = malloc(sizeof(token_node_t));
    new_line_tok->type = NEW_LINE;
    new_line_tok->token_str = "\n";
    new_line_tok->next_node = NULL;

    insert_node(&_tok_list, new_line_tok);

}

void _handle_tell(char _p_buffer[]) {
    // format
    // tell [name] [message]

    //first, remove "tell "(include space)
    char tmp[10000];
    strcpy(tmp, (_p_buffer + 5));

    // create tell node
    token_node_t* tell_node = malloc(sizeof(token_node_t));
    tell_node->type = CMD;
    tell_node->token_str = "tell";
    tell_node->next_node = NULL;

    insert_node(&_tok_list, tell_node);

    // second, get name
    // (oops: it not user name, is client id, :P)
    // XXX: using ugly methon to get name
    char name[21]; // name is less then 20 characters
    bzero(name, 21);
    int _i; // temp index
    for(_i=0; _i<20; _i++) {
        if(tmp[_i] == ' ') {
            break;
        } else {
            name[_i] = tmp[_i];
        }
    }

    // generate name node
    token_node_t* name_node = malloc(sizeof(token_node_t));
    name_node->type = CMD;
    name_node->token_str = name;
    name_node->next_node = NULL;

    insert_node(&_tok_list, name_node);


    // finally, remove name and get message
    char message[10000];
    bzero(message, 10000);
    strcpy(message, (tmp + _i + 1));

    // remove last space and new line
    _i = strlen(message) - 1;
    while(message[_i] == '\n' || message[_i] == ' ') {
        message[_i--] = '\0';
    }

    //generate message node

    token_node_t* msg_node = malloc(sizeof(token_node_t));
    msg_node->type = CMD;
    msg_node->token_str = message;
    msg_node->next_node = NULL;

    insert_node(&_tok_list, msg_node);

    token_node_t* new_line_tok = malloc(sizeof(token_node_t));
    new_line_tok->type = NEW_LINE;
    new_line_tok->token_str = "\n";
    new_line_tok->next_node = NULL;

    insert_node(&_tok_list, new_line_tok);
}
