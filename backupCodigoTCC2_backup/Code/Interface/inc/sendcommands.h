#ifndef SENDCOMMANDS_H
#define SENDCOMMANDS_H

#include<stdint.h>
#include "socket_local.h"

class sendCommands
{
    private:
        int socket_id;
        char socket_name[100];
        interface_to_machine_message interface_message;

        bool enabled;
        int16_t sampling_period;

        


    public:
        sendCommands();
        ~sendCommands();
        void setCommand(uint8_t command);
        uint8_t errorOccurred;

        uint8_t connectToMachine();
        int16_t sendMessage();
        
        void setVelocity(int16_t velocity);
        void setDistance(int16_t distance);
        void setPressure(uint8_t pressure);
        void setEnabled(uint8_t enabled);
        void setSamplingPeriod(uint16_t sample_period);
        int16_t getVelocity();
        int16_t getDistance();
        uint16_t getPressure();
        uint8_t getEnabled();
        uint16_t getSamplingPeriod();

};

#endif // SENDCOMMANDS_H
