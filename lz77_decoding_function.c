#include "libpnm.h"
#include <string.h>
#include <stdio.h>
void Decode_Using_LZ77(char *in_compressed_filename_Ptr){
    FILE *file;
    file = fopen(in_compressed_filename_Ptr, "r"); //open file in read mode
    char buffer[255]; //creat buffer to take in file data one line at a time
    fgets(buffer, 255, file); //get first line of header (P2)
    fgets(buffer, 255, file); //get next line
    int width = atoi(buffer); //convert to int and store as width
    fgets(buffer, 255, file); //repeat for all other header data
    int height = atoi(buffer);
    fgets(buffer, 255, file);
    int max_gray_val = atoi(buffer);
    fgets(buffer, 255, file);
    int search_buffer_size = atoi(buffer);
    fgets(buffer, 255, file);
    int offset_array[width*height]; //create empty array to store offset values
    int i = 0; //i used to iterate through array
    int value; //value used to temporarily store data before moving to array
    if(strcmp(buffer,"[\n") == 0){ //when array start token is found
        while(strcmp(buffer,"]\n") != 0){ //until array end token is found
            fgets(buffer, 255, file); //get each line and store as int in array
            value = atoi(buffer);
            offset_array[i] = value;
            i++;
        }
    }
    int token_max = i; //i is the total number of tokens in each array (no need to iterate through all of array)
    i = 0;
    int length_array[width*height]; //repeat steps with length and token array
    fgets(buffer, 255, file);
    if(strcmp(buffer,"[\n") == 0){
        while(strcmp(buffer,"]\n") != 0){
            fgets(buffer, 255, file);
            value = atoi(buffer);
            length_array[i] = value;
            i++;
        }
    }
    i = 0;
    int token_array[width*height];
    fgets(buffer, 255, file);
    if(strcmp(buffer,"[\n") == 0){
        while(strcmp(buffer,"]\n") != 0){
            fgets(buffer, 255, file);
            value = atoi(buffer);
            token_array[i] = value;
            i++;
        }
    }
    //decoding
    int decoded_msg[width*height]; //create array to hold decoded message
    int msg_idx = 0;
    int searchingBuffer[search_buffer_size]; //create searching buffer
    int curr_buff_index = 0; //curr_buff index used to track current location in search buffer
    for(int x = 0; x < token_max; x++){ //iterate through arrays
        if(offset_array[x] != 0){ //if there is an offset
            for(int i = 0; i < length_array[x]; i++){ //find matching string
                searchingBuffer[curr_buff_index] = searchingBuffer[curr_buff_index-(offset_array[x])+i];
                decoded_msg[msg_idx] = searchingBuffer[curr_buff_index]; //add to search buffer and decoded message
                curr_buff_index++;
                msg_idx++;
            }
        }

        searchingBuffer[curr_buff_index] = token_array[x]; //add next token to search buffer and decoded message
        decoded_msg[msg_idx] = searchingBuffer[curr_buff_index];
        curr_buff_index++;
        msg_idx++;
    }
    struct PGM_Image *pgm = (struct PGM_Image *)malloc(sizeof(struct PGM_Image)); //create pgm image pointer
    create_PGM_Image(pgm, width, height, max_gray_val); //create pgm image
    int z = 0;
    for(int x = 0; x<height; x++){ //iterate through pgm image array and save decoded message
        for(int y = 0; y<width; y++){
            pgm->image[x][y] = decoded_msg[z];
            z++;
        }
    }
    char *out_file_name = strcat(in_compressed_filename_Ptr, ".pgm"); //concatenate filename
    save_PGM_Image(pgm,out_file_name, false); //save image
}