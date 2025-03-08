#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H


#define MSG_SLOT_CHANNEL _IOW(MAJOR_NUM, 0, unsigned int)
#define DEVICE_FILE_NAME "message_slot"
#define SUCCESS 0
#define MAJOR_NUM 235

struct message_slot{
    char* buffer;
    int msg_len;
    int id;
    struct message_slot* next;

} ;


struct device_file{
    int minor;
    struct message_slot* head;
    struct device_file* next;
};

#endif