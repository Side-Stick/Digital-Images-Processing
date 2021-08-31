#ifndef struct_bmpFileStructure_h
#define struct_bmpFileStructure_h

#if 64 == __WORDSIZE
    typedef unsigned short u_word2;
    typedef short word2;
    typedef unsigned int u_word4;
    typedef int word4;
#elif 32 == __WORDSIZE
    typedef unsigned short u_word2;
    typedef short word2;
    typedef unsigned int u_word4;
    typedef int word4;
#elif 16 == __WORDSIZE
    typedef unsigned short u_word2;
    typedef short word2;
    typedef unsigned long u_word4;
    typedef long word4;
#endif

typedef struct struct_BitMapFileHeader {
    //u_word2 bfType;  //must be "BM" (0x424D)
    u_word4 bfSize;   //Bytes(s)
    u_word2 bfReserved1; //usually 0
    u_word2 bfReserved2;
    u_word4 bfOffBits;    //position of real data, (Bytes)
} BitMapFileHeader;

typedef struct struct_BitMapInfoHeader {
    u_word4 biSize;   //40 Bytes
    word4 biWidth;
    word4 biHeight;  //+: Bottom -> Top; -: Top -> Bottom
    u_word2 biPlanes;    //must be 1
    u_word2 biBitCount;  //1, 4, 8, 16, 24 or 32
    u_word4 biCompression;    //seen in "const_bmpSystem.h"
    u_word4 biSizeImage;
    word4 biXPelsPerMeter;
    word4 biYPelsPerMeter;
    u_word4 biClrUsed;
    u_word4 biClrImportant;
} BitMapInfoHeader;

typedef struct struct_RGBQuad {
    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    unsigned char rgbReserved;
} RgbQuad;

typedef struct struct_bmpData {
    long bmp_Width;
    long bmp_Height;
    unsigned short bmp_BitCount;
    RgbQuad* bmp_color_table;
    unsigned char* bmp_data_array;
} bmpData;

#endif /* struct_bmpFileStructure_h */
