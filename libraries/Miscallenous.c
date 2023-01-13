// © Copyright 2023, D0MlNIC, All Rights Reserved.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int int_to_str(int num, char *dest);
char *lstrip(char *str);
char *rstrip(char *str);
char *strip(char *str);


int int_to_str(int num, char *dest) {
    // https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
    int length = snprintf(NULL, 0, "%d", num);
    char* str = malloc(length + 1);
    snprintf(str, length + 1, "%d", num);
    strcpy(dest, str);
    free(str);
    return 0;
}

/**
 * Accepts a string of type char*
 * Returns a new string of type char*
 * NOTE: Don't forget to free the new string
*/
char *lstrip(char *str) {
    int n;
    char *new_str;
    n = strspn(str, "\n\r");
    new_str = calloc(strlen(str)-n+1, 1);
    strcpy(new_str, str+n);
    return new_str;

    // This is another way of doing it without using strspn, but still using string library. HEHE!!
    //size_t o;
    /*
    for (n=0; n<strlen(str); n++) {
        o = strcspn(str+n, "\n\r");
        if (o!=0) {
            break;
        }
    }
    */
}

char *rstrip(char *str) {
    int n;
    size_t o, len;
    char *new_str;

    len = strlen(str);
    for (n=0; n<len; n++) {
        o = strcspn(str+len-1-n, "\n\r");
        if (o!=0) {
            break;
        }
    }
    
    new_str = calloc(len-n+1, 1);
    strncpy(new_str, str, len-n);
    return new_str;
}

char *strip(char *str) {
    char *temp_str;
    char *new_str;

    temp_str = lstrip(str);
    new_str = rstrip(temp_str);
    free(temp_str);
    return new_str;
}