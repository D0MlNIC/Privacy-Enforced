// © Copyright 2023, D0MlNIC, All Rights Reserved.

#ifndef COMMANDS_HM
#define COMMANDS_HM

void main_command_parser(char **full_command, int num_words, struct info_mes_det *command_success);

/***
 * Command Types
 * quit                    = quit // Will quit the connection, implement it after implementing get_device_func
 * add_device_func         = add-device company device-type model-number/model-name device_num
 * add_device_info_func    = add-device-info company device-type model-number/model-name device_num key value
 * login_func              = login username/email password
 * get_device_details_func = get-device company device-type model-number/model-name device-num   [type] type:0,1,2 
 *                                                                                               0 = Info (Default)
 *                                                                                               1 = Functions
 *                                                                                               2 = Both 0 and 1 
 * logout_func             = logout
 * remove_device_func      = remove-device company device-type model-number/model-name           [type] type:0,1
 *                                                                                               0 = Temporary
 *                                                                                               1 = Permanent
*/

#endif