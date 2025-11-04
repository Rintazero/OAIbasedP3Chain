#ifndef __INT_NR_GNB_H__
#define __INT_NR_GNB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char intf_NR_gNB_test_str[];

int intf_NR_gNB_test_func();

void *intf_gNB_task(void *args_p);

void *intf_gNB(void *notUsed);

#endif // __INT_NR_GNB_H__