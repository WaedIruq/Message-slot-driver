


#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#include "message_slot.h"



struct message_slot* get_message_slot(int minor, int channel_id,int RorW);


MODULE_LICENSE("GPL");





static struct device_file* devices;






static int device_open(struct inode* inode, struct file* file){
    int minor;
    struct device_file* curr;
    struct device_file* last;
    struct message_slot* msgSlotInit;
    minor = iminor(inode);
    curr = devices;
    while(curr->minor!=-2){
        if(curr->minor == minor) return SUCCESS;
        curr = curr->next;
    }
    last = (struct device_file*)kmalloc(sizeof(struct device_file),GFP_KERNEL);
    last->minor = -2;
    curr->next = last;
    curr->minor = minor;
    if(!(curr->head)){
        msgSlotInit = (struct message_slot*)kmalloc(sizeof(struct message_slot),GFP_KERNEL);
        if(!msgSlotInit){
            printk("Could not allocate memory in open.\n");
            return -1;
        }
        curr->head = msgSlotInit;
        msgSlotInit->id = -2;
    }
    return SUCCESS;
}



static long device_ioctl(struct file* file, unsigned int i, unsigned long l){
    if(l<1)return -1;
    file->private_data = (void*)l;
    return SUCCESS;
}

static ssize_t device_read(struct file* file, char* buff,size_t size, loff_t* off){
        long channel_id;
        struct message_slot* messageSlot;
        if(!file->private_data){
            printk("No channel has been set for this fd");
            return -EINVAL;
        }
        channel_id = (long)file->private_data;
        messageSlot = get_message_slot(iminor(file_inode(file)), channel_id,1);

        if(!messageSlot || messageSlot->msg_len ==0){
            printk("No message in the channel");
            return -EWOULDBLOCK;
        }
        if(size < messageSlot->msg_len){
            printk("Buffer is too small");
            return -ENOSPC;
        }
        if(copy_to_user(buff, messageSlot->buffer,messageSlot->msg_len) != 0){
            printk("Failed to copy to user");
            return -1;
        };
        return messageSlot->msg_len;
}


static ssize_t device_write(struct file* file, const char __user* message, size_t size, loff_t* off){
        long channel_id;
        struct message_slot* messageSlot;
        char* new_buffer;
        if(!(file->private_data)){
            printk("No channel has been set for this fd]\n");
            return -EINVAL;
        }
        channel_id  = (long)file->private_data;

        if(size == 0 || size>128){
            printk("The passed message lenght is incorrect");
            return -EMSGSIZE;
        }
        messageSlot = get_message_slot(iminor(file_inode(file)), channel_id,0);
        
        if(!messageSlot){
            printk("No channel has been set for this fd]\n");
            return -EINVAL;
        }
        if(!messageSlot->buffer){
            new_buffer = (char *)kmalloc(sizeof(char) * (size + 1), GFP_KERNEL);
            if (!new_buffer) {
                printk("Failed to allocate memory for message buffer\n");
                return -ENOMEM; 
                }
            messageSlot->buffer = (char*)kmalloc(sizeof(char)*size, GFP_KERNEL);
            }
        else{
            kfree(messageSlot->buffer);
            new_buffer = (char *)kmalloc(sizeof(char) * size, GFP_KERNEL);
            if (!new_buffer) {
                printk("Failed to allocate memory for message buffer\n");
                return -ENOMEM; 
                }
            messageSlot->buffer = (char*)kmalloc(sizeof(char)*size, GFP_KERNEL);
        }
        if(copy_from_user(messageSlot->buffer, message,size) != 0){
            printk("Failed to copy from user\n");
            return -1;
        }
        
        
        

        messageSlot->msg_len = (int)size;
        return messageSlot->msg_len;
}




 struct message_slot* get_message_slot(int minor, int channel_id,int RorW){
    struct device_file* curr_device;
    struct message_slot* curr_slot, *last;
    if(!devices)return NULL;
    curr_device = devices;
    while(curr_device->minor != -2 && curr_device->minor != minor){
        curr_device = curr_device->next;
        }
    if(curr_device->minor == -2)return NULL;
    curr_slot = curr_device->head;
    while(curr_slot->id != -2){
        if(curr_slot->id == channel_id) return curr_slot;
        curr_slot = curr_slot->next;
        }
    if(RorW)return NULL;
    last = (struct message_slot*)kmalloc(sizeof(struct message_slot),GFP_KERNEL);
    if(!last){
        printk("Could not allocate memmory in get_message_slot helper function.\n");
        return NULL;
    }
    curr_slot->id = channel_id;
    curr_slot->next = last;
    last->id = -2;
    return curr_slot;
}


static struct file_operations Fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .unlocked_ioctl = device_ioctl,
};


static int __init init(void){
    int rc;
    rc = -1;
    rc = register_chrdev( MAJOR_NUM, DEVICE_FILE_NAME, &Fops);
    if( rc < 0 ) {
        printk( KERN_ALERT "%s registraion failed for  %d\n",DEVICE_FILE_NAME, MAJOR_NUM );
        return rc;
    }
    devices = (struct device_file*)kmalloc(sizeof(struct device_file),GFP_KERNEL);
    devices->minor = -2;

    return SUCCESS;
}

static void __exit cleanup_devices(void){
    struct device_file* prev,*curr;
    struct message_slot* prev_s,*curr_s;
    curr = devices;
    while(curr->minor != -2){
        curr_s = curr->head;
        while(curr_s->id != -2){
            kfree(curr_s->buffer);
            prev_s = curr_s;
            curr_s = curr_s->next;
            kfree(prev_s); 
        }
        kfree(curr_s);
        prev = curr;
        curr = curr->next;
        kfree(prev);
    }
    kfree(curr);
    unregister_chrdev(MAJOR_NUM, DEVICE_FILE_NAME);


}

module_init(init);
module_exit(cleanup_devices);
