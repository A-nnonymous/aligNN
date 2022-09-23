#include "main.h"

int main(int argc, const char * argv[]) {
    //If user requests help
    if (!(strcmp(argv[1], "-h")) || 
        !(strcmp(argv[1], "--help") ) || 
        argc < 3) {
        show_usage();
        return 0;
    }
    

    return 0;
}