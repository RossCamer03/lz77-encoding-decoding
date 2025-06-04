#include "libpnm.h"
#include "lz77_encoding_function.h"
#include <time.h>
int main(int argc, char * argv[]){
    char *file_name = argv[1]; //take in file name and buffer size as arguments
    int buffer_size = atoi(argv[2]);
    float *avg_offset = malloc(sizeof(float)); //initialize pointers to store statistical values in encoding
    float *std_offset = malloc(sizeof(float));
    float *avg_length = malloc(sizeof(float));
    float *std_length = malloc(sizeof(float));
    time_t start = time(NULL); //capture time at function call
    Encode_Using_LZ77(file_name, buffer_size, avg_offset, std_offset, avg_length, std_length); //call encoding function
    time_t stop = time(NULL); //capture time at function return
    printf("Encoding Time: %ld - %ld = %ld\n", start,stop, start-stop); //print required data
    printf("Average Offset: %.6f\n", *avg_offset);
    printf("Standard Deviation Offset: %.6f\n", *std_offset);
    printf("Average length: %.6f\n", *avg_length);
    printf("Standard Deviation Length: %.6f\n", *std_length);
    return 0; //end of main
}