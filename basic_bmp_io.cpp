/* ***************************************************************************
 functions in this (basic_bmp_io.cpp) cpp file:
 
 (1) void DeleteBmpData (bmpData bmp_image);
 * free memory of a bmpData (if not NULL).
 
 (2) bmpData ReadBmp (char* bmp_file_path);
 * may throw: WRONG_FILE_PATH, NOT_BMP_FILE, FILE_DAMAGED.
 * Read BMP file, get it's file-header and info-header,
 * but read in color-table and data directly (without any processing).
 * If in big-endian, this function will call <read_by_byte>.
 
 (3) bool read_by_byte (void* target, unsigned long size_byte, FILE* source);
 * may throw: FILE_DAMAGED.
 * Read in data byte by byte, MAX: 4bytes.
 * Designed for different endian mode.
 
 (4) int SaveBmp (char* save_file_path, bmpData bmp_image);
 * may throw: NO_DATA, WRONG_FILE_PATH, WRITE_IN_ERROR.
 * Save data in bmp_image to a new BMP file, and then <DeleteBmpData>.
 * Logically similar with <ReadBmp>.
 * If in big-endian, this function will call <write_by_byte>.
 
 (5) bool write_by_byte (void* content, unsigned long size_byte, FILE* output);
 * may throw: WRITE_IN_ERROR.
 * MAX: 4bytes.
 * Logically similar with <read_by_byte>.
 *****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "const_bmpSystem.h"
#include "const_ErrorCodes.h"
#include "struct_bmpFileStructure.h"

//#define debug_bmp_io
//#define debug_bmp_io_endian
#define running_with_clang
/* about running_with_clang :
 while complied on Xcode(macOS) with clang, "FILE* bmp_file = NULL;" has a strange bug:
    if that phrase is inside function <read_bmp>,
    "fread( , , , source)" in function <read_by_byte> will EXC_BAD_ACCESS.
 but on Linux or Windows, the bug won't appear. */

extern bool is_little_endian;
#ifdef running_with_clang
static FILE* bmp_file = NULL;
#endif

bool read_by_byte (void* target, unsigned long size_byte, FILE* source);    //basic_bmp_io.cpp
bool write_by_byte (void* content, unsigned long size_byte, FILE* output);  //basic_bmp_io.cpp

void DeleteBmpData (bmpData bmp_image) {
    if (NULL != bmp_image.bmp_color_table)
        delete [] bmp_image.bmp_color_table;
    
    if (NULL != bmp_image.bmp_data_array)
        delete [] bmp_image.bmp_data_array;
}

bmpData ReadBmp (char* bmp_file_path) {
    bmpData bmp_image;
#ifndef running_with_clang
    FILE* bmp_file = NULL;
#endif
    unsigned short file_type_check = 0;
    unsigned long line_byte = 0;
    unsigned long data_byte = 0;
    unsigned long color_table_byte = 0;
    unsigned long succeeded_length = 0;
    BitMapFileHeader bmp_file_header = {0};
    BitMapInfoHeader bmp_info_header = {0};
    RgbQuad *bmp_quad = NULL;
    
    bmp_file = fopen(bmp_file_path, "rb");
    if (NULL == bmp_file)
        throw WRONG_FILE_PATH;
    
#ifdef debug_bmp_io_endian
    if (0) {
#else
    if (is_little_endian) {
#endif
        //just a faster process, only on little_endian machine.
        
        if (1 != fread(&file_type_check, sizeof(unsigned short), 1, bmp_file))
            throw NOT_BMP_FILE;
        if (0x4D42 != file_type_check)   //little_endian
            throw NOT_BMP_FILE;
        
        succeeded_length = fread(&bmp_file_header, sizeof(BitMapFileHeader), 1, bmp_file);
        if (1 != succeeded_length)
            throw FILE_DAMAGED;
        succeeded_length = fread(&bmp_info_header, sizeof(BitMapInfoHeader), 1, bmp_file);
        if (1 != succeeded_length)
            throw FILE_DAMAGED;
    }
    else {
        //no matter little or big endian.
        //But if running on a little-endian machine, the "if" bench can process faster than "else" bench.
        
        if (!read_by_byte(&file_type_check, 2, bmp_file))
            throw NOT_BMP_FILE;
        if (0x4D42 != file_type_check)
            throw NOT_BMP_FILE;
        
        try {
            read_by_byte(&bmp_file_header.bfSize, 4, bmp_file);
            read_by_byte(&bmp_file_header.bfReserved1, 2, bmp_file);
            read_by_byte(&bmp_file_header.bfReserved2, 2, bmp_file);
            read_by_byte(&bmp_file_header.bfOffBits, 4, bmp_file);
            
            read_by_byte(&bmp_info_header.biSize, 4, bmp_file);
            read_by_byte(&bmp_info_header.biWidth, 4, bmp_file);
            read_by_byte(&bmp_info_header.biHeight, 4, bmp_file);
            read_by_byte(&bmp_info_header.biPlanes, 2, bmp_file);
            read_by_byte(&bmp_info_header.biBitCount, 2, bmp_file);
            read_by_byte(&bmp_info_header.biCompression, 4, bmp_file);
            read_by_byte(&bmp_info_header.biSizeImage, 4, bmp_file);
            read_by_byte(&bmp_info_header.biXPelsPerMeter, 4, bmp_file);
            read_by_byte(&bmp_info_header.biYPelsPerMeter, 4, bmp_file);
            read_by_byte(&bmp_info_header.biClrUsed, 4, bmp_file);
            read_by_byte(&bmp_info_header.biClrImportant, 4, bmp_file);
        } catch (...) {
            throw FILE_DAMAGED;
        }
    }
    
#ifdef debug_bmp_io
    printf("BitMapFileHeader:\n");
    printf("file type: %x\n", file_type_check);
    printf("size of the file: %u\n", bmp_file_header.bfSize);
    printf("reserved words: %u %u\n", bmp_file_header.bfReserved1, bmp_file_header.bfReserved2);
    printf("OffBits: %u\n\n", bmp_file_header.bfOffBits);
    
    printf("BitMapInfoHeader:\n");
    printf("size of the struct: %u\n", bmp_info_header.biSize);
    printf("width of the bmp: %d\n", bmp_info_header.biWidth);
    printf("height of the bmp: %d\n", bmp_info_header.biHeight);
    printf("biPlanes: %u\n", bmp_info_header.biPlanes);
    printf("BitCount: %u\n", bmp_info_header.biBitCount);
    printf("Compression: %u\n", bmp_info_header.biCompression);
    printf("SizeImage: %u\n", bmp_info_header.biSizeImage);
    printf("X: %d dpm; Y: %d dpm\n", bmp_info_header.biXPelsPerMeter, bmp_info_header.biYPelsPerMeter);
    printf("Color Used: %u\n", bmp_info_header.biClrUsed);
    printf("Color Important: %u\n\n", bmp_info_header.biClrImportant);
#endif
    
    bmp_image.bmp_Width = bmp_info_header.biWidth;
    bmp_image.bmp_Height = bmp_info_header.biHeight;
    bmp_image.bmp_BitCount = bmp_info_header.biBitCount;
    
    if (bmp_image.bmp_BitCount <= 8) {
        color_table_byte = (unsigned long)pow(2, bmp_image.bmp_BitCount);
        bmp_quad = new RgbQuad[color_table_byte];
        succeeded_length = fread(bmp_quad, sizeof(RgbQuad), color_table_byte, bmp_file);
        if (succeeded_length != color_table_byte)
            throw FILE_DAMAGED;
    }
    else {
        //while BitCount == 24 or 32, there's no color table.
        bmp_quad = NULL;
    }
    bmp_image.bmp_color_table = bmp_quad;
    
    line_byte = (abs(bmp_image.bmp_Width) * bmp_image.bmp_BitCount / 8 + 3) / 4 * 4;
    data_byte = line_byte * abs(bmp_image.bmp_Height);
    bmp_image.bmp_data_array = new unsigned char[data_byte];
    succeeded_length = fread(bmp_image.bmp_data_array, sizeof(unsigned char), data_byte, bmp_file);
    if (succeeded_length != data_byte)
        throw FILE_DAMAGED;
    
    fclose(bmp_file);
    return bmp_image;
}

bool read_by_byte (void* target, unsigned long size_byte, FILE* source) {
    unsigned long succeeded_length = 0;
    unsigned char read_buffer[4] = {0};
    
    succeeded_length = fread(read_buffer, sizeof(unsigned char), size_byte, source);
    if (succeeded_length != size_byte)
        throw FILE_DAMAGED;
    
    *(word4*)target = read_buffer[size_byte - 1];
    for (int i = (int)size_byte - 2; i >= 0; i--) {
        *(word4*)target = (*(word4*)target << 8) + (word4)read_buffer[i];
    }
    
    return true;
}

int SaveBmp (char* save_file_path, bmpData bmp_image) {
    unsigned long line_byte = 0;
    unsigned long data_byte = 0;
    unsigned long color_table_byte = 0;
    unsigned short file_type = 0;
    unsigned long succeeded_length = 0;
#ifndef running_with_clang
    FILE* bmp_file = NULL;
#endif
    BitMapFileHeader bmp_file_header = {0};
    BitMapInfoHeader bmp_info_header = {0};
    
    if (NULL == bmp_image.bmp_data_array)
        throw NO_DATA;
    
    //edit the file_header and info_header:
    file_type = 0x4D42;
    line_byte = (abs(bmp_image.bmp_Width) * bmp_image.bmp_BitCount / 8 + 3) / 4 * 4;
    data_byte = line_byte * abs(bmp_image.bmp_Height);
    if (bmp_image.bmp_BitCount <= 8) {
        color_table_byte = (unsigned long)pow(2, bmp_image.bmp_BitCount) * 4;
    }
    else {
        color_table_byte = 0;
    }
    bmp_file_header.bfReserved1 = 0;
    bmp_file_header.bfReserved2 = 0;
    bmp_file_header.bfSize = 54 + (u_word4)color_table_byte + (u_word4)data_byte;
    bmp_file_header.bfOffBits = 54 + (u_word4)color_table_byte;
    
    bmp_info_header.biSize = 40;
    bmp_info_header.biWidth = (word4)bmp_image.bmp_Width;
    bmp_info_header.biHeight = (word4)bmp_image.bmp_Height;
    bmp_info_header.biPlanes = 1;
    bmp_info_header.biBitCount = bmp_image.bmp_BitCount;
    bmp_info_header.biCompression = BI_RGB;
    bmp_info_header.biSizeImage = (u_word4)data_byte;
    bmp_info_header.biXPelsPerMeter = 0;
    bmp_info_header.biYPelsPerMeter = 0;
    bmp_info_header.biClrUsed = 0;
    bmp_info_header.biClrImportant = 0;
    
    //write the data(s) to target file:
    bmp_file = fopen(save_file_path, "wb");
    if (NULL == bmp_file)
        throw WRONG_FILE_PATH;
    
#ifdef debug_bmp_io_endian
    if (0) {
#else
    if (is_little_endian) {
#endif
        succeeded_length = fwrite(&file_type, sizeof(unsigned short), 1, bmp_file);
        if (1 != succeeded_length)
            throw WRITE_IN_ERROR;
        
        succeeded_length = fwrite(&bmp_file_header, sizeof(BitMapFileHeader), 1, bmp_file);
        if (1 != succeeded_length)
            throw WRITE_IN_ERROR;
        succeeded_length = fwrite(&bmp_info_header, sizeof(BitMapInfoHeader), 1, bmp_file);
        if (1 != succeeded_length)
            throw WRITE_IN_ERROR;
    }
    else {
        try {
            write_by_byte(&file_type, 2, bmp_file);
            write_by_byte(&bmp_file_header.bfSize, 4, bmp_file);
            write_by_byte(&bmp_file_header.bfReserved1, 2, bmp_file);
            write_by_byte(&bmp_file_header.bfReserved2, 2, bmp_file);
            write_by_byte(&bmp_file_header.bfOffBits, 4, bmp_file);
            
            write_by_byte(&bmp_info_header.biSize, 4, bmp_file);
            write_by_byte(&bmp_info_header.biWidth, 4, bmp_file);
            write_by_byte(&bmp_info_header.biHeight, 4, bmp_file);
            write_by_byte(&bmp_info_header.biPlanes, 2, bmp_file);
            write_by_byte(&bmp_info_header.biBitCount, 2, bmp_file);
            write_by_byte(&bmp_info_header.biCompression, 4, bmp_file);
            write_by_byte(&bmp_info_header.biSizeImage, 4, bmp_file);
            write_by_byte(&bmp_info_header.biXPelsPerMeter, 4, bmp_file);
            write_by_byte(&bmp_info_header.biYPelsPerMeter, 4, bmp_file);
            write_by_byte(&bmp_info_header.biClrUsed, 4, bmp_file);
            write_by_byte(&bmp_info_header.biClrImportant, 4, bmp_file);
        }
        catch (...) {
            throw WRITE_IN_ERROR;
        }
    }
    
    if (0 != color_table_byte) {
        succeeded_length = fwrite(bmp_image.bmp_color_table, sizeof(RgbQuad), color_table_byte / 4, bmp_file);
        if (succeeded_length != color_table_byte / 4)
            throw WRITE_IN_ERROR;
    }
        
    succeeded_length = fwrite(bmp_image.bmp_data_array, sizeof(unsigned char), data_byte, bmp_file);
    if (succeeded_length != data_byte)
        throw WRITE_IN_ERROR;
    
    fclose(bmp_file);
    DeleteBmpData(bmp_image);
    return 0;
}

bool write_by_byte (void* content, unsigned long size_byte, FILE* output) {
    unsigned long succeeded_length = 0;
    unsigned char write_buffer[4] = {0};
    
    for (int i = 0; i < size_byte; i++) {
        write_buffer[i] = *(word4*)content >> (8 * i);
    }
    
    succeeded_length = fwrite(write_buffer, sizeof(unsigned char), size_byte, output);
    
    if (succeeded_length == size_byte)
        return true;
    else
        throw WRITE_IN_ERROR;
}
