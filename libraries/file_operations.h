// © Copyright 2023, D0MlNIC, All Rights Reserved.

#ifndef FILE_OPERATIONS_HM
#define FILE_OPERATIONS_HM

#include <stdio.h>
#include <stdbool.h>
#include "OpenSource/ini/ini.h"

/*Parse the ini file from Config Folder*/
int word_exists_in_file(FILE *fp, char *word, bool *exists);
char *ini_get_whole_section(ini_t *ini_data, char *section);
int log_info_message(int num, bool to_print);
void copy_ini_file(char *source, char *dest);

#endif