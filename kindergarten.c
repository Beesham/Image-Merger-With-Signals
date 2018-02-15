#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int colorImage(unsigned char color[], int quadtrant, int fd); //Prototype

int main(int argc, char *argv[]) {

    char invalidArgCount[] = "Invalid amount of arguments";
    char fileErr[] = "Error creating file";
    char fileWriteErr[] = "Error writing to file";
    int fd;
    int status;
       
    unsigned char red[] = {255,0,0};
    int green = 120;
    int blue = 130;
    int yellow = 140;
    int orange = 150;
    int cyan = 160;
    int magenta = 170;
    int ocean = 180;
    int violet = 190;

    int width = 1000;
    int height = 1000;

    int numOfProcesses = 1;

    char fileHeader[] = "P6\n1000 1000\n255\n";

    if(argc  != 7) {
        write(STDOUT_FILENO, invalidArgCount, sizeof(invalidArgCount));    
        exit(0);
    }

    //Create the file
    if((fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0755)) == -1) write(STDOUT_FILENO, fileErr, sizeof(fileErr));
    else {
        //Write header to file
        if((write(fd, fileHeader, sizeof(fileHeader)-1)) == -1) {
            write(STDOUT_FILENO, fileWriteErr, sizeof(fileWriteErr));
            close(fd);
            exit(1);
        }
       
        for(int i = 0; i < numOfProcesses; i++) {
            colorImage(red, 1, fd);
        }
        close(fd);
    }


}

/*
    Writes to a file the color. Paints a specific quadrant a specific color
    params int color, int quadrant, file descriptor
    returns 1 on success, 0 on failure 
*/
int colorImage(unsigned char color[], int quadtrant, int fd) {
    printf("in colorImage\n");
    
    if(fork() == 0) {
        printf("%s", "in child\n");
        unsigned char buff[3][1000] = {0};
        for(int i = 0; i < 1000; i++) {
            for(int j = 0; j < 3; j++) {
                 buff[i][j] = color[j];           
            }
        }
       
        
        // for(int i = 0; i < 250; i++) {
        //if((write(fd, &buff, 3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            write(fd, "eee", 3);
        // }
    }else{
        printf("%s", "in parent\n");
    }


    return 0;
}

