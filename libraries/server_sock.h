// © Copyright 2023, D0MlNIC, All Rights Reserved.

#ifndef SERVER_SOCK_HM
#define SERVER_SOCK_HM

#include <stdint.h>

#define MAX_FILE_PATH 128
struct info_mes_det {
    uint16_t code;
    uint8_t success;
    uint8_t type;
    uint8_t section_num;
    uint8_t resend;
    char *file_path;
};

int main_server_socket();

#endif