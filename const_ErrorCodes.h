#ifndef const_ErrorCodes_h
#define const_ErrorCodes_h
//ErrorCodes: 4 Bytes
//first 2 Bytes represent area, last 2 Bytes represent specific error.

//errors in bmp_io (0x0001----)
#define WRONG_FILE_PATH 0x00010001
#define NOT_BMP_FILE    0x00010002
#define NO_DATA         0x00010003
#define FILE_DAMAGED    0x00010004
#define WRITE_IN_ERROR  0x00010005

#endif /* const_ErrorCodes_h */
