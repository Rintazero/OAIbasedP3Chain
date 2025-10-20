
#ifndef __INTF_NR_UE_H__
#define __INTF_NR_UE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char intf_NR_UE_test_str[];

int intf_NR_UE_test_func();

void *intf_nrue_task(void *args_p);

void *intf_nrue(void *notUsed);

#endif // __INTF_NR_UE_H__