#define running_timer

#include <iostream>
#include "top_index.h"

#ifdef running_timer
    #include <ctime>
    clock_t total_start_time, total_end_time;
    clock_t part_start_time, part_end_time;
#endif

int main (int argc, const char * argv[]) {
    bmpData buffer;
    char ch = '\0';
    int i;

    char read_path[70] = {'\0'};
    char save_path[70] = {'\0'};
    
    std::cout << "input a read_path：";
    i = (int)strlen(read_path);
    ch = fgetc(stdin);
    while (ch != '\n' && ch != EOF) {
        read_path[i] = ch;
        i++;
        ch = fgetc(stdin);
    }
    strcat(read_path, ".bmp");
    std::cout << read_path << std::endl;
    
    std::cout << "input a save_path：";
    i = (int)strlen(save_path);
    ch = fgetc(stdin);
    while (ch != '\n' && ch != EOF) {
        save_path[i] = ch;
        i++;
        ch = fgetc(stdin);
    }
    strcat(save_path, "-out.bmp");
    std::cout << save_path << std::endl;
        
    #ifdef running_timer
        total_start_time = clock();
    #endif
        
    initial();
    
    try {
        buffer = ReadBmp(read_path);
    } catch (const int error1) {
        std::cerr << "error code: " << error1 << std::endl;
        exit(1);
    }
    
    BitMapImg *temp = new BitMapImg(buffer);
    ColorTrans *ptr = new ColorTrans(*temp);
    temp = NULL;
    
    ptr->ExponentStretch(128, 2, 0.6);
    
    try {
        SaveBmp(save_path, ptr->TransToBmp());
    } catch (const int error2) {
        std::cerr << "error code: " << error2 << std::endl;
        exit(1);
    }
    
    delete ptr;
    ptr = NULL;
    
#ifdef running_timer
    total_end_time = clock();
    printf("\nTotal Run-time: %f ms\n\n", (double)(total_end_time - total_start_time) / CLOCKS_PER_SEC * 1000);
#endif
    
    return 0;
}
