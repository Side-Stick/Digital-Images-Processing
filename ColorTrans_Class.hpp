/* ***************************************************************************
 functions in this (ColorTrans_Class.hpp) hpp file:

 (1) GrayTrans(bmpData org_bmp_img) : BitMapImg(org_bmp_img);
 
 (2) GrayTrans(BitMapImg &org);
 * copy from a BitMapImg object.
 
 (3) void ColorToGray(void);
 * Trans color(24-bit) to gray(8-bit) with formula:
 * I = 0.3 * Blue + 0.59 * Green + 0.11 * Red
 
 (4) void Binary(int threshold = 128);
 * Only processing gray images.
 * If is_gray_form == false, will call <ColorToGray> first.
 
 (5) void Reverse(void);
 
 (6) void LogarithmStretch(double a, double b, double c);
 * g(x, y) = a + ln[f(x, y) + 1] / (b * lnc)
 * Reference: a = 0, b = 0.033, c = 2
 * Usually lighter.
 
 (7) void ExponentStretch(double a, double b, double c);
 * g(x, y) = b ^ {c * [f(x, y) - a]} - 1.
 * Reference: a = 128, b = 2, c = 0.6
 * Usually darker.
 
 *****************************************************************************/

#ifndef ColorTrans_Class_hpp
#define ColorTrans_Class_hpp

#include <cmath>

class ColorTrans : public BitMapImg {
//functions:
public:
    ColorTrans(bmpData org_bmp_img) : BitMapImg(org_bmp_img) {
        return;
    }
    ColorTrans(BitMapImg &org) {
        width = org.GetWidth();
        height = org.GetHeight();
        is_gray = org.GetGrayForm();
        bitmap_array = org.MoveBitmapDataTo(bitmap_array);
        delete &org;
        
        return;
    }
    void ColorToGray(void);
    void Binary(int threshold);
    void Reverse(void);
    void LogarithmStretch(double a, double b, double c);
    void ExponentStretch(double a, double b, double c);
};



void ColorTrans::ColorToGray (void) {
    if (is_gray)
        return;
    
    unsigned char* gray_bitmap_array = new unsigned char[height * width];
    for (long i = 0; i < height * width; i++) {
        gray_bitmap_array[i] = 0.3 * bitmap_array[i * 3] + 0.59 * bitmap_array[i * 3 + 1] + 0.11 * bitmap_array[i * 3 + 2];
    }
    
    delete [] bitmap_array;
    bitmap_array = gray_bitmap_array;
    is_gray = true;
}

void ColorTrans::Binary(int threshold = 128) {
    if (!is_gray)
        ColorToGray();
    
    for (long i = 0; i < height * width; i++) {
        if (bitmap_array[i] < threshold)
            bitmap_array[i] = 0;
        else
            bitmap_array[i] = 255;
    }
}

void ColorTrans::Reverse(void) {
    long array_length = 0;
    if (is_gray)
        array_length = height * width;
    else
        array_length = height * width * 3;
    
    for (long i = 0; i < array_length; i++) {
        bitmap_array[i] = 255 - bitmap_array[i];
    }
}

void ColorTrans::LogarithmStretch(double a = 0, double b = 0.033, double c = 2) {
    long array_length = 0;
    if (is_gray)
        array_length = height * width;
    else
        array_length = height * width * 3;
    
    double result;
    
    for (long i = 0; i < array_length; i++) {
        result = a + (log(bitmap_array[i] + 1)) / (b * log(c));
        if (result > 255)
            result = 255;
        else if (result < 0)
            result = 0;
        
        bitmap_array[i] = (int)result;
    }
}

void ColorTrans::ExponentStretch(double a = 128, double b = 2, double c = 0.6) {
    long array_length = 0;
    if (is_gray)
        array_length = height * width;
    else
        array_length = height * width * 3;
    
    double result;
    
    for (long i = 0; i < array_length; i++) {
        result = pow(b, (c * (bitmap_array[i] - a))) - 1;
        if (result > 255)
            result = 255;
        else if (result < 0)
            result = 0;
        
        bitmap_array[i] = (int)result;
    }
}

#endif /* ColorTrans_Class_hpp */
