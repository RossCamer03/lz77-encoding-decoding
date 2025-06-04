#include "libpnm.h"
#include "lz77_decoding_function.h"
#include <time.h>
int main(int argc, char * argv[]){
    char *file_name = argv[1]; //take in and store argument
    time_t start = time(NULL); //capture time at function call
    Decode_Using_LZ77(file_name); //call function
    time_t stop = time(NULL); //capture time at function return
    printf("Decoding Time: %ld - %ld = %ld\n", start,stop, start-stop); //print required data
    return 0; //end of main
}