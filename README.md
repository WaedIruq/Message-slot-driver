# Message-slot-driver

# Linux Kernel Module: Message Slot Device

This is a Linux kernel module that implements a simple message slot device. The module allows user-space applications to communicate with the kernel by sending and receiving messages through a character device. It supports multiple minor devices and channels, enabling message passing between user-space and kernel-space.

## Features

- **Multiple Minor Devices**: Supports multiple minor devices, each with its own set of channels.
- **Channels**: Each minor device can have multiple channels, identified by a unique channel ID.
- **Message Passing**: Allows user-space applications to send and receive messages up to 128 bytes in length.
- **IOCTL Interface**: Provides an IOCTL interface to set the active channel for a file descriptor.
- **Dynamic Memory Allocation**: Uses `kmalloc` and `kfree` for dynamic memory management.

## Code Overview

The module consists of the following components:

1. **Data Structures**:
   - `struct device_file`: Represents a minor device and contains a linked list of message slots.
   - `struct message_slot`: Represents a channel and contains a message buffer and its length.

2. **Functions**:
   - `device_open`: Initializes a minor device and its first message slot.
   - `device_ioctl`: Sets the active channel for a file descriptor.
   - `device_read`: Reads a message from the active channel.
   - `device_write`: Writes a message to the active channel.
   - `get_message_slot`: Helper function to retrieve or create a message slot for a given channel ID.
   - `init`: Initializes the module and registers the character device.
   - `cleanup_devices`: Cleans up resources and unregisters the character device.

3. **File Operations**:
   - The `file_operations` structure defines the operations supported by the device (open, read, write, and ioctl).
