#ifndef MESSAGE_HEADER_HM
#define MESSAGE_HEADER_HM

#include <stdint.h>
/**
 * This is a custom header file for the messages sent to the client from this server, not vice versa.
 * Total bytes are 12.
 * MAGIC BYTES are 4     ---> x5d515b47 \x5d\x51\x5b\x47
 * TYPE is 1 byte        ---> More details below, used to identify the answer for specific queries
 * DELIMITER is 1 byte   ---> No Specific reason for 2 bytes, just to conform with the padding stuff
 * NUM_BYTES is 2 bytes  ---> To identify the number of total bytes of the message, basically it's size
 * ARRAY_SIZE is 2 bytes ---> To identify the number of arrays in the answers
 * 
 * First Delimiter is '~', although I may change it later to some random byte.
 * Second Delimiter is '`'.
 */

/**
 * So, Talking about Types
 * 0x00 = Quit Type           // Might not use it
 * 0x01 = Login Type
 * 0x02 = Logout Type
 * 0x03 = Add-Device Type
 * 0x04 = Add-Device-Info Type
 * 0x05 = Get-Device Type
 * 0x06 = Remove-Device Type
 * 0xff = Resend             // Resend the message for it was not in correct format
 * 
 * Will be adding more as time will pass
 */

/**
 * Functions related to Header
 * 1- So make up the data in the specified format.
 *      Like (2 delims or not?) and with that make the header.
 * Pass in to the init function where it will request data. and return struct as a header
 * Make data function as well, which will request a malloc pointer to store the final data in.
 */

/**
 * Login_Data_Header! Do I need it?
 * I will just return success with some bunch of data, what data?
 * For now it's just success, and we are logged in, why not pass a session_token type of thing to verify with?
 * 
 * Logout_Data_Header! Just delete session token.
 * 
 * Add-Device_Header! Just return success stuff
 * 
 * Remove-Device_Header! Return deleted successfully. bruh
 * 
 * Get_Devices_Header! Return all of the devices, this will need a header, why not just seperate data with delim, and pass in another delim.
 *  
 */

struct Mes_Header {
    const uint8_t bytes[4];
    uint8_t type;
    uint8_t delimiter;
    uint16_t num_bytes;
    uint16_t num_arrays;
};

enum Mes_Types {
    quit,
    login,
    logout,
    addDevice,
    getDevice,
    removeDevice
};

//extern struct info_mes_det;

//void *data_init(char *file_path, unsigned int section_num, struct info_mes_det *info);
void *header_init(uint8_t type, char *file_path, unsigned int section_num, struct info_mes_det *info, int *reply_size);

/**
 * This is the Array Header Definitions.
 * Used to store a single piece of data upto n pieces as required.
 * 
 * TYPE is 1 byte
 * DATA_SIZE is 2 bytes
 */

struct Data_Header {
    uint16_t type;
    uint16_t data_size;
};

#endif