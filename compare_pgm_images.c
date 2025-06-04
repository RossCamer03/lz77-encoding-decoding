#include "libpnm.h"
#include "mean_absolute_error.h"
int main(int argc, char * argv[]){
    char *file_1_name = argv[1]; //accept file names as arguments
    char *file_2_name = argv[2];
    float mae = mean_absolute_error(file_1_name, file_2_name); //compare files using men_absolute_error()
    printf("M.A.E: %.6f\n", mae); //print result
    return 0; //end of program
}