// © Copyright 2023, D0MlNIC, All Rights Reserved.

#ifndef CFG_FILES_HM
#define CFG_FILES_HM

/*Check if the config files exist under the /usr directory*/
int check_cfg_files();

/*Setup the config files during first stratup or if the user reset the app*/
int setup_cfg_files(int result_code);

#endif