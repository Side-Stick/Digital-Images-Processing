#ifndef top_index_h
#define top_index_h

//******************************************************************
//************* REMEMBER to adjust these parameters!!!**************
//******************************************************************
#define __WORDSIZE  64  //current platform is 64 bit, Intel x86-64.
//*********************** END of parameters ************************

//const(s) define, such as Error_Codes:
#include "const_bmpSystem.h"
#include "const_ErrorCodes.h"

//struct(s) or data type(s):
#include "struct_bmpFileStructure.h"

//function(s):
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
bool get_system_endian (void);  //basic_SetUp.cpp
void initial (void);    //basic_SetUp.cpp
bmpData ReadBmp (char* bmp_file_path);  //basic_bmp_io.cpp
int SaveBmp (char* save_file_path, bmpData bmp_image); //basic_bmp_io.cpp
void DeleteBmpData (bmpData bmp_image); //basic_bmp_io.cpp

//class(es):
#include "BitMapImg_BaseClass.hpp"
#include "ColorTrans_Class.hpp"
#include "GeometryTrans_Class.hpp"

#endif /* top_index_h */
