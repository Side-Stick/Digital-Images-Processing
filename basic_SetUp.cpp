bool is_little_endian = 0;

bool get_system_endian (void) {
    union checkCPU {
        int checkCPU_longer;
        unsigned char checkCPU_1byte;
    } checker;
    
    checker.checkCPU_longer = 1;
    return (1 == checker.checkCPU_1byte);
}

void initial (void) {    
    is_little_endian = get_system_endian();
//    printf("is little_endian: %d\n", is_little_endian);
}
