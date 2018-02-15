#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int colorImage(char color[], int quadtrant, int fd); //Prototype

int main(int argc, char *argv[]) {

    char invalidArgCount[] = "Invalid amount of arguments";
    char fileErr[] = "Error creating file";
    char fileWriteErr[] = "Error writing to file";
    int fd;
    int status;

    char red[] = {'0','F','0'} ;
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

    int numOfProcesses = 2;

    char fileHeader[] = "P6\n1 1\n255\n";

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
       
        int pid;
        //for(int i = 0; i < numOfProcesses; i++, pid = wait(&status)) {
        //    if(fork() == 0) {
                colorImage(red, 1, fd);
        //    }
        //}
        close(fd);
    }


}

/*
    Writes to a file the color. Paints a specific quadrant a specific color
    params int color, int quadrant, file descriptor
    returns 1 on success, 0 on failure 
*/
int colorImage(char color[], int quadtrant, int fd) {

    unsigned char red[] = {0,255,0};
    write(fd, &red, 3);
    return 0;
}

