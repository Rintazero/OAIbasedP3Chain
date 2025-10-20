#define INTF_NR_UE
#define INTF_NR_UE_C

#include "intertask_interface.h"
#include "intf_NR_UE.h"

//#include "nr-uesoftmodem.h"
#include "common/utils/LOG/log.h"
#include "common/utils/LOG/vcd_signal_dumper.h"

char intf_NR_UE_test_str[] = "OpenAirInterface NR_UE Interface Test String";

int intf_NR_UE_test_func() {
    return 2025;
}

// nr-ue function interface task
void *intf_nrue_task(void *args_p){
    itti_mark_task_ready(TASK_INTF_NRUE);
    LOG_I(INTF, "INTF NR-UE task started\n");
    while(1){
        intf_nrue(NULL);
    }
}

void *intf_nrue(void *notUsed){
    MessageDef *msg_p = NULL;
    itti_receive_msg(TASK_INTF_NRUE, &msg_p);
    instance_t instance = ITTI_MSG_DESTINATION_INSTANCE(msg_p);
    LOG_D(INTF, "[UE %ld] Received %s\n", instance, ITTI_MSG_NAME(msg_p));

    switch(ITTI_MSG_ID(msg_p)){
        case TERMINATE_MESSAGE:
            LOG_W(INTF, " *** Exiting INTF thread\n");
            itti_exit_task();
            break;

        default:
            LOG_W(INTF, "[UE %ld] Unhandled INTF message %s\n", instance, ITTI_MSG_NAME(msg_p));
            break;
    }

    return NULL;
}