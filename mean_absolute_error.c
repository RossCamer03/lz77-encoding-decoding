#include "libpnm.h"
#include <stdlib.h>
#include <math.h> 
float mean_absolute_error(char *file_name_1_ptr, char *file_name_2_ptr){
    struct PGM_Image *file_1 = (struct PGM_Image *)malloc(sizeof(struct PGM_Image)); //load pgm images from file names
    load_PGM_Image(file_1,file_name_1_ptr);
    struct PGM_Image *file_2 = (struct PGM_Image *)malloc(sizeof(struct PGM_Image));
    load_PGM_Image(file_2,file_name_2_ptr);
    if(file_1->width != file_2->width || file_1->height != file_2->height){ //if files are not same size, abort
        return -1;
    }
    if(file_1->maxGrayValue < file_2->maxGrayValue){ //adjust smaller max gray value to size of larger max gray value
        int greater_gray = file_2->maxGrayValue;
        int lesser_gray = file_1->maxGrayValue;
        int multiplier = greater_gray/lesser_gray;
        file_1->maxGrayValue = multiplier * lesser_gray;
        for(int x=0;x<file_1->height; x++){
            for(int y=0;y<file_1->width; y++){
                file_1->image[x][y] = file_1->image[x][y] * multiplier; //use multiplier to adjust each element of lesser gray value image
            }
        }
    }
    else if(file_1->maxGrayValue > file_2->maxGrayValue){ //repeat steps for other file in case file 2 has the smaller gray value
        int greater_gray = file_1->maxGrayValue;
        int lesser_gray = file_2->maxGrayValue;
        int multiplier = greater_gray/lesser_gray;
        file_2->maxGrayValue = multiplier * lesser_gray;
        for(int x=0;x<file_2->height; x++){
            for(int y=0;y<file_2->width; y++){
                file_2->image[x][y] = file_2->image[x][y] * multiplier;
            }
        }
    }
    float sum = 0; //initialize sum
    int n = file_1->width*file_1->height; //find n (all elements in list)
    for(int x=0;x<file_2->height; x++){ //for each element get the absolute value of file1-file2
            for(int y=0;y<file_2->width; y++){
                sum+=abs(file_1->image[x][y] - file_2->image[x][y]); //add to sum
            }
    }
    float mae = sum/n; //mean absolute error is the sum divided by n
    return mae; //return mean absolute error
}