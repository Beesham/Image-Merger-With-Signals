/*
 * Beesham Sarendranaauth, 104854956
 * 2018/02/18
 * Assigment 2: Color an image.ppm with 5 colors using seperate processes in sequence and signals IPC
 * 
 * 
*/

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

//Prototypes

//Colors the image
int colorImage(int fd, int procCount); 

//Fills a 2D array with 2 colors
void fill2DArrayWithColor(int row, int column,  unsigned char array[][column], unsigned char color1[], unsigned char color2[]);

//Fills a 2D array  with 3 colors
void fillArrayTriColor(int row, int column, unsigned char array[][column], unsigned char color1[], unsigned char color2[], unsigned char color3[], int midColNum); 

//Setup an array of key-value pairs of possible colors
void fillColorKeyValue();  

//Assigns the colors chosen to their respective quadrant on the image
void assignColorsToQuadrant(int argc, char *argv[]); 

//Validates colors
int validateColors();

//Signal Handler for child processes
void signalHandlerChild(int signo);

//Signal handler for parent proecess
void signalHandlerParent(int signo);

#define numOfProcesses  10
int maxProc;    //maximum number of processes to use
int sizeOfColorArray = 9;   

typedef struct{
    unsigned char center[3];
    unsigned char topLeft[3];
    unsigned char topRight[3];
    unsigned char bottomLeft[3];
    unsigned char bottomRight[3];
} Quadrants;

typedef const struct{
    char *red; 
    char *green;
    char *blue;
    char *yellow;
    char *orange;
    char *cyan;
    char *magenta;
    char *ocean;
    char *violet;
} ColorsKey;

typedef const struct{
    unsigned char red[3];
    unsigned char green[3];
    unsigned char blue[3];
    unsigned char yellow[3];
    unsigned char orange[3];
    unsigned char cyan[3];
    unsigned char magenta[3];
    unsigned char ocean[3];
    unsigned char violet[3];
} ColorsValue;

typedef struct{
    char colorName[20];
    unsigned char colorValue[3];
} ColorKeyValue;

ColorKeyValue colorKV[10]; 

Quadrants quadrant;
ColorsKey colorsKey = {"red", "green", "blue", "yellow", "orange", "cyan", "magenta", "ocean", "violet"};
ColorsValue colorsValue = {
    {255,0,0},
    {0,255,0},
    {0,0,255},
    {255,255,0},
    {255,165,0},
    {0,255,255},
    {255,0,255},
    {30,144,255},
    {238,130,238}
};

//Child process checks this flags for 'go ahead' to write to file
int FLAG_WRITE_PERM = 0;

//List of child pids the parent keeps as a queue
int childPids[numOfProcesses];

int main(int argc, char *argv[]) {

    char invalidArgCount[] = "Invalid amount of arguments";
    char fileErr[] = "Error creating file";
    char fileWriteErr[] = "Error writing to file";
    char unsupCol[] = "Unsupported color";
    int fd;

    maxProc = numOfProcesses;

    char fileHeader[] = "P6\n1000 1000\n255\n";
    fillColorKeyValue();

    if(argc  != 7) {
        write(STDOUT_FILENO, invalidArgCount, sizeof(invalidArgCount));    
        exit(0);
    }

    if(!validateColors(argc, argv)) {
        write(STDOUT_FILENO, unsupCol, sizeof(unsupCol));
        exit(0);
    }

    assignColorsToQuadrant(argc, argv);

    //Create the file
    if((fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0755)) == -1) write(STDOUT_FILENO, fileErr, sizeof(fileErr));
    else {
        //Write header to file
        if((write(fd, fileHeader, sizeof(fileHeader)-1)) == -1) {
            write(STDOUT_FILENO, fileWriteErr, sizeof(fileWriteErr));
            close(fd);
            exit(1);
        }
      
        
        signal(SIGUSR2, signalHandlerParent);
        //Colors the image  
        for(int i = 0; i < numOfProcesses; i++) {
            childPids[i] = colorImage(fd, i);
            printf("Child pid: %d\n", childPids[i]);
        }
        close(fd);
    }


}

/*
    Handles the signals for the IPC 
*/
void signalHandlerChild(int signo) {

}

/*
    Habdles the sidnals for parent proc
*/
void signalHandlerParent(int signo) {
    if(signo == childPids[0]) {
        int pid = childPids[0];
        kill(pid, SIGUSR1);

        int sizeOfArr = sizeof(childPids) / sizeof(int);
        for(int i = 0; i < sizeOfArr; i++) {
            childPids[i] = childPids[i+1];   
        }
    }
}

/*
    Writes to a file 4 colors in specific orientation. Paints a specific quadrant a specific color 
*/
int colorImage(int fd, int procCount) {
    int row = 1000;
    int column = 3;

    int pid = fork();
    if(pid == 0) {
        unsigned char buff[1000][3] =  {{0}};
        
        //Writes a number of rows with specific color arrangement based on the process count
        if(procCount < 2) {
            //Fill the array with colors to write to file
            fill2DArrayWithColor(row, column, buff, quadrant.topLeft, quadrant.topRight);  
            signal(SIGUSR1, signalHandlerChild);
            pause();
            //Writes 100 row of 1000px
            for(int i = 0; i < 100; i++) { 
                if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            }
        }else if(procCount == 2) {
            fill2DArrayWithColor(row, column, buff, quadrant.topLeft, quadrant.topRight);  
            signal(SIGUSR1, signalHandlerChild);
            pause();
            for(int i = 0; i < 50; i++) { 
                if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            }

            for(int i = 0; i < 50; i++) { 
                fillArrayTriColor(row, column, buff, quadrant.topLeft, quadrant.center, quadrant.topRight, i);  
                if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            }
        }else if(procCount == 3) { 
            signal(SIGUSR1, signalHandlerChild);
            pause();
            for(int i = 0; i < 100; i++) { 
                fillArrayTriColor(row, column, buff, quadrant.topLeft, quadrant.center, quadrant.topRight, i+50);  
                if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            }
        }else if(procCount == 4) { 
            signal(SIGUSR1, signalHandlerChild);
            pause();
            for(int i = 0; i < 100; i++) { 
                fillArrayTriColor(row, column, buff, quadrant.topLeft, quadrant.center, quadrant.topRight, i+150);  
                if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            }
        }else if(procCount == 5) { 
            signal(SIGUSR1, signalHandlerChild);
            pause();
            for(int i = 0; i < 100; i++) { 
                fillArrayTriColor(row, column, buff, quadrant.bottomLeft, quadrant.center, quadrant.bottomRight, 250-i);  
                if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            }
        }else if(procCount == 6) { 
            signal(SIGUSR1, signalHandlerChild);
            pause();
            for(int i = 0; i < 100; i++) { 
                fillArrayTriColor(row, column, buff, quadrant.bottomLeft, quadrant.center, quadrant.bottomRight, 150-i);  
                if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            }
        }else if(procCount == 7) {
            signal(SIGUSR1, signalHandlerChild);
            pause();
            for(int i = 0; i < 50; i++) { 
                fillArrayTriColor(row, column, buff, quadrant.bottomLeft, quadrant.center, quadrant.bottomRight, 50 - i);  
                if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            }
   
            fill2DArrayWithColor(row, column, buff, quadrant.bottomLeft, quadrant.bottomRight);  
            for(int i = 0; i < 50; i++) { 
                if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            }
            
        }else{
            fill2DArrayWithColor(row, column, buff, quadrant.bottomLeft, quadrant.bottomRight);  
            signal(SIGUSR1, signalHandlerChild);
            pause();
            for(int i = 0; i < 100; i++) { 
                if((write(fd, &buff, sizeof(buff[0][0])*3000)) < 0) write(STDOUT_FILENO, "ERR WRITING", 11);
            }
        }
        
        if (procCount == maxProc-1) write(fd, "\n", 1);
        exit(0);
    }else{
        int status;
        //signal(SIGUSR2, signalHandlerParent);
        //waitpid(pid,&status,0);
        return pid;
    }

    return 0;
}

void fill2DArrayWithColor(int row, int column, unsigned char array[][column], unsigned char color1[], unsigned char color2[]) {
    
    //Fills n rows of 1000px
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < column; j++) {
            if(i < row/2) 
                array[i][j] = color1[j];   
            else
                array[i][j] = color2[j];   
        }
    }
}

void fillArrayTriColor(int row, int column, unsigned char array[][column], unsigned char color1[], unsigned char color2[], unsigned char color3[], int midColNum) {
     //Fills 1 rows of 1000px
    for(int i = 0; i < row; i++) {
        for(int j = 0; j < column; j++) {
                if(i < (row/2) - midColNum) 
                    array[i][j] = color1[j];   
                else if(i > (row/2) - midColNum && i < (row/2) + midColNum) 
                    array[i][j] = color2[j];
                else
                    array[i][j] = color3[j];
        }
    }

}


void fillColorKeyValue() {
    ColorKeyValue red;
    strcpy(red.colorName, colorsKey.red);
    memcpy(red.colorValue, colorsValue.red, 3);

    ColorKeyValue green;
    strcpy(green.colorName, colorsKey.green);
    memcpy(green.colorValue, colorsValue.green, 3);

    ColorKeyValue blue;
    strcpy(blue.colorName, colorsKey.blue);
    memcpy(blue.colorValue, colorsValue.blue, 3);

    ColorKeyValue yellow;
    strcpy(yellow.colorName, colorsKey.yellow);
    memcpy(yellow.colorValue, colorsValue.yellow, 3);

    ColorKeyValue orange; 
    strcpy(orange.colorName, colorsKey.orange);
    memcpy(orange.colorValue, colorsValue.orange, 3);

    ColorKeyValue cyan;
    strcpy(cyan.colorName, colorsKey.cyan);
    memcpy(cyan.colorValue, colorsValue.cyan, 3);

    ColorKeyValue magenta;
    strcpy(magenta.colorName, colorsKey.magenta);
    memcpy(magenta.colorValue, colorsValue.magenta, 3);

    ColorKeyValue ocean;
    strcpy(ocean.colorName, colorsKey.ocean);
    memcpy(ocean.colorValue, colorsValue.violet, 3);

    ColorKeyValue violet;
    strcpy(violet.colorName, colorsKey.violet);
    memcpy(violet.colorValue, colorsValue.violet, 3);

    colorKV[0] = red;
    colorKV[1] = green;
    colorKV[2] = blue;
    colorKV[3] = yellow;
    colorKV[4] = orange;
    colorKV[5] = cyan;
    colorKV[6] = magenta;
    colorKV[7] = ocean;
    colorKV[8] = violet;
}

void assignColorsToQuadrant(int argc, char *argv[]) {
    for(int j = 0; j < sizeOfColorArray; j++) {
        if((strcmp(colorKV[j].colorName, argv[2])) == 0) {
            memcpy(quadrant.center, colorKV[j].colorValue, 3); 
        }
        if(strcmp(colorKV[j].colorName, argv[3]) == 0) {
            memcpy(quadrant.topLeft, colorKV[j].colorValue, 3); 
        }
        if(strcmp(colorKV[j].colorName, argv[4]) == 0) {
            memcpy(quadrant.topRight, colorKV[j].colorValue, 3); 
        }
        if(strcmp(colorKV[j].colorName, argv[5]) == 0) {
            memcpy(quadrant.bottomLeft, colorKV[j].colorValue, 3); 
        }
        if(strcmp(colorKV[j].colorName, argv[6]) == 0) {
            memcpy(quadrant.bottomRight, colorKV[j].colorValue, 3); 
        }    
    }
}

int validateColors(int argc, char *argv[]) {
    for(int i = 2; i < 7; i++) { 
        for(int j = 0; j < sizeOfColorArray; j++) {
            if((strcmp(colorKV[j].colorName, argv[i])) == 0) {
                j = sizeOfColorArray;
            }else if((strcmp(colorKV[j].colorName, argv[i])) != 0 && j == sizeOfColorArray-1) {
                return 0;
            }
        }
    }
    return 1;
}








