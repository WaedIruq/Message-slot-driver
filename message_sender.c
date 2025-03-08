#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message_slot.h"

int main(int argc, char const *argv[]) {
    int fd, msgLen;
    unsigned int channel_id;
    
    if (argc != 4) {
        perror("Number of arguments is incorrect it should be: 1.message slot file path, 2.message channel id, 3.message");
        exit(1);
    }
    fd = open(argv[1], O_WRONLY);
    if (fd == -1) {
        perror("Failed to open device file \n");
        exit(1);
    }
    channel_id = atoi(argv[2]); 
    if (ioctl(fd, MSG_SLOT_CHANNEL, channel_id) < 0){
        perror("Failed to set the channel id \n");
        exit(1);
    }
    msgLen = strlen(argv[3]);
    if (write(fd, argv[3], msgLen) != msgLen) {
        perror("Failed to write to the message slot file \n");
        exit(1);
    }
    close(fd);
    exit(0);
}