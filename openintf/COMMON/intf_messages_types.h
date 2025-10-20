#ifndef INTF_MESSAGES_TYPES_H__
#define INTF_MESSAGES_TYPES_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define INTF_HELLOWORLD_MSG(mSGpTR) (mSGpTR)->ittiMsg.intf_helloworld_msg

typedef struct {
    int num_data;
} intf_helloworld_msg_t;


#endif // INTF_MESSAGES_TYPES_H__