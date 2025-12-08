#ifndef __INTF_NR_MESSAGES_ASN1_MSG__H__
#define __INTF_NR_MESSAGES_ASN1_MSG__H__

#include <stdint.h>
#include <stdio.h>

#include "NR_INTF-TEST-Message.h"
#include "intf_messages_types.h"

int do_INTF_MSG_TEST(uint8_t *buffer, size_t buffer_size, uint8_t int_data);

void do_INTF_MSG_DECODE_TEST(uint8_t *buf, uint32_t len);


#endif //__INTF_NR_MESSAGES_ASN1_MSG__H__