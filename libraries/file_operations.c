// © Copyright 2023, D0MlNIC, All Rights Reserved.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "OpenSource/ini/ini.h"
#include "Miscallenous.h"
#include "mixed.h"

#define BUFFER_SIZE 1000
#define TEMP_SIZE 512

int word_exists_in_file(FILE *fp, char *word, bool *exists);
char *ini_get_whole_section(ini_t *ini_data, char *section);
int print_error_message();
void copy_ini_file(char *source, char *dest);

//char app_folder[256];

int word_exists_in_file(FILE *fp, char *word, bool *exists) {
    char line[BUFFER_SIZE];
    *exists = false;
    while (((fgets(line, BUFFER_SIZE, fp)) != NULL) && (*exists == false)) {
        if (strstr(line, word) != NULL) {
            *exists = true;
        }
    }
}

int log_info_message(int num, bool to_print) {
    char temp[TEMP_SIZE];

    strncpy(temp, app_folder, TEMP_SIZE);
    FILE *log_file = fopen(strcat(temp, "log_file.txt"), "a");
    strncpy(temp, app_folder, TEMP_SIZE); 
    ini_t *info_file = ini_load(strcat(temp, "errors.ini"));
    char *info_num = malloc(10);
    int_to_str(num, info_num);
    const char *info_mes = ini_get(info_file, NULL, info_num);
    fprintf(log_file, "Info Num     = %d\nInfo Message = %s\n", num, info_mes);
    if (to_print) {
        printf("Info Num     = %d\nInfo Message = %s\n", num, info_mes);
    }
    ini_free(info_file);
    free(info_num);
}

/**
 * [ini_data]   is the ini data from the ini_tostring function.
 * [section]    is the name of the section you want all keys and values from.
 * Returns all of the section key and values
 * NOTE: Use a copy of ini data if you want to use it elsewhere as other data is lost here.
 * NOTE: Don't forget to free the ptr after using the data.
 */
char *ini_get_whole_section(ini_t *ini_file, char *section) {
    char *section_ptr;
    char *temp_ptr;
    char *whole_section;
    char *data_cpy;
    char *ini_data;

    ini_data = ini_tostring(ini_file);
    data_cpy = malloc(strlen(ini_data) + 1);
    strcpy(data_cpy, ini_data);

    section_ptr = strstr(data_cpy, section);
    whole_section = strtok_r(section_ptr, "[]", &section_ptr); // This is the section name itself
    whole_section = strtok_r(section_ptr, "[]", &section_ptr); // We want the keys and the values
    whole_section = strip(whole_section);                      // Strip /n from the data
    free(data_cpy);
    return whole_section;                                      // Remember to free the ptr
}

void copy_ini_file(char *source, char *dest) {
    ini_t *src = ini_load(source);
    ini_save(src, dest);
    ini_free(src);
}