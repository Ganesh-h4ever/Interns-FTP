#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
 using namespace std;

//connection establishment related errors
 const string ERR_SOCKET_CREATION = "Error: Unable to create Socket";
 const string ERR_BIND_FAILED = "Error: Bind failed";
 const string ERR_LISTEN_FAILED = "Error: Listen failed";
 const string ERR_ACCEPT_FAILED = "Error: Accept failed";
 const string ERR_CONNECTION_FAILED = "Error: Connection failed";

//command related errors
 const string ERR_FILE_NOT_FOUND ="Error: File not found";
 const string ERR_FILE_CREATE_FAILED ="Error: File creation failed";
 const string ERR_FILE_WRITE_FAILED ="Error: File write failed";
 const string ERR_FILE_READ_FAILED = "Error: File read failed";
 const string ERR_DIR_NOT_FOUND="Error: Directory not found";
 const string ERR_FILE_REMOVE_FAILED = "Error: File removal failed";

 const string ERR_INVALID_OPERATION="Error: Invalid Operation";
 const string ERR_INVALID_PROTOCOL= "Error: Invalid Protocol";
 const string ERR_INVALID_COMMAND = "Error: Invalid Command";

 const string PERMISSION_DENIED = "Error: Permission Denied";
const string ERR_INVALID_ARGS = "Error: Invalid arguments provided.";
#endif
