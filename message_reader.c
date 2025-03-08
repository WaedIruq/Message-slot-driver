#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>

#include "message_slot.h"

int main(int argc, char const *argv[]) {
    int fd, msgLen;
    unsigned int channel_id;
    char buffer[128];

    if (argc != 3) {
        perror("Number of arguments is incorrect it should be: 1.message slot file path, 2.message channel id");
        exit(1);
    }
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Failed to open device file \n");
        exit(1);
    }
    channel_id = atoi(argv[2]); 
    if (ioctl(fd, MSG_SLOT_CHANNEL, channel_id) < 0){
        perror("Failed to set the channel id \n");
        exit(1);
    }
    msgLen = read(fd, buffer,128);
    if ( msgLen < 0) {
        perror("Failed to read from message slot file \n");
        exit(1);
    }
    close(fd);
    if (write(1, buffer,  msgLen) != msgLen) {
        perror("Failed to write the message to stdout \n");
        exit(1);
    }
    exit(0);
}