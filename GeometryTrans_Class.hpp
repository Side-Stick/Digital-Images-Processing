/* ***************************************************************************
 functions in this (ColorTrans_Class.hpp) hpp file:

 (1) GeometryTrans(bmpData org_bmp_img) : BitMapImg(org_bmp_img);
 
 (2) GeometryTrans(BitMapImg &org);
 * copy from a BitMapImg object.
 
 (3) (inline) void Zoom(long out_width, long out_height, int select_algorithm = 1);
    1-> void Zoom_Neighbor(long out_width, long out_height);
    2-> void Zoom_DoubleLinear(long out_width, long out_height);
    3-> void Zoom_Convolution(long out_width, long out_height);
 * Zoom the image to a given size.
 * 1 is the fastest, 3 is the clearest.
 
 (4) (inline) void Rotate(double degree, int select_algorithm = 1, unsigned char color_default = 255, bool cut = 0);
    1-> void Rotate_90(void);
    2-> void Rotate_180(void);
    3-> void Rotate_270(void);
    4-> void Rotate_Neighbor(double degree, unsigned char color_default, bool cut);
    5-> void Rotate_DoubleLinear(double degree, unsigned char color_default, bool cut);
    6-> void Rotate_Convolution(double degree, unsigned char color_default, bool cut);
 * Rotate (clockwise)(degree).
 * color_default: usually white(255) or black(0).
 * cut: what about the other part out of a rectangle, cut or remain?
 * [x']   [ cos(degree), sin(degree), 0][x]
 * [y'] = [-sin(degree), cos(degree), 0][y]
 * [1 ]   [ 0          , 0          , 1][1]
 * So, g(x', y') = f(u, v) : [(a,b), (c,d) are the center before & after rotation]
 * u = x'cos() - y'sin() - c cos() + d cos() + a;
 * v = x'sin() + y'cos() - c sin() - d cos() + b;
 
 (5) unsigned char Interpolation_DoubleLinear_core(unsigned char around[2][2], double x_pos, double y_pos);
 * Chinese name (utf-8): 双线性插值法
 * For single pixel.
 * Average of the surrounding 4 pixels.
 
 (6) unsigned char Interpolation_Convolution_core(unsigned char around[4][4], double x_pos, double y_pos);
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
    inline void Zoom(long out_width, long out_height, int select_algorithm);
    inline void Rotate(double degree, int select_algorithm, unsigned char color_default, bool cut);
    
private:
    unsigned char Interpolation_DoubleLinear_core(unsigned char around[2][2], double x_pos, double y_pos);
    unsigned char Interpolation_Convolution_core(unsigned char around[4][4], double x_pos, double y_pos);
    
    void Zoom_Neighbor(long out_width, long out_height);
    void Zoom_DoubleLinear(long out_width, long out_height);
    void Zoom_Convolution(long out_width, long out_height);
    
    void Rotate_90(void);
    void Rotate_180(void);
    void Rotate_270(void);
    void Rotate_Neighbor(double degree, unsigned char color_default, bool cut);
    void Rotate_DoubleLinear(double degree, unsigned char color_default, bool cut);
    void Rotate_Convolution(double degree, unsigned char color_default, bool cut);
};



inline void GeometryTrans::Zoom(long out_width, long out_height, int select_algorithm = 1) {
    if (out_width == width && out_height == height)
        return;
    
    if (1 == select_algorithm)
        Zoom_Neighbor(out_width, out_height);
    else if (2 == select_algorithm)
        Zoom_DoubleLinear(out_width, out_height);
    else if (3 == select_algorithm)
        Zoom_Convolution(out_width, out_height);
}

inline void GeometryTrans::Rotate(double degree, int select_algorithm = 1, unsigned char color_default = 255, bool cut = false) {
    const double eps = 1e-10;
    
    int degree_int = (int)degree;
    double degree_float = degree - degree_int;
    degree_int %= 360;
    degree = degree_int + degree_float;
    
    if (fabs(degree - 90) < eps)
        Rotate_90();
    else if (fabs(degree - 180) < eps)
        Rotate_180();
    else if (fabs(degree - 270) < eps)
        Rotate_270();
    else if (fabs(degree - 360) < eps)
        return;
    else {
        if (1 == select_algorithm)
            Rotate_Neighbor(degree, color_default, cut);
        else if (2 == select_algorithm)
            Rotate_DoubleLinear(degree, color_default, cut);
        else if (3 == select_algorithm)
            Rotate_Convolution(degree, color_default, cut);
    }
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

void GeometryTrans::Zoom_Neighbor(long out_width, long out_height) {
    double ratio_x = (double)out_width / width;
    double ratio_y = (double)out_height / height;
    long org_x, org_y;
    int pixel_byte = is_gray ? 1 : 3;
    long x, y;
    unsigned char* result = new unsigned char[out_width * out_height * pixel_byte];
    
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
    unsigned char* result = new unsigned char[out_width * out_height * pixel_byte];
    
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
    unsigned char* result = new unsigned char[out_width * out_height * pixel_byte];
    
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

void GeometryTrans::Rotate_90(void) {
    int pixel_byte = is_gray ? 1 : 3;
    long x, y, swap_temp;
    unsigned char* result = new unsigned char[height * width * pixel_byte];
    
    for (y = 0; y < width; y++) {
        for (x = 0; x < height; x++) {
            for (int i = 0; i < pixel_byte; i++) {
                result[(y * height + x) * pixel_byte + i] = bitmap_array[(x * width + (width - y - 1)) * pixel_byte + i];
            }
        }
    }
    
    delete [] bitmap_array;
    bitmap_array = result;
    swap_temp = width;
    width = height;
    height = swap_temp;
}

void GeometryTrans::Rotate_180(void) {
    int pixel_byte = is_gray ? 1 : 3;
    long x, y;
    unsigned char* result = new unsigned char[height * width * pixel_byte];
    
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            for (int i = 0; i < pixel_byte; i++) {
                result[(y * width + x) * pixel_byte + i] = bitmap_array[((height - y - 1) * width + (width - x - 1)) * pixel_byte + i];
            }
        }
    }
    
    delete [] bitmap_array;
    bitmap_array = result;
}

void GeometryTrans::Rotate_270(void) {
    int pixel_byte = is_gray ? 1 : 3;
    long x, y, swap_temp;
    unsigned char* result = new unsigned char[height * width * pixel_byte];
    
    for (y = 0; y < width; y++) {
        for (x = 0; x < height; x++) {
            for (int i = 0; i < pixel_byte; i++) {
                result[(y * height + x) * pixel_byte + i] = bitmap_array[((height - x - 1) * width + y) * pixel_byte + i];
            }
        }
    }
    
    delete [] bitmap_array;
    bitmap_array = result;
    swap_temp = width;
    width = height;
    height = swap_temp;
}

void GeometryTrans::Rotate_Neighbor(double degree, unsigned char color_default, bool cut) {
    long org_x, org_y;
    int pixel_byte = is_gray ? 1 : 3;
    long x, y, out_width, out_height;
    unsigned char* result;
    double before_edge_x[4], before_edge_y[4];
    double after_edge_x[4], after_edge_y[4];
    //0: left-up, 1: right-up, 2: left-down, 3: right-down.
    double temp1, temp2;
    
    double sin_d = sin(2 * (4 * atan(1)) * degree / 360);
    double cos_d = cos(2 * (4 * atan(1)) * degree / 360);
    before_edge_x[0] = - ((double)width  - 1) / 2;
    before_edge_y[0] =   ((double)height - 1) / 2;
    before_edge_x[1] =   ((double)width  - 1) / 2;
    before_edge_y[1] =   ((double)height - 1) / 2;
    before_edge_x[2] = - ((double)width  - 1) / 2;
    before_edge_y[2] = - ((double)height - 1) / 2;
    before_edge_x[3] =   ((double)width  - 1) / 2;
    before_edge_y[3] = - ((double)height - 1) / 2;
    after_edge_x[0] =  cos_d * before_edge_x[0] + sin_d * before_edge_y[0];
    after_edge_y[0] = -sin_d * before_edge_x[0] + cos_d * before_edge_y[0];
    after_edge_x[1] =  cos_d * before_edge_x[1] + sin_d * before_edge_y[1];
    after_edge_y[1] = -sin_d * before_edge_x[1] + cos_d * before_edge_y[1];
    after_edge_x[2] =  cos_d * before_edge_x[2] + sin_d * before_edge_y[2];
    after_edge_y[2] = -sin_d * before_edge_x[2] + cos_d * before_edge_y[2];
    after_edge_x[3] =  cos_d * before_edge_x[3] + sin_d * before_edge_y[3];
    after_edge_y[3] = -sin_d * before_edge_x[3] + cos_d * before_edge_y[3];
    
    if (cut) {
        out_width = (long)(MIN(fabs(after_edge_x[3] - after_edge_x[0]), fabs(after_edge_x[2] - after_edge_x[1])) + 0.5);
        out_height = (long)(MIN(fabs(after_edge_y[3] - after_edge_y[0]), fabs(after_edge_y[2] - after_edge_y[1])) + 0.5);
    }
    else {
        out_width = (long)(MAX(fabs(after_edge_x[3] - after_edge_x[0]), fabs(after_edge_x[2] - after_edge_x[1])) + 0.5);
        out_height = (long)(MAX(fabs(after_edge_y[3] - after_edge_y[0]), fabs(after_edge_y[2] - after_edge_y[1])) + 0.5);
    }
    
    result = new unsigned char[out_height * out_width * pixel_byte];
    
    temp1 = -0.5 * (out_width - 1) * cos_d + 0.5 * (out_height - 1) * sin_d + 0.5 * (width - 1);
    temp2 = -0.5 * (out_width - 1) * sin_d - 0.5 * (out_height - 1) * cos_d + 0.5 * (height - 1);
    
    for (y = 0; y < out_height; y++) {
        for (x = 0; x < out_width; x++) {
            org_x = (long)(x * cos_d - y * sin_d + temp1 + 0.5);
            org_y = (long)(x * sin_d + y * cos_d + temp2 + 0.5);
            
            if (org_x >= 0 && org_x < width && org_y >= 0 && org_y < height) {
                for (int i = 0; i < pixel_byte; i++) {
                    result[(y * out_width + x) * pixel_byte + i] = bitmap_array[(org_y * width + org_x) * pixel_byte + i];
                }
            }
            else {
                for (int i = 0; i < pixel_byte; i++) {
                    result[(y * out_width + x) * pixel_byte + i] = color_default;
                }
            }
        }
    }
    
    delete [] bitmap_array;
    bitmap_array = result;
    width = out_width;
    height = out_height;
}

void GeometryTrans::Rotate_DoubleLinear(double degree, unsigned char color_default, bool cut) {
    long org_x, org_y;
    int pixel_byte = is_gray ? 1 : 3;
    long x, y, out_width, out_height, u, v;
    unsigned char* result;
    double before_edge_x[4], before_edge_y[4];
    double after_edge_x[4], after_edge_y[4];
    //0: left-up, 1: right-up, 2: left-down, 3: right-down.
    double temp1, temp2;
    unsigned char surrounding[2][2];
    
    double sin_d = sin(2 * (4 * atan(1)) * degree / 360);
    double cos_d = cos(2 * (4 * atan(1)) * degree / 360);
    before_edge_x[0] = - ((double)width  - 1) / 2;
    before_edge_y[0] =   ((double)height - 1) / 2;
    before_edge_x[1] =   ((double)width  - 1) / 2;
    before_edge_y[1] =   ((double)height - 1) / 2;
    before_edge_x[2] = - ((double)width  - 1) / 2;
    before_edge_y[2] = - ((double)height - 1) / 2;
    before_edge_x[3] =   ((double)width  - 1) / 2;
    before_edge_y[3] = - ((double)height - 1) / 2;
    after_edge_x[0] =  cos_d * before_edge_x[0] + sin_d * before_edge_y[0];
    after_edge_y[0] = -sin_d * before_edge_x[0] + cos_d * before_edge_y[0];
    after_edge_x[1] =  cos_d * before_edge_x[1] + sin_d * before_edge_y[1];
    after_edge_y[1] = -sin_d * before_edge_x[1] + cos_d * before_edge_y[1];
    after_edge_x[2] =  cos_d * before_edge_x[2] + sin_d * before_edge_y[2];
    after_edge_y[2] = -sin_d * before_edge_x[2] + cos_d * before_edge_y[2];
    after_edge_x[3] =  cos_d * before_edge_x[3] + sin_d * before_edge_y[3];
    after_edge_y[3] = -sin_d * before_edge_x[3] + cos_d * before_edge_y[3];
    
    if (cut) {
        out_width = (long)(MIN(fabs(after_edge_x[3] - after_edge_x[0]), fabs(after_edge_x[2] - after_edge_x[1])) + 0.5);
        out_height = (long)(MIN(fabs(after_edge_y[3] - after_edge_y[0]), fabs(after_edge_y[2] - after_edge_y[1])) + 0.5);
    }
    else {
        out_width = (long)(MAX(fabs(after_edge_x[3] - after_edge_x[0]), fabs(after_edge_x[2] - after_edge_x[1])) + 0.5);
        out_height = (long)(MAX(fabs(after_edge_y[3] - after_edge_y[0]), fabs(after_edge_y[2] - after_edge_y[1])) + 0.5);
    }
    
    result = new unsigned char[out_height * out_width * pixel_byte];
    
    temp1 = -0.5 * (out_width - 1) * cos_d + 0.5 * (out_height - 1) * sin_d + 0.5 * (width - 1);
    temp2 = -0.5 * (out_width - 1) * sin_d - 0.5 * (out_height - 1) * cos_d + 0.5 * (height - 1);
    
    for (y = 0; y < out_height; y++) {
        for (x = 0; x < out_width; x++) {
            org_x = x * cos_d - y * sin_d + temp1;
            org_y = x * sin_d + y * cos_d + temp2;
            u = (long)org_x;
            v = (long)org_y;
            
            if (org_x >= 0 && org_x < width - 1 && org_y >= 0 && org_y < height - 1) {
                for (int i = 0; i < pixel_byte; i++) {
                    surrounding[0][0] = bitmap_array[(v * width + u) * pixel_byte + i];
                    surrounding[0][1] = bitmap_array[(v * width + (u + 1)) * pixel_byte + i];
                    surrounding[1][0] = bitmap_array[((v + 1) * width + u) * pixel_byte + i];
                    surrounding[1][1] = bitmap_array[((v + 1) * width + (u + 1)) * pixel_byte + i];
                    
                    result[(y * out_width + x) * pixel_byte + i] = Interpolation_DoubleLinear_core(surrounding, org_x - u, org_y - v);
                }
            }
            else {
                for (int i = 0; i < pixel_byte; i++) {
                    result[(y * out_width + x) * pixel_byte + i] = color_default;
                }
            }
        }
    }
    
    delete [] bitmap_array;
    bitmap_array = result;
    width = out_width;
    height = out_height;
}

void GeometryTrans::Rotate_Convolution(double degree, unsigned char color_default, bool cut) {
    long org_x, org_y;
    int pixel_byte = is_gray ? 1 : 3;
    long x, y, out_width, out_height, u, v;
    unsigned char* result;
    double before_edge_x[4], before_edge_y[4];
    double after_edge_x[4], after_edge_y[4];
    //0: left-up, 1: right-up, 2: left-down, 3: right-down.
    double temp1, temp2;
    int i, j;
    unsigned char surrounding[4][4];
    
    double sin_d = sin(2 * (4 * atan(1)) * degree / 360);
    double cos_d = cos(2 * (4 * atan(1)) * degree / 360);
    before_edge_x[0] = - ((double)width  - 1) / 2;
    before_edge_y[0] =   ((double)height - 1) / 2;
    before_edge_x[1] =   ((double)width  - 1) / 2;
    before_edge_y[1] =   ((double)height - 1) / 2;
    before_edge_x[2] = - ((double)width  - 1) / 2;
    before_edge_y[2] = - ((double)height - 1) / 2;
    before_edge_x[3] =   ((double)width  - 1) / 2;
    before_edge_y[3] = - ((double)height - 1) / 2;
    after_edge_x[0] =  cos_d * before_edge_x[0] + sin_d * before_edge_y[0];
    after_edge_y[0] = -sin_d * before_edge_x[0] + cos_d * before_edge_y[0];
    after_edge_x[1] =  cos_d * before_edge_x[1] + sin_d * before_edge_y[1];
    after_edge_y[1] = -sin_d * before_edge_x[1] + cos_d * before_edge_y[1];
    after_edge_x[2] =  cos_d * before_edge_x[2] + sin_d * before_edge_y[2];
    after_edge_y[2] = -sin_d * before_edge_x[2] + cos_d * before_edge_y[2];
    after_edge_x[3] =  cos_d * before_edge_x[3] + sin_d * before_edge_y[3];
    after_edge_y[3] = -sin_d * before_edge_x[3] + cos_d * before_edge_y[3];
    
    if (cut) {
        out_width = (long)(MIN(fabs(after_edge_x[3] - after_edge_x[0]), fabs(after_edge_x[2] - after_edge_x[1])) + 0.5);
        out_height = (long)(MIN(fabs(after_edge_y[3] - after_edge_y[0]), fabs(after_edge_y[2] - after_edge_y[1])) + 0.5);
    }
    else {
        out_width = (long)(MAX(fabs(after_edge_x[3] - after_edge_x[0]), fabs(after_edge_x[2] - after_edge_x[1])) + 0.5);
        out_height = (long)(MAX(fabs(after_edge_y[3] - after_edge_y[0]), fabs(after_edge_y[2] - after_edge_y[1])) + 0.5);
    }
    
    result = new unsigned char[out_height * out_width * pixel_byte];
    
    temp1 = -0.5 * (out_width - 1) * cos_d + 0.5 * (out_height - 1) * sin_d + 0.5 * (width - 1);
    temp2 = -0.5 * (out_width - 1) * sin_d - 0.5 * (out_height - 1) * cos_d + 0.5 * (height - 1);
    
    for (y = 0; y < out_height; y++) {
        for (x = 0; x < out_width; x++) {
            org_x = x * cos_d - y * sin_d + temp1;
            org_y = x * sin_d + y * cos_d + temp2;
            u = (long)org_x;
            v = (long)org_y;
            
            if (org_x >= 0 && org_x < width - 2 && org_y >= 0 && org_y < height - 2) {
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
                for (int k = 0; k < pixel_byte; k++) {
                    result[(y * out_width + x) * pixel_byte + k] = color_default;
                }
            }
        }
    }
    
    delete [] bitmap_array;
    bitmap_array = result;
    width = out_width;
    height = out_height;
}

#endif /* GeometryTrans_Class_hpp */
