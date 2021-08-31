/* ***************************************************************************
 functions in this (ColorTrans_Class.hpp) hpp file:

 (-) GeometryTrans(bmpData org_bmp_img) : BitMapImg(org_bmp_img);
 
 (-) GeometryTrans(BitMapImg &org);
 * copy from a BitMapImg object.
 
 (-) void Zoom(long out_width, long out_height, int select_algorithm = 1);
    1-> void Zoom_Neighbor(long out_width, long out_height);
    2-> void Zoom_DoubleLinear(long out_width, long out_height);
    3-> void Zoom_Convolution(long out_width, long out_height);
 * Zoom the image to a given size.
 * 1 is the fastest, 3 is the clearest.
 
 (-) unsigned char Interpolation_DoubleLinear_core(unsigned char around[2][2], double x_pos, double y_pos);
 * Chinese name (utf-8): 双线性插值法
 * For single pixel.
 * Average of the surrounding 4 pixels.
 
 (-) unsigned char Interpolation_Convolution_core(unsigned char around[4][4], double x_pos, double y_pos);
 * Chinese name (utf-8): 立方卷积插值法（矩阵乘法）
 * (For single pixel.)
 * g(x, y) = f(u, v) = ABC
 * A = [s(1+b), s(b), s(1-b), s(2-b)];
 * B = around[4][4];
 * C = [s(1+a), s(a), s(1-a), s(2-a)]^T;
 * a = u - [u]; b = v - [v];
 *        {1 - 2|w|^2 + |w|^3            ,|w| < 1;
 * s(w) = {4 - 8|w| + 5|w|^2 - |w|^3     ,1 <= |w| < 2;
 *        {0                             ,|w| >= 2.

*****************************************************************************/

#ifndef GeometryTrans_Class_hpp
#define GeometryTrans_Class_hpp

#include <cmath>

class GeometryTrans : public BitMapImg {
//functions:
public:
    GeometryTrans(bmpData org_bmp_img) : BitMapImg(org_bmp_img) {
        return;
    }
    GeometryTrans(BitMapImg &org) {
        width = org.GetWidth();
        height = org.GetHeight();
        is_gray = org.GetGrayForm();
        bitmap_array = org.MoveBitmapDataTo(bitmap_array);
        delete &org;
        
        return;
    }
    void Zoom(long out_width, long out_height, int select_algorithm = 1) {
        if (1 == select_algorithm)
            Zoom_Neighbor(out_width, out_height);
        else if (2 == select_algorithm)
            Zoom_DoubleLinear(out_width, out_height);
        else if (3 == select_algorithm)
            Zoom_Convolution(out_width, out_height);
    }
    
private:
    void Zoom_Neighbor(long out_width, long out_height);
    void Zoom_DoubleLinear(long out_width, long out_height);
    void Zoom_Convolution(long out_width, long out_height);
    unsigned char Interpolation_DoubleLinear_core(unsigned char around[2][2], double x_pos, double y_pos);
    unsigned char Interpolation_Convolution_core(unsigned char around[4][4], double x_pos, double y_pos);
};



void GeometryTrans::Zoom_Neighbor(long out_width, long out_height) {
    double ratio_x = (double)out_width / width;
    double ratio_y = (double)out_height / height;
    long org_x, org_y;
    int pixel_byte = is_gray ? 1 : 3;
    long x, y;
    unsigned char* result = new unsigned char[out_width * out_width * pixel_byte];
    
    for (y = 0; y < out_height; y++) {
        for (x = 0; x < out_width; x++) {
            //calculate (x, y)'s position in original bitmap (org_x, org_y)
            org_x = (double)x / ratio_x + 0.5;
            org_y = (double)y / ratio_y + 0.5;
            
            if (0 <= org_x && org_x < width && 0 <= org_y && org_y < height) {
                for (int i = 0; i < pixel_byte; i++) {
                    result[(y * out_width + x) * pixel_byte + i] = bitmap_array[(org_y * width + org_x) * pixel_byte + i];
                }
            }
            else {
                for (int i = 0; i < pixel_byte; i++)
                    result[(y * out_width + x) * pixel_byte + i] = 255;
            }
        }
    }
    
    delete [] bitmap_array;
    bitmap_array = result;
    width = out_width;
    height = out_height;
}

void GeometryTrans::Zoom_DoubleLinear(long out_width, long out_height) {
    double ratio_x = (double)out_width / width;
    double ratio_y = (double)out_height / height;
    double org_x, org_y;
    int pixel_byte = is_gray ? 1 : 3;
    long u, v, x, y;
    unsigned char surrounding[2][2] = {0};
    unsigned char* result = new unsigned char[out_width * out_width * pixel_byte];
    
    for (y = 0; y < out_height; y++) {
        for (x = 0; x < out_width; x++) {
            //calculate (x, y)'s position in original bitmap (org_x, org_y)
            org_x = x / ratio_x;
            org_y = y / ratio_y;
            u = (int)org_x;
            v = (int)org_y;
            
            if (0 <= org_x && org_x < width - 1 && 0 <= org_y && org_y < height - 1) {
                for (int i = 0; i < pixel_byte; i++) {
                    surrounding[0][0] = bitmap_array[(v * width + u) * pixel_byte + i];
                    surrounding[0][1] = bitmap_array[(v * width + (u + 1)) * pixel_byte + i];
                    surrounding[1][0] = bitmap_array[((v + 1) * width + u) * pixel_byte + i];
                    surrounding[1][1] = bitmap_array[((v + 1) * width + (u + 1)) * pixel_byte + i];
                    
                    result[(y * out_width + x) * pixel_byte + i] = Interpolation_DoubleLinear_core(surrounding, org_x - u, org_y - v);
                }
            }
            else {
                //when the pixel is near the margin, use Neighbor Interpolation
                for (int i = 0; i < pixel_byte; i++) {
                    result[(y * out_width + x) * pixel_byte + i] = bitmap_array[(v * width + u) * pixel_byte + i];
                }
            }
        }
    }
    
    delete [] bitmap_array;
    bitmap_array = result;
    width = out_width;
    height = out_height;
}

void GeometryTrans::Zoom_Convolution(long out_width, long out_height) {
    double ratio_x = (double)out_width / width;
    double ratio_y = (double)out_height / height;
    double org_x, org_y;
    int pixel_byte = is_gray ? 1 : 3;
    int i, j;
    long u, v, x, y;
    unsigned char surrounding[4][4] = {0};
    unsigned char* result = new unsigned char[out_width * out_width * pixel_byte];
    
    for (y = 0; y < out_height; y++) {
        for (x = 0; x < out_width; x++) {
            //calculate (x, y)'s position in original bitmap (org_x, org_y)
            org_x = x / ratio_x;
            org_y = y / ratio_y;
            u = (int)org_x;
            v = (int)org_y;
            
            if (0 <= org_x && org_x < width - 2 && 0 <= org_y && org_y < height - 2) {
                for (int k = 0; k < pixel_byte; k++) {
                    for (j = (int)v - 1; j < v + 3; j++) {
                        for (i = (int)u - 1; i < u + 3; i++) {
                            surrounding[j - v + 1][i - u + 1] = bitmap_array[(j * width + i) * pixel_byte + k];
                        }
                    }
                    result[(y * out_width + x) * pixel_byte + k] = Interpolation_Convolution_core(surrounding, org_x - u, org_y - v);
                }
            }
            else {
                //when the pixel is near the margin, use Neighbor Interpolation
                for (int k = 0; k < pixel_byte; k++) {
                    result[(y * out_width + x) * pixel_byte + k] = bitmap_array[(v * width + u) * pixel_byte + k];
                }
            }
        }
    }
    
    delete [] bitmap_array;
    bitmap_array = result;
    width = out_width;
    height = out_height;
}

unsigned char GeometryTrans::Interpolation_DoubleLinear_core(unsigned char around[2][2], double x_pos, double y_pos) {
    int verti1, verti2;
    double result;
    
    //interpolation on vertical:
    verti1 = (1 - y_pos) * around[0][0] + y_pos * around[1][0];
    verti2 = (1 - y_pos) * around[0][1] + y_pos * around[1][1];
    //interpolation on horizontal
    result = ((1 - x_pos) * verti1 + x_pos * verti2);
    
    if (result > 255)
        result = 255;
    else if (result < 0)
        result = 0;
    
    return (unsigned char)result;
}

unsigned char GeometryTrans::Interpolation_Convolution_core(unsigned char around[4][4], double x_pos, double y_pos) {
    double col_matrix[4], row_matrix[4], result_AB[4], result_ABC = 0;
    double w;
    int i;
    
    //initial
    col_matrix[0] = x_pos + 1;
    col_matrix[1] = x_pos;
    col_matrix[2] = 1 - x_pos;
    col_matrix[3] = 2 - x_pos;
    row_matrix[0] = y_pos + 1;
    row_matrix[1] = y_pos;
    row_matrix[2] = 1 - y_pos;
    row_matrix[3] = 2 - y_pos;
    
    //calculate s(w):
    //s(w) = 1 - 2|w|^2 + |w|^3            ,|w| < 1;
    //       4 - 8|w| + 5|w|^2 - |w|^3     ,1 <= |w| < 2;
    //       0                             ,|w| >= 2.
    for (i = 0; i < 4; i++) {
        w = fabs(col_matrix[i]);
        if (w >= 0 && w < 1)
            col_matrix[i] = pow(w, 3) - 2 * pow(w, 2) + 1;
        else if (w >= 1 && w < 2)
            col_matrix[i] = - pow(w, 3) + 5 * pow(w, 2) - 8 * w + 4;
        else
            col_matrix[i] = 0;
    }
    for (i = 0; i < 4; i++) {
        w = fabs(row_matrix[i]);
        if (w >= 0 && w < 1)
            row_matrix[i] = pow(w, 3) - 2 * pow(w, 2) + 1;
        else if (w >= 1 && w < 2)
            row_matrix[i] = - pow(w, 3) + 5 * pow(w, 2) - 8 * w + 4;
        else
            row_matrix[i] = 0;
    }
    
    //calculate matrix multiplication:
    for (i = 0; i < 4; i++) {
        result_AB[i] = row_matrix[0] * around[0][i] +
                       row_matrix[1] * around[1][i] +
                       row_matrix[2] * around[2][i] +
                       row_matrix[3] * around[3][i];
    }
    for (i = 0; i < 4; i++) {
        result_ABC += result_AB[i] * col_matrix[i];
    }
    
    if (result_ABC > 255)
        result_ABC = 255;
    else if (result_ABC < 0)
        result_ABC = 0;
    
    return (unsigned char)result_ABC;
}

#endif /* GeometryTrans_Class_hpp */
