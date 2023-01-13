// © Copyright 2023, D0MlNIC, All Rights Reserved.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "file_operations.h"
#include "mixed.h"
#include "Miscallenous.h"
#include "server_sock.h"
#include "OpenSource/ini/ini.h"

#define INITIAL_HEADER_SIZE 10
#define INITIAL_DATA_SIZE 50

//const char main_delim = '~';
//const char ini_delim  = '=';
// \n works as a delimiter b/w keys and values in ini files
// delims[0] = main_delim AND delims[1] = ini_delim

unsigned char delims[2] = {'~', '='}; 
uint8_t MAGIC_BYTES[4] = {'\x5d', '\x51', '\x5b', '\x47'};
char *section_names[2] = {"Device_Info", "Device_Functions"};

struct Data_Storage_Info {
    size_t pos;
    size_t size;
    size_t pieces;
};

//void data_init(char *data_loc, char *data);
void *data_init(uint8_t type, char *file_path, unsigned int section_num, struct info_mes_det *info, struct Data_Storage_Info *data_info);
void *header_init(uint8_t type, char *file_path, unsigned int section_num, struct info_mes_det *info, int *reply_size); 
//void get_device_functions();
//void get_device_info();

void *header_init(uint8_t type, char *file_path, unsigned int section_num, struct info_mes_det *info, int *reply_size) {
    
    // Not using Message_Header struct, because in the end i will be copying everything to the void ptr array 

    int *data_size;
    void *header;
    char *temp_data;
    void *reply;
    struct Data_Storage_Info header_info;
    struct Data_Storage_Info data_info;

    header = calloc(INITIAL_HEADER_SIZE, 1);
    header_info.pos = 0;
    header_info.size = INITIAL_HEADER_SIZE;
    temp_data = data_init(type, file_path, section_num, info, &data_info);

    add_strings(MAGIC_BYTES, 4, &header, &header_info, false);  // Setting Magic Bytes
    add_char(type, header, &header_info);               // Setting The type
    add_char(delims[0], header, &header_info);          // Setting the delim
    memcpy(((char *)header) + 6, &data_info.size, 2);   // Setting the size of the data
    memcpy(((char *)header) + 8, &data_info.pieces, 2); // Setting the number of pieces

    reply = calloc(INITIAL_HEADER_SIZE + data_info.size, 1);
    memcpy(reply, header, INITIAL_HEADER_SIZE);
    memcpy(((char*)reply)+INITIAL_HEADER_SIZE, temp_data, data_info.size);
    *reply_size = data_info.size + 10;

    free(temp_data);
    free(header);
    
    return reply;
}

void *data_init(uint8_t type, char *file_path, unsigned int section_num, struct info_mes_det *info, struct Data_Storage_Info *data_info) {
    // Get Data, how? In an array maybe?

    char *struct_name[] = {"code", "success"};
    char *ini_data;
    char *section;
    unsigned int struct_data[] = {info->code, info->success};
    void *data_buffer;
    ini_t *fp;

    data_buffer = calloc(INITIAL_DATA_SIZE, 1);
    data_info->pos = 0;
    data_info->size = INITIAL_DATA_SIZE;
    
    memcpy(data_buffer, &delims[0], sizeof(char));
    //data_info->pos = data_info->pos + sizeof(uint8_t);
    memcpy(((unsigned char *)data_buffer) + 1, &info->success, sizeof(uint8_t));
    memcpy(((unsigned char *)data_buffer) + 2, &delims[0], sizeof(char));
    memcpy(((unsigned char *)data_buffer) + 3, &info->code, sizeof(uint16_t));
    memcpy(((unsigned char *)data_buffer) + 5, &delims[0], sizeof(char));

    data_info->pos = 6;
    data_info->pieces = 2;
    
    // Check if file_path is NULL or not.
    switch ((unsigned char)type) {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3: break;
        case 4: break;
        case 255: break;
        default:
            if (file_path != NULL) {
                // Add a section in the data returned.
                switch (section_num) {
                    case 0: // Need to fix all this mess later on, for now it's temp here!!
                    case 1:
                        fp = ini_load(file_path);
                        //ini_data = ini_tostring(fp);
                        section = ini_get_whole_section(fp, section_names[section_num]);
                        add_strings(section, strlen(section), &data_buffer, data_info, true);
                        data_info->pieces = 3;
                        ini_free(fp);
                        free(ini_data);
                        free(section);
                        break;
                    case 99:
                        // Temporary workaround for now!!!!
                        fp = ini_load(file_path);
                        //ini_data = ini_tostring(fp);
                        section = ini_get_whole_section(fp, section_names[0]);
                        add_strings(section, strlen(section), &data_buffer, data_info, true);
                        data_info->pieces = 3;
                        ini_free(fp);
                        free(ini_data);
                        free(section);

                        fp = ini_load(file_path);
                        ///ini_data = ini_tostring(fp);
                        section = ini_get_whole_section(fp, section_names[1]);
                        add_strings(section, strlen(section), &data_buffer, data_info, true);
                        data_info->pieces = 3;
                        ini_free(fp);
                        free(ini_data);
                        free(section);
                }
            }
    }

    data_buffer = realloc(data_buffer, data_info->pos+10); // Truncating the array to required size.
    data_info->size = data_info->pos;
    return data_buffer;
}

void increase_space(void **buf, struct Data_Storage_Info *buf_info, size_t size) {
    void *temp_buf;
    
    if ((buf_info->pos + size + 1) > buf_info->size) {
        buf_info->size = buf_info->size + 1000;
        temp_buf = realloc(*buf, buf_info->size);
        *buf = temp_buf;
        //return temp_buf;
    }
    //return NULL;
}

/*void truncate(void *buf, struct Data_Storage_Info *buf_info) {
    realloc(buf, buf_info->size);
}*/

/**
 * [num] is for the delim in the delim array
 * [buf] is the data array
 * [buf_info] is the info of the data array
 */
void add_char(char c, void *buf, struct Data_Storage_Info *buf_info) {
    memcpy(((unsigned char *)buf) + buf_info->pos, &c, sizeof(char));
    buf_info->pos += 1;
}

void add_int(int num, void **buf, struct Data_Storage_Info *buf_info) {
    void *temp_buf;

    increase_space(buf, buf_info, sizeof(int));
    /*if (temp_buf != NULL) {
        *buf = temp_buf;
    }*/
    memcpy(((unsigned char *)*buf) + buf_info->pos, num ,sizeof(int));
    buf_info->pos += sizeof(int);
    add_char(delims[0], *buf, buf_info);
}

void add_strings(char *str, size_t str_size, void **buf, struct Data_Storage_Info *buf_info, bool add_delim) {
    increase_space(buf, buf_info, str_size);
    memcpy(((unsigned char *)*buf) + buf_info->pos, str, str_size);
    buf_info->pos += str_size;
    if (add_delim) {
        add_char(delims[0], *buf, buf_info);
    }
}
