#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

int colorImage(unsigned char color[], int quadtrant, int fd); //Prototype
void fill2DArrayWithColor(int row, int column,  unsigned char array[][column],unsigned char color1[], unsigned char color2[]);
void fill3DArrayWithColor(int row, int column, int z, unsigned char array[][column][z], unsigned char color1[], unsigned char color2[], unsigned char color3[]);

int main(int argc, char *argv[]) {

    char invalidArgCount[] = "Invalid amount of arguments";
    char fileErr[] = "Error creating file";
    char fileWriteErr[] = "Error writing to file";
    int fd;
    int status;
       
    unsigned char red[] = {255,0,0};
    unsigned char green[] ={0,255,0};
    unsigned char blue[] = {0,0,255};
    unsigned char yellow[] = {255,255,0};
    unsigned char orange[] = {255,165,0};
    unsigned char cyan[] = {0,255,255};
    unsigned char magenta[] = {255,0,255};
    unsigned char ocean[] = {30,144,255};
    unsigned char violet[] = {238,130,238};

    int width = 1000;
    int height = 1000;
    int diamondNorth = height/4;
    int diamondSouth = diamondNorth*3;
    int diamondWest = width/4;
    int diamondEast = diamondWest*3; 

    int numOfProcesses = 10;

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
    int row = 1000;
    int column = 3;

    unsigned char randomcolor[] = {255,255,50};

    int pid = fork();
    if(pid == 0) {
        unsigned char buff[1000][3] =  {{0}};
        fill2DArrayWithColor(row, column, buff, color, randomcolor);  
        //Writes 100 row of 1000px
        for(int i = 0; i < 100; i++) { 
            if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
        }
            write(fd, "\n", 1);
            exit(0);
    }else{
        int status;
        printf("CHild pid: %d",pid);
        waitpid(pid,&status,0);
        printf("%s: %d : %d", "in parent ", getpid(), status);
    }


    return 0;
}

void fill2DArrayWithColor(int row, int column,  unsigned char array[][column],unsigned char color1[], unsigned char color2[]) {
    
    //Fills 1 rows of 1000px
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < column; j++) {
            if(i < row/2) 
                array[i][j] = color1[j];   
            else
                array[i][j] = color2[j];   
        }
    }
}

