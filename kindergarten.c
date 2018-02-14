#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    char invalidArgCount[] = "Invalid amount of arguments";
    char fileErr[] = "Error creating file";
    int fd;

    int red =  110;
    int green = 120;
    int blue = 130;
    int yellow = 140;
    int orange = 150;
    int cyan = 160;
    int magenta = 170;
    int ocean = 180;
    int violet = 190;

    if(argc  != 6) {
        write(STDOUT_FILENO, invalidArgCount, sizeof(invalidArgCount));    
        exit(0);
    }

    if((fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0755)) == -1) write(STDOUT_FILENO, fileErr, sizeof(fileErr));
    else {
        

        close(fd);
    }


}
