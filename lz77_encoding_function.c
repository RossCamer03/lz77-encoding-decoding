#include "libpnm.h"
#include <math.h>
#include <stdio.h>
void Encode_Using_LZ77(char *in_PGM_filename_Ptr, unsigned int searching_buffer_size, float *avg_offset_Ptr, float *std_offset_Ptr, float *avg_length_Ptr, float *std_length_Ptr){
    int curr_offset = 0; //initalize variables to be used in finding best match
    int best_offset = 0;
    int curr_length = 0;
    int best_length = 0;
    bool next_match = true; //bool to be used for while loop (match searching)
    struct PGM_Image *pgm = (struct PGM_Image *)malloc(sizeof(struct PGM_Image)); //create pgm pointer
    load_PGM_Image(pgm, in_PGM_filename_Ptr); //load pgm
    int width = pgm->width; //get header information
    int height = pgm->height;
    int searchingBuffer[searching_buffer_size]; //create searching buffer
    int image_array[width*height]; //create image array to hold image data in 1d array (not 2d)
    int offset_array[width*height]; //create arrays to hold token information
    int length_array[width*height];
    int token_array[width*height];
    int i = 0;
    int data_store_idx = 0; //variable to be used when storing tokens in arrays
    int curr_buff_idx = 0; //index of the next open element of the search buffer
    for(int x = 0; x < height; x++){ //iterate through image data... load this data into 1d array
        for(int y = 0; y < width; y++){
            image_array[i] = pgm->image[x][y];
            i++;
        }
    }
    for(int q = 0; q<(width*height);q++){ //iterate through image array using q
        int start = q; //initialize start which holds the index of teh current element in image array
        for(int r = 0; r<curr_buff_idx; r++){ //iterate through search buffer with r
            if(image_array[q] == searchingBuffer[r]){ //on first match
                start = q; //ensure start is set to q (solves issue with iteration later in program)
                next_match = true; //set next match to true
                curr_offset = (start - r)%searching_buffer_size; //get the current offset which will change as search buffer resets (hence %searching_buffer_size)
                curr_length++; //increment the length of current match
                r++; //move to next element in search buffer
                start++; //move to next element in image array
                while(next_match){
                    if(image_array[start] == searchingBuffer[r]){ //if another match is found increment length of match and position in both arrays
                        curr_length++;
                        r++;
                        start++;
                    }
                    else{ //if no match found then end of current match
                        if(curr_length>best_length){ //decide if match is best found yet
                            best_length = curr_length;
                            best_offset = curr_offset;
                        }
                        else if(curr_length == best_length){
                            if(curr_offset < best_offset){
                                best_offset = curr_offset;
                            }
                        }
                        curr_length = 0; //reset varaiables
                        curr_offset = 0;
                        next_match = false; //end loop
                    }
                }
            }
        }
        if((curr_buff_idx+best_length) >= searching_buffer_size){ //if search buffer about to fill
            for(int i=0; i<searching_buffer_size; i++){ //reset search buffer
                searchingBuffer[i] = 0;

            }
            curr_buff_idx = 0; //set curr buffer index to 0
            start -= best_length; //move back start
            best_offset = 0; //reset best match varaibles
            best_length = 0;

        }
        else{ //if buffer not about to fill
            offset_array[data_store_idx] = best_offset; //store token values in array
            length_array[data_store_idx] = best_length;
            token_array[data_store_idx] = image_array[q+best_length];
            data_store_idx++; //increment position for next token saved
            for(int z = q; z <= q+best_length; z++){ //append match to searching buffer
                searchingBuffer[curr_buff_idx] = image_array[z];
                curr_buff_idx++;
            }
            q+=(best_length); //move current position in image array past elements moved to search buffer
        }
        best_offset = 0; //reset best match variables
        best_length = 0;
        
    }
    //write header
    FILE *file;
    char file_name[100];
    snprintf(file_name, sizeof(file_name), "%s.%d.lz", in_PGM_filename_Ptr, searching_buffer_size); //concatenate file name
    file = fopen(file_name, "w"); //open file for writing 
    fprintf(file, "P2\n%d\n%d\n%d\n%d\n", width, height, pgm->maxGrayValue, searching_buffer_size); //add header info
    //write arrays to file
    fprintf(file, "[\n");
    for(int a = 0; a<data_store_idx;a++){
        fprintf(file, "%d\n",offset_array[a]);
    }
    fprintf(file, "]\n");
    fprintf(file, "[\n");
     for(int b = 0; b<data_store_idx;b++){
        fprintf(file, "%d\n", length_array[b]);
    }
    fprintf(file, "]\n");
    fprintf(file, "[\n");
     for(int c = 0; c<data_store_idx;c++){
        fprintf(file, "%d\n", token_array[c]);
    }
    fprintf(file, "]\n");
    fclose(file);
    FILE *file1;
    char file_name1[100];
    snprintf(file_name1, sizeof(file_name1), "%s.%d.offsets.csv", in_PGM_filename_Ptr, searching_buffer_size); //concatenate file name
    file1 = fopen(file_name1, "w"); //open new file for writing
    int offset_freq[searching_buffer_size]; //initialize offset frequency array
    for (int i = 0; i < searching_buffer_size; i++) {
        offset_freq[i] = 0;
    }
    for(int a = 0; a<data_store_idx;a++){ //each time an offset is found, the element at that index of the frequency array is incremented by 1
        int index = offset_array[a];
        int value = offset_freq[index];
        value++;
        offset_freq[index] = value;
    }
    for(int a = 0; a<(searching_buffer_size);a++){ //print array to file
        if(offset_freq[a] != 0){
            fprintf(file1, "%d, %d\n", a, offset_freq[a]);
        }
    }
    fclose(file1);
    //repeat previous steps for length frequency file
    FILE *file2;
    char file_name2[100];
    snprintf(file_name2, sizeof(file_name2), "%s.%d.lengths.csv", in_PGM_filename_Ptr, searching_buffer_size);
    file2 = fopen(file_name2, "w");
    int length_freq[searching_buffer_size];
    for (int i = 0; i < searching_buffer_size; i++) {
        length_freq[i] = 0;
    }
    for(int a = 0; a<data_store_idx;a++){
        int index = length_array[a];
        int value = length_freq[index];
        value++;
        length_freq[index] = value;
    }
    for(int a = 0; a<(searching_buffer_size);a++){
        if(length_freq[a] != 0){
            fprintf(file2, "%d, %d\n", a, length_freq[a]);
        }
    }
    //calculate stats
    float off_sum = 0.0; //initialize variables for calculations
    float off_avg;
    float off_std = 0.0;
    int num_elements = data_store_idx;
    for(int a = 0; a<data_store_idx;a++){ //find sum
        off_sum += offset_array[a];
    }
    off_avg = off_sum/num_elements; //divide sum by number of elements
    *avg_offset_Ptr = off_avg; //assign average value to average pointer
    for(int a = 0; a<data_store_idx;a++){ //first step in standard dev
        off_std += pow(offset_array[a] - off_avg, 2);
    }
    float off_standard_dev = sqrt(off_std/num_elements); //take root and divide previously found value by number of elements
    *std_offset_Ptr = off_standard_dev; //save value to pointer
    
    float len_sum = 0.0; //repeat steps for length average and standard dev
    float len_avg;
    float len_std = 0.0;
    for(int a = 0; a<data_store_idx;a++){
        len_sum += length_array[a];
    }
    len_avg = len_sum/num_elements;
    *avg_length_Ptr = len_avg;
    for(int a = 0; a<data_store_idx;a++){
        len_std += pow(length_array[a] - len_avg, 2);
    }
    float len_standard_dev = sqrt(len_std/num_elements);
    *std_length_Ptr = len_standard_dev;
    free_PGM_Image(pgm); //free memeory
}