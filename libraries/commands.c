#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "commands.h"
#include "OpenSource/ini/ini.h"
#include "file_operations.h"
#include "Message_Header.h"
#include "mixed.h"
#include "server_sock.h"

#define NUM_COMMANDS 7
#define PATH_SIZE 512
#define TEMP_SIZE 512

#define MAX_NAME_SIZE 20
#define MAX_FUNC_NAME_SIZE 30
#define MAX_MAIN_COMMAND_SIZE 100

//char app_folder[256];

char *commands[NUM_COMMANDS] = { // When adding new commands, make sure to modify NUM_COMMANDS.
    "quit",               // 0
    "login",              // 1
    "logout",             // 2
    "add-device",         // 3
    "add-device-info",    // 4
    "get-device",         // 5
    "remove-device"       // 6
    
};

/*void *fun_ptr_arr[1] = {
    &quit_func
};

typedef struct commands_functions {
    char *name;
};*/

struct command_info {
    unsigned int num;
    char command_name[MAX_MAIN_COMMAND_SIZE];
};
struct command_info all_commands[NUM_COMMANDS]; //Interlink using the enum in Message_Header.h


void main_command_parser(char **full_command, int num_words, struct info_mes_det *command_success);
void login_func(char **full_com, int num_words, struct info_mes_det *command_success);
int add_device_func(char **full_com, int num_words, struct info_mes_det *command_success);
void add_device_info_func(char **full_com, int num_words, struct info_mes_det *command_success);
void get_device_details_func(char **full_com, int num_words, struct info_mes_det *command_success);

void main_command_parser(char **full_command, int num_words, struct info_mes_det *command_success) {
    
    int command_num, code;
    command_info_maker();
    
    for (int n=0;n<NUM_COMMANDS;n++) {
        if ((strcmp(all_commands[n].command_name, full_command[0])) == 0) {
            command_num = all_commands[n].num;
            break;
        }
    }

    switch (all_commands[command_num].num) { // I have defined which number stands for what function above in the commands array
        case 0:
            quit_func(); // Under Implementation
            break;
        case 1:
            login_func(full_command, num_words, command_success);
            break;
        case 2:
            //logout_func(); // Under Implementation
            break;
        case 3:
            add_device_func(full_command, num_words, command_success);
            break;
        case 4:
            add_device_info_func(full_command, num_words, command_success);
            break;
        case 5:
            get_device_details_func(full_command, num_words, command_success);
            break;
        case 6:
            remove_device_func(full_command, num_words); // Under Implementation
            break;
    }
    //return code;
}

int quit_func() {
    // What will it do.
    // Why am I implementing it, just to turn off the program remotely, bruh why!!!!!! Leave it
}

int add_device_func(char **full_com, int num_words, struct info_mes_det *command_success) {

    /*  Command will be sent with what?
        After adding device, what next?
        Ask for details, make sep function i guess, makes more sense for adding data piece by piece.
    */
    // MAKE A SEPERATE ADD_SPECIFIC_DEVICE_DATA Function type of function.  !!ENGLISH HAS GONE FOR WINTER HOLIDAYS

    command_success->type = 3;
    command_success->section_num = 1;
    command_success->file_path = NULL;

    char temp[TEMP_SIZE];
    char key[PATH_SIZE];
    char value[PATH_SIZE];
    char original_file[PATH_SIZE];
    char copy_file[PATH_SIZE];
    ini_t *paths_file;

    char *comp = full_com[1];
    char *device_type = full_com[2];
    char *model_num = full_com[3];
    char *device_num = full_com[4];
    
    strncpy(temp, app_folder, TEMP_SIZE);
    paths_file = ini_load(strcat(temp, "added_devices.ini"));

    // Apply Basic File/Folder checks here, You know, easy to <HACK>
    // And some custom ones as well!
    snprintf(key, PATH_SIZE, "%s_%s_%s_%s", comp, device_type, model_num, device_num); // Concatenating to make a unique key
    snprintf(value, PATH_SIZE, "%sCompanies/%s/%s/%s/%s_%s.ini", app_folder, comp, device_type, model_num, model_num, device_num); // Same as previous, but unique value
    snprintf(original_file, PATH_SIZE, "%sCompanies/%s/%s/%s/%s_Original.ini", app_folder, comp, device_type, model_num, model_num);
    snprintf(copy_file, PATH_SIZE, "%sCompanies/%s/%s/%s/%s_%s.ini", app_folder, comp, device_type, model_num, model_num, device_num);
    
    ini_set(paths_file, "Added_Devices_Path", key, value);
    ini_pset(paths_file, "Num_Devices", key, "%s", device_num);
    strncpy(temp, app_folder, TEMP_SIZE);
    ini_save(paths_file, strcat(temp, "added_devices.ini"));
    copy_ini_file(original_file, copy_file);
    ini_free(paths_file);

    command_success->code = 1006;
    command_success->success = 1;
    return 1;
    // Apply basic checks and logging info, cuz we need to log stuff and set command_success respectively. 
}

void add_device_info_func(char **full_com, int num_words, struct info_mes_det *command_success) {

    command_success->type = 4;
    command_success->section_num = 1;
    command_success->file_path = NULL;

    // Variables Definition
    char temp[TEMP_SIZE];
    char *tempPtr;
    char filePathKey[PATH_SIZE];
    char *device_file_path;
    ini_t *added_devices_file;
    ini_t *device_file;
    
    // Sub_Commands
    char *comp        = full_com[1];
    char *device_type = full_com[2];
    char *model_num   = full_com[3];
    char *device_num  = full_com[4];
    char *key         = full_com[5];
    char *value       = full_com[6];


    snprintf(filePathKey, PATH_SIZE, "%s_%s_%s_%s", comp, device_type, model_num, device_num);
    strncpy(temp, app_folder, TEMP_SIZE);
    added_devices_file = ini_load(strcat(temp, "added_devices.ini"));
    device_file_path = ini_get(added_devices_file, "Added_Devices_Path", key);
    device_file = ini_load(device_file_path);
    ini_set(device_file, section_names[0], key, value);
    ini_save(device_file, device_file_path);
    ini_free(added_devices_file);
    ini_free(device_file);

    command_success->code = 1;
    command_success->success = 1; // Need to change later
    // Apply basic checks. (3 funcs you need to do same thing.);
    // Add logging info as well, all 3 funcs.

}

void get_device_details_func(char **full_com, int num_words, struct info_mes_det *command_success) {
    // Implement tomorrow
    command_success->type = 5;
    command_success->section_num = 0;

    // Variables Definition
    long details_type;
    char *tempPtr;
    char temp[TEMP_SIZE];
    char key[PATH_SIZE];
    //char *device_file_path_temp;
    char *device_file_path;
    ini_t *added_devices_file;
    ini_t *device_file; // May use it later during refactoring

    // Sub_Commands
    char *comp        = full_com[1];
    char *device_type = full_com[2];
    char *model_num   = full_com[3];
    char *device_num  = full_com[4];
    char *details_type_temp;

    if (num_words == 6) {
        details_type = strtol(full_com[5], &tempPtr, 10);    
    }

    //details_type = strtol(details_type_temp, &tempPtr, 10);
    snprintf(key, PATH_SIZE, "%s_%s_%s_%s", comp, device_type, model_num, device_num);
    strncpy(temp, app_folder, TEMP_SIZE);
    added_devices_file = ini_load(strcat(temp, "added_devices.ini"));
    device_file_path = ini_get(added_devices_file, "Added_Devices_Path", key);
    //strcpy(device_file_path, device_file_path_temp);

    command_success->file_path = device_file_path;
    
    switch (details_type) {
        case 1:
            command_success->section_num = 1;
            break;
        case 2:
            command_success->section_num = 99; // To return both arrays
            break;
        default:
            command_success->section_num = 0;
            break;
    }

    ini_free(added_devices_file);
    command_success->code = 1;
    command_success->success = 1; //need to change later as well
    // Again, need to apply basic checks and measures, don't forget.
}

int remove_device_func(char **full_com, int num_words) {
    // Remove Devices here, will complete later. Meh Not interesting to implement.
}

void login_func(char **full_com, int num_words, struct info_mes_det *command_success) {
    command_success->type = 1;
    command_success->section_num = 1;
    command_success->file_path = NULL;
    // Oh dear, How am I gonna implement login func, I srsly haven't planned for this. bruh!
    // So, I get username and pw, I compare it with the one stroed in cfg files. 
    // If matched, then I will send back an allow messaage for now. (Gotta focus on security)
    // If not, then Drop the connection.

    unsigned int op_code;
    char *info_mes;
    char temp[TEMP_SIZE];

    strncpy(temp, app_folder, TEMP_SIZE);
    ini_t *cfg_file = ini_load(strcat(temp, "config.ini"));
    char *email = ini_get(cfg_file, "User Details", "email");
    char *password = ini_get(cfg_file, "User Details", "password");

    if (strcmp(full_com[1], email) && strcmp(full_com[2], password)) {
        op_code = 2; // For Success
    } else {op_code = 3;}
    ini_free(cfg_file);

    switch (op_code) {
    case 2:
        log_info_message(op_code, false);
        command_success->code = 1002;
        command_success->success = 1;
    case 3:
        log_info_message(op_code, false);
        command_success->code = 1003;
        command_success->success = 0;
    }
}

int single_command_handler(char *main_word) {
    /*for (int n=0; n<NUM_COMMANDS; n++) {
        if (strcmp(all_commands[n].command_name, main_word)) {

        }
    }*/
}

void command_info_maker() {
    for (int n=0; n<NUM_COMMANDS; n++) {
        all_commands[n].num = n;
        strcpy(all_commands[n].command_name, commands[n]);
    }
}