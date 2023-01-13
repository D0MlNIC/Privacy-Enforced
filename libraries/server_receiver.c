// © Copyright 2023, D0MlNIC, All Rights Reserved.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "file_operations.h"
#include "server_sock.h"

#define MAX_COMMAND_SIZE 64

int main_server_receiver(char *command, int command_size, struct info_mes_det *command_success);
void delimiter_finder(char delim, char *mes, int* pos_holder, int* pos_size, struct info_mes_det *command_success);
void delimiter_command_seperator(char delim, char *mes, char **words);

int main_server_receiver(char *command, int command_size, struct info_mes_det *command_success) {
    // Delete int command_size in parameter
    int n, code, num_words;
    int* pos_holder = (int*) malloc(10*sizeof(int)); // To hold the indexes of where the seperator is
    int* pos_size = malloc(sizeof(int)); // To know how many seperators there are
    //struct command_info command_details;

    delimiter_finder(":", command, pos_holder, pos_size, command_success);
    switch (command_success->code) {
    case 204: 
        { /* Make them resend, bruh, is this gonna happen?????? AND FREE MEMORY FROM MALLOC*/ }
    case 205:
        { /* Make them resend, same as above, but different. Use FALLTHROUGH HERE*/ }
    case 206: {/*USE FALLTHROUGH*/}
        command_success->resend = 1;
        break;
    default:
        num_words = pos_size[0] - 1;
        char **sub_commands = malloc(num_words*sizeof(char*));
        command_success->resend = 0;

        delimiter_command_seperator(':', command, sub_commands); // Maybe pass command_success here.
        main_command_parser(sub_commands, num_words, command_success); // Set var
        subcommand_free(sub_commands, num_words);
    }
    free(pos_holder);
    free(pos_size);
    // Finish off this area after implementing Header of the message
}

void delimiter_finder(char delim, char *mes, int* pos_holder, int* pos_size, struct info_mes_det *command_success) {
    int pos, count, i;
    count = 0;
    i = 0;
    pos = 0;
    while (mes[i] != '\0') {
        if (mes[i] == delim) {
            pos = i;
            pos_holder[count] = pos;
            count++;          
        }
        i++;
    }
    pos_size[0] = count;
    if (count == 0) {
        // No command sent;
        log_info_message(204, false);
        command_success->code=204;
        command_success->success=0;
        //return 204;
    }
    else if (mes[0] != delim) {
        // Command not sent starting with delim;
        log_info_message(205, false);
        command_success->code=205;
        command_success->success=0;
        //return 205;
    }
    else if ((strrchr(mes, delim)-mes)!=(strlen(mes)-1)) {
        // Command not sent ending with delim; MY ENGLISH HAS GONE FOR WALK!! 
        log_info_message(206, false);
        command_success->code=206;
        command_success->success=0;
        //return 206;
    }
    else {
        log_info_message(1, false);
        command_success->code=1;
        command_success->success=1;
        //return 0;
    }

    if (command_success->success == 0) {
        command_success->resend=1;
    } else {command_success->resend=0;}
}


void delimiter_command_seperator(char *delim, char *mes, char **words) {
    int n;
    char *word;
    //char *mes_cpy;

    //mes_cpy = mes;
    n = 0;

    n = 0;
    //strcpy(mes_cpy, mes);
    word = strtok(mes, delim);
    while( word != NULL ) {
        words[n] = malloc(strlen(word));
        strcpy(words[n], word);
        n++;
        word = strtok(NULL, delim);
    }

    /*while((word = strtok_r(mes_cpy, delim, &mes_cpy))) {
        words[n] = malloc(strlen(word));
        strcpy(words[n], word);
        n++;
    }*/ // Don't know what's wrong with this one.
    //return 0;
}

void subcommand_free(char **words, int num_words) {
    for (int n=0; n<num_words; n++) {
        free(words[n]);
    }
    free(words);
}
