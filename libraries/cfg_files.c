//A /lib file
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "file_operations.h"
#include "Miscallenous.h"
#include "mixed.h"
#include "OpenSource/ini/ini.h"

#define BUFFER_SIZE 1000
#define LOW_BOUND_BUFFER_SIZE 100
#define TEMP_SIZE 512

/*Public Functions*/
int check_cfg_files(char *path);
int setup_cfg_files(int result_code);

/*Private Functions*/
int input_details_cfg_files();
void details_cfg_files();
void initialize();

// Global Definitions of variables
char name_of_config_file[11];
char word[32];
//char app_folder[256];

// Struct Definitions
struct userDetails {
    char email[LOW_BOUND_BUFFER_SIZE];
    char password[LOW_BOUND_BUFFER_SIZE];
} userDetail;


void initialize_words() {
    // Copying Contents to the string arrays
    strcpy(name_of_config_file, "config.ini");
    strcpy(word, "email");
    
}

int input_details_cfg_files(ini_t *fp) {
    printf("Please enter the email address.\nemail = ");
    fgets(userDetail.email, LOW_BOUND_BUFFER_SIZE, stdin);
    printf("Please enter the password you want to use, make sure you enter it correctly."); //Hide the pw, verify inputs, changes later.
    fgets(userDetail.password, LOW_BOUND_BUFFER_SIZE, stdin);
    char *sEmail = strip(userDetail.email);
    char *sPassword = strip(userDetail.password);
    ini_set(fp, "App Details", "email", sEmail);
    ini_set(fp, "App Details", "password", sPassword);
    
    // Gonna add more stuff as time passes most prob
}

int check_cfg_files(char *path) {
    // Defining Variables
    int code;
    bool exists;
    char temp[TEMP_SIZE];
    FILE *fp;

    initialize_words();
    strncpy(temp, app_folder, TEMP_SIZE);
    fp = fopen(strcat(temp, name_of_config_file), "r");
    if (fp != NULL) {
        // Checking if the word exists or not such as email
        word_exists_in_file(fp, word, &exists);
        code = exists == false ? 102 : 1;
        if (code == 102) {
            log_info_message(102, true);
        }
    } else {
        log_info_message(101, true);
        code = 101;
    }
    fclose(fp);
    return code;
}

int setup_cfg_files(int result_code) {
    char *paths_ini_data; // Not Used Yet
    char temp[TEMP_SIZE];
    int code;
    //FILE *fp;
    ini_t *cfg_file;
    ini_t *paths_file;

    strncpy(temp, app_folder, TEMP_SIZE);
    cfg_file = ini_load(strcat(temp, name_of_config_file));

    //initialize(); // Don't know why I used it, gotta comment it out
    //fp = fopen(strcat(path, name_of_config_file), "a");
    
    // Initialize the config.ini file
    if (result_code == 101) {
        if (cfg_file == NULL) {
            log_info_message(100, true);
            code = 100;
        }
        else {
            //fputs("[App Details]\napp_name = Privacy_Enforced\npath = /usr/share/Privacy_Enforced/\n", fp);
            ini_set(cfg_file, "App Details", "app_name", "Privacy_Enforced");
            ini_set(cfg_file, "App Details", "path", app_folder);
            input_details_cfg_files(cfg_file);
            code = 1;
        }
    } else if (result_code == 102) {
        input_details_cfg_files(cfg_file);
        code = 1;
    }
    strncpy(temp, app_folder, 512);
    ini_save(cfg_file, strcat(temp, name_of_config_file));
    ini_free(cfg_file);

    // Initialize the added_devices.ini file
    paths_file = ini_create(paths_ini_data, 0);
    ini_set(paths_file, "Added_Devices_Paths", "root_path", app_folder);
    ini_set(paths_file, "Num_Devices", "root_path", "Testing Purposes : 0");
    strncpy(temp, app_folder, 512);
    ini_save(paths_file, strcat(temp, "added_devices.ini"));
    ini_free(paths_file);

    return code;
}