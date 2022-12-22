#include <stdio.h>
#include <string.h>
#include "mixed.h"
#include "cfg_files.h"
#include "server_sock.h"

char app_folder[256];

// For concatenation, Use:
// snprintf(dest, sizeof(dest), format_specifier, relevant variables)

int main() {
    int result_code;
    int server_sock_fd;
    strcpy(app_folder, "/usr/share/Privacy_Enforced/");
    /*Check if Config Files exist or not*/
    /*Set Every required fill below here*/
    result_code = check_cfg_files(); // Pass the path of the config folder
    if (result_code != 1) {
        result_code = setup_cfg_files(result_code);
        /*if (result_code == 100) {
            printf("Please run the program with root access. Thanks! You may use sudo Privacy_Enforced");
            return 100;
        }*/ // Might not need this part.
    }
    /*Add DB for storing info later on the stage.
    Need to add a lot of things.*/
    /*End of setting the required files*/
    
    /*Socket Implementation here.*/
    // This will be a server socket.
    // It will be accepting commands from the client i.e. android (For Now)
    // These commands will be comapared with the pre-set commands in a file.
    // Relevant function will be called based on the info.
    // Output will be returened back through the socket.
    // For now we will have socket implementation.
    // Later on I may add web functionality (http).
    server_sock_fd = main_server_socket(); //Change Var name
    if (server_sock_fd != 1) {
        return server_sock_fd;
    }
    return 0;
}