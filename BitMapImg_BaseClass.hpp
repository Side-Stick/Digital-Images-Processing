/* ***************************************************************************
 functions in this (BitMapImg_BaseClass.hpp) hpp file:
 
 (-) BitMapImg(void);
 * just designed for derived class(es).
 
 (-) BitMapImg(bmpData org_bmp_data);
 * (inline)
 * call <StandardizeBMP>
 
 (-) ~BitMapImg(void);
 * (inline)
 * delete [] bitmap_array.
 
 (-) bmpData BitMapImg::TransToBmp(void);
 * Write data of this class into a bmpData for output.
 * Always output 24-bit form or 8-bit form.
 
 (-) long getWidth(void) {return width;}
 
 (-) long getHeight(void) {return height;}
 
 (-) bool GetGrayForm(void) {return is_gray_form;}
 
 (-) unsigned char* MoveBitmapDataTo(unsigned char* target);
 
 (-) void StandardizeBMP(bmpData org_bmp_data)
 !!!!!!!!!!!!!! CAN NOT processing 16-bit BMP !!!!!!!!!!!!!!
 * Transfer all other kinds of BMP data to 24-bit (B:1byte, G:1byte, R:1byte).
 * Can processing Height < 0, and change it to Height > 0.
 * If input(org_bmp_data) is 16-bit, function will printf and exit(1).
 *****************************************************************************/

#ifndef BitMapImg_BaseClass_hpp
#define BitMapImg_BaseClass_hpp

#include <cstdlib>
#include <cstdio>

class BitMapImg {
//data:
protected:
    long width;
    long height;
    bool is_gray;
    unsigned char* bitmap_array;

//functions:
public:
    BitMapImg(void) {
        width = 0;
        height = 0;
        is_gray = false;
        bitmap_array = NULL;
    }
    BitMapImg(bmpData org_bmp_data) {
        StandardizeBMP(org_bmp_data);
    }
    ~BitMapImg(void) {
        if (NULL != bitmap_array)
            delete [] bitmap_array;
    }
    long GetWidth(void) {return width;}
    long GetHeight(void) {return height;}
    bool GetGrayForm(void) {return is_gray;}
    unsigned char* MoveBitmapDataTo(unsigned char* target) {
        target = bitmap_array;
        bitmap_array = NULL;
        return target;
    }
    bmpData TransToBmp(void);
private:
    void StandardizeBMP(bmpData org_bmp_data);
};



void BitMapImg::StandardizeBMP(bmpData org_bmp_data) {
    long line_byte = (abs(org_bmp_data.bmp_Width) * org_bmp_data.bmp_BitCount / 8 + 3) / 4 * 4;
    long x, y, bitmap_array_index, org_array_index;
    unsigned char color_index;
    
    is_gray = true;
    width = org_bmp_data.bmp_Width;
    height = org_bmp_data.bmp_Height;
    bitmap_array = new unsigned char[width * abs(height) * 3];
    
    switch (org_bmp_data.bmp_BitCount) {
        case 1:
            if (height > 0) {
                //y-axis from Bottom to Top
                for (y = 0; y < height; y++) {
                    for (x = 0; x < width; x++) {
                        color_index = (org_bmp_data.bmp_data_array[y * line_byte + (x / 8)] & (0b10000000 >> (x % 8))) >> (7 - x % 8);
                        
                        bitmap_array_index = (y * width + x) * 3;
                        
                        bitmap_array[bitmap_array_index] = org_bmp_data.bmp_color_table[color_index].rgbBlue;
                        bitmap_array[bitmap_array_index + 1] = org_bmp_data.bmp_color_table[color_index].rgbGreen;
                        bitmap_array[bitmap_array_index + 2] = org_bmp_data.bmp_color_table[color_index].rgbRed;
                        
                        if (is_gray) {
                            if ((bitmap_array[bitmap_array_index] !=     bitmap_array[bitmap_array_index + 1]) ||
                                (bitmap_array[bitmap_array_index] != bitmap_array[bitmap_array_index + 2]) ||
                                (bitmap_array[bitmap_array_index + 1] != bitmap_array[bitmap_array_index + 2]))
                                is_gray = false;
                        }
                    }
                }
            }
            else {
                //y-axis from Top to Bottom
                height = 0 - height;
                for (y = height - 1; y >= 0; y--) {
                    for (x = 0; x < width; x++) {
                        color_index = (org_bmp_data.bmp_data_array[y * line_byte + (x / 8)] & (0b10000000 >> (x % 8))) >> (7 - x % 8);

                        bitmap_array_index = ((height - 1 - y) * width + x) * 3;
                        
                        bitmap_array[bitmap_array_index] = org_bmp_data.bmp_color_table[color_index].rgbBlue;
                        bitmap_array[bitmap_array_index + 1] = org_bmp_data.bmp_color_table[color_index].rgbGreen;
                        bitmap_array[bitmap_array_index + 2] = org_bmp_data.bmp_color_table[color_index].rgbRed;
                        
                        if (is_gray) {
                            if ((bitmap_array[bitmap_array_index] !=     bitmap_array[bitmap_array_index + 1]) ||
                                (bitmap_array[bitmap_array_index] != bitmap_array[bitmap_array_index + 2]) ||
                                (bitmap_array[bitmap_array_index + 1] != bitmap_array[bitmap_array_index + 2]))
                                is_gray = false;
                        }
                    }
                }
            }
            break;
            
        case 4:
            if (height > 0) {
                //y-axis from Bottom to Top
                for (y = 0; y < height; y++) {
                    for (x = 0; x < width; x++) {
                        color_index = (org_bmp_data.bmp_data_array[y * line_byte + (x / 2)] & (0b11110000 >> ((x % 2) * 4))) >> ((1 - x % 2) * 4);
                        bitmap_array_index = (y * width + x) * 3;
                        
                        bitmap_array[bitmap_array_index] = org_bmp_data.bmp_color_table[color_index].rgbBlue;
                        bitmap_array[bitmap_array_index + 1] = org_bmp_data.bmp_color_table[color_index].rgbGreen;
                        bitmap_array[bitmap_array_index + 2] = org_bmp_data.bmp_color_table[color_index].rgbRed;
                        
                        if (is_gray) {
                            if ((bitmap_array[bitmap_array_index] !=     bitmap_array[bitmap_array_index + 1]) ||
                                (bitmap_array[bitmap_array_index] != bitmap_array[bitmap_array_index + 2]) ||
                                (bitmap_array[bitmap_array_index + 1] != bitmap_array[bitmap_array_index + 2]))
                                is_gray = false;
                        }
                    }
                }
            }
            else {
                //y-axis from Top to Bottom
                height = 0 - height;
                for (y = height - 1; y >= 0; y--) {
                    for (x = 0; x < width; x++) {
                        color_index = (org_bmp_data.bmp_data_array[y * line_byte + (x / 2)] & (0b11110000 >> ((x % 2) * 4))) >> ((1 - x % 2) * 4);
                        bitmap_array_index = ((height - 1 - y) * width + x) * 3;
                        
                        bitmap_array[bitmap_array_index] = org_bmp_data.bmp_color_table[color_index].rgbBlue;
                        bitmap_array[bitmap_array_index + 1] = org_bmp_data.bmp_color_table[color_index].rgbGreen;
                        bitmap_array[bitmap_array_index + 2] = org_bmp_data.bmp_color_table[color_index].rgbRed;
                        
                        if (is_gray) {
                            if ((bitmap_array[bitmap_array_index] !=     bitmap_array[bitmap_array_index + 1]) ||
                                (bitmap_array[bitmap_array_index] != bitmap_array[bitmap_array_index + 2]) ||
                                (bitmap_array[bitmap_array_index + 1] != bitmap_array[bitmap_array_index + 2]))
                                is_gray = false;
                        }
                    }
                }
            }
            break;
            
        case 8:
            if (height > 0) {
                //y-axis from Bottom to Top
                for (y = 0; y < height; y++) {
                    for (x = 0; x < width; x++) {
                        color_index = org_bmp_data.bmp_data_array[y * line_byte + x];
                        bitmap_array_index = (y * width + x) * 3;
                        
                        bitmap_array[bitmap_array_index] = org_bmp_data.bmp_color_table[color_index].rgbBlue;
                        bitmap_array[bitmap_array_index + 1] = org_bmp_data.bmp_color_table[color_index].rgbGreen;
                        bitmap_array[bitmap_array_index + 2] = org_bmp_data.bmp_color_table[color_index].rgbRed;
                        
                        if (is_gray) {
                            if ((bitmap_array[bitmap_array_index] !=     bitmap_array[bitmap_array_index + 1]) ||
                                (bitmap_array[bitmap_array_index] != bitmap_array[bitmap_array_index + 2]) ||
                                (bitmap_array[bitmap_array_index + 1] != bitmap_array[bitmap_array_index + 2]))
                                is_gray = false;
                        }
                    }
                }
            }
            else {
                //y-axis from Top to Bottom
                height = 0 - height;
                for (y = height - 1; y >= 0; y--) {
                    for (x = 0; x < width; x++) {
                        color_index = org_bmp_data.bmp_data_array[y * line_byte + x];
                        bitmap_array_index = ((height - 1 - y) * width + x) * 3;
                        
                        bitmap_array[bitmap_array_index] = org_bmp_data.bmp_color_table[color_index].rgbBlue;
                        bitmap_array[bitmap_array_index + 1] = org_bmp_data.bmp_color_table[color_index].rgbGreen;
                        bitmap_array[bitmap_array_index + 2] = org_bmp_data.bmp_color_table[color_index].rgbRed;
                        
                        if (is_gray) {
                            if ((bitmap_array[bitmap_array_index] !=     bitmap_array[bitmap_array_index + 1]) ||
                                (bitmap_array[bitmap_array_index] != bitmap_array[bitmap_array_index + 2]) ||
                                (bitmap_array[bitmap_array_index + 1] != bitmap_array[bitmap_array_index + 2]))
                                is_gray = false;
                        }
                    }
                }
            }
            break;
            
        case 24:
            if (height > 0) {
                //y-axis from Bottom to Top
                for (y = 0; y < height; y++) {
                    for (x = 0; x < width; x++) {
                        bitmap_array_index = (y * width + x) * 3;
                        org_array_index = y * line_byte + x * 3;
                        
                        bitmap_array[bitmap_array_index] = org_bmp_data.bmp_data_array[org_array_index];
                        bitmap_array[bitmap_array_index + 1] = org_bmp_data.bmp_data_array[org_array_index + 1];
                        bitmap_array[bitmap_array_index + 2] = org_bmp_data.bmp_data_array[org_array_index + 2];
                        
                        if (is_gray) {
                            if ((bitmap_array[bitmap_array_index] !=     bitmap_array[bitmap_array_index + 1]) ||
                                (bitmap_array[bitmap_array_index] != bitmap_array[bitmap_array_index + 2]) ||
                                (bitmap_array[bitmap_array_index + 1] != bitmap_array[bitmap_array_index + 2]))
                                is_gray = false;
                        }
                    }
                }
            }
            else {
                //y-axis from Top to Bottom
                height = 0 - height;
                for (y = height - 1; y >= 0; y--) {
                    for (x = 0; x < width; x++) {
                        bitmap_array_index = (y * width + x) * 3;
                        org_array_index = (height - 1 - y) * line_byte + x * 3;
                        
                        bitmap_array[bitmap_array_index] = org_bmp_data.bmp_data_array[org_array_index];
                        bitmap_array[bitmap_array_index + 1] = org_bmp_data.bmp_data_array[org_array_index + 1];
                        bitmap_array[bitmap_array_index + 2] = org_bmp_data.bmp_data_array[org_array_index + 2];
                        
                        if (is_gray) {
                            if ((bitmap_array[bitmap_array_index] !=     bitmap_array[bitmap_array_index + 1]) ||
                                (bitmap_array[bitmap_array_index] != bitmap_array[bitmap_array_index + 2]) ||
                                (bitmap_array[bitmap_array_index + 1] != bitmap_array[bitmap_array_index + 2]))
                                is_gray = false;
                        }
                    }
                }
            }
            break;
            
        case 32:
            if (height > 0) {
                //y-axis from Bottom to Top
                for (y = 0; y < height; y++) {
                    for (x = 0; x < width; x++) {
                        bitmap_array_index = (y * width + x) * 3;
                        org_array_index = y * line_byte + x * 4;
                        
                        bitmap_array[bitmap_array_index] = org_bmp_data.bmp_data_array[org_array_index];
                        bitmap_array[bitmap_array_index + 1] = org_bmp_data.bmp_data_array[org_array_index + 1];
                        bitmap_array[bitmap_array_index + 2] = org_bmp_data.bmp_data_array[org_array_index + 2];
                        
                        if (is_gray) {
                            if ((bitmap_array[bitmap_array_index] !=     bitmap_array[bitmap_array_index + 1]) ||
                                (bitmap_array[bitmap_array_index] != bitmap_array[bitmap_array_index + 2]) ||
                                (bitmap_array[bitmap_array_index + 1] != bitmap_array[bitmap_array_index + 2]))
                                is_gray = false;
                        }
                    }
                }
            }
            else {
                //y-axis from Top to Bottom
                height = 0 - height;
                for (y = height - 1; y >= 0; y--) {
                    for (x = 0; x < width; x++) {
                        bitmap_array_index = (y * width + x) * 3;
                        org_array_index = (height - 1 - y) * line_byte + x * 4;
                        
                        bitmap_array[bitmap_array_index] = org_bmp_data.bmp_data_array[org_array_index];
                        bitmap_array[bitmap_array_index + 1] = org_bmp_data.bmp_data_array[org_array_index + 1];
                        bitmap_array[bitmap_array_index + 2] = org_bmp_data.bmp_data_array[org_array_index + 2];
                        
                        if (is_gray) {
                            if ((bitmap_array[bitmap_array_index] !=     bitmap_array[bitmap_array_index + 1]) ||
                                (bitmap_array[bitmap_array_index] != bitmap_array[bitmap_array_index + 2]) ||
                                (bitmap_array[bitmap_array_index + 1] != bitmap_array[bitmap_array_index + 2]))
                                is_gray = false;
                        }
                    }
                }
            }
            break;
            
        default:
            printf("maybe 16-bit bmp!\n");
            exit(1);
            break;
    }
    
    if (is_gray) {
        unsigned char* gray_array = new unsigned char[width * height];
        
        for (x = 0; x < width * height; x++) {
            gray_array[x] = bitmap_array[x * 3];
        }
        delete [] bitmap_array;
        bitmap_array = gray_array;
    }
    return;
}

bmpData BitMapImg::TransToBmp(void) {
    bmpData output;
    long line_byte;
    long data_byte;
    long x, y;
    
    if (is_gray) {
        output.bmp_BitCount = 8;
        line_byte = (width * output.bmp_BitCount / 8 + 3) / 4 * 4;
        data_byte = line_byte * height;
        output.bmp_Height = height;
        output.bmp_Width = width;
        output.bmp_color_table = new RgbQuad[256]();
        output.bmp_data_array = new unsigned char[data_byte]();
        
        for (x = 0; x < 256; x++) {
            output.bmp_color_table[x].rgbBlue = x;
            output.bmp_color_table[x].rgbGreen = x;
            output.bmp_color_table[x].rgbRed = x;
        }
        
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                output.bmp_data_array[y * line_byte + x] = bitmap_array[y * width + x];
            }
        }
    }
    else {
        output.bmp_BitCount = 24;
        line_byte = (width * output.bmp_BitCount / 8 + 3) / 4 * 4;
        data_byte = line_byte * height;
        output.bmp_color_table = NULL;
        output.bmp_Height = height;
        output.bmp_Width = width;
        output.bmp_data_array = new unsigned char[data_byte]();
        
        for (y = 0; y < height; y++) {
            for (x = 0; x < width * 3; x++) {
                output.bmp_data_array[y * line_byte + x] = bitmap_array[y * width * 3 + x];
            }
        }
    }
    return output;
}

#endif /* BitMapImg_BaseClass_hpp */
