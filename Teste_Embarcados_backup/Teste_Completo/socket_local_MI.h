#ifndef SOCKET_LOCAL_MI_H
#define SOCKET_LOCAL_MI_H

#include <stdint.h>

#define payload_size 8*sizeof(int)

typedef union {
    unsigned char payload[payload_size];
    struct {
        int sample_number;
        float displacement[2];
        float load[2];
        uint8_t state;
    };
} machine_message;

#endif
