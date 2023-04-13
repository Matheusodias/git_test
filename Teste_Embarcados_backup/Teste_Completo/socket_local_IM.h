#ifndef SOCKET_LOCAL_IM_H
#define SOCKET_LOCAL_IM_H

#include <stdint.h>

#define payload_size 3*sizeof(uint16_t)

typedef union {
    unsigned char payload[payload_size];
    struct {
        uint8_t command;
        union {
            uint16_t pressure;
            struct {
				uint8_t enabled_pos;
                int16_t distance;
                int16_t velocity;
            };
            struct {
                uint8_t enabled_data;
                uint16_t sample_period;
            };

        };
    };
} union_message;


#endif