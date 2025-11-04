#define INTF_NR_UE
#define INTF_NR_UE_C

#include "common/utils/system.h"

#include "intertask_interface.h"
#include "intf_NR_gNB.h"

// #include "nr-uesoftmodem.h"
#include "common/utils/LOG/log.h"
#include "common/utils/LOG/vcd_signal_dumper.h"

// intf network api include
#include "openintf/COMMON/intf_network_api.h"

// net and socket includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


char intf_NR_gNB_test_str[] = "OpenAirInterface NR_gNB Interface Test String";

int intf_NR_gNB_test_func() {
    return 2025;
}

// Global network API ID for INTF NR-gNB
INTF_NETWORK_API_ID intf_gnb_network_api_id;
static int udp_socket_fd = -1;

// udp processer(test) triggered by receiving udp pkg
int process_udp_pkg(INTF_NETWORK_API_ID* api_id, const char* data, size_t len){
    if(len > 0){
        LOG_I(INTF, "INTF gNB processing UDP pkg: %.*s\n", (int)len, data);
        LOG_I(INTF, "INTF gNB replying to UDP pkg\n");
        char reply[] = "Hello from INTF gNB";
        ssize_t reply_len = strlen(reply);
        api_id->send(api_id->endpoint, reply, reply_len);
    } else {
        LOG_W(INTF, "INTF gNB received empty UDP pkg\n");
    }
    return 0;
}

// nr-gNB function interface task
void *intf_gNB_task(void *args_p){
    itti_mark_task_ready(TASK_INTF_GNB);

    //* test: Send a hello world message
    MessageDef *msg = itti_alloc_new_message(TASK_INTF_GNB, 0, INTF_HELLOWORLD_MSG);
    INTF_HELLOWORLD_MSG(msg).num_data = 2025;
    itti_send_msg_to_task(TASK_INTF_GNB, 0, msg);

    //*
    const char* udp_host = "127.0.0.1";
    const char* udp_port = "21214";
    if (intf_network_api_initialize(&intf_gnb_network_api_id, udp_host, udp_port) != RETURNok) {
        LOG_E(INTF, "Failed to initialize network API for INTF gNB task\n");
        return NULL;
    }
    udp_socket_fd = intf_network_api_get_fd(&intf_gnb_network_api_id);
    if(udp_socket_fd == -1){
        LOG_E(INTF, "Failed to get UDP socket fd for INTF gNB task\n");
        return NULL;
    } else {
        LOG_I(INTF, "INTF gNB UDP socket fd: %d\n", udp_socket_fd);
    }
    LOG_I(INTF,"Subscribing UDP socket fd %d to ITTI epoll events\n", udp_socket_fd);
    itti_subscribe_event_fd(TASK_INTF_GNB, udp_socket_fd);
    LOG_I(INTF,"Finished subscribing UDP socket fd to ITTI epoll events\n");
    //*/

    LOG_I(INTF,"Entering main loop of NR_GNB interface task\n");
    while(1){
        intf_gNB(NULL);
    }
}

void *intf_gNB(void *notUsed){
    int result;
    MessageDef *msg_p = NULL;
    itti_receive_msg(TASK_INTF_GNB, &msg_p);
    // LOG_D(INTF, "[UE %ld] Received %s\n", instance, ITTI_MSG_NAME(msg_p));
    
    if(msg_p!=NULL){
        instance_t instance = ITTI_MSG_DESTINATION_INSTANCE(msg_p);
        LOG_I(INTF, "instance %ld: Received %s\n", instance, ITTI_MSG_NAME(msg_p));
        switch(ITTI_MSG_ID(msg_p)){
            case TERMINATE_MESSAGE:
                LOG_W(INTF, " *** Exiting INTF thread\n");
                // intf_nrgnb_network_api_id.close(intf_nrgnb_network_api_id.endpoint);
                itti_exit_task();
                break;

            default:
                LOG_W(INTF, "instance %ld: Unhandled INTF message %s\n", instance, ITTI_MSG_NAME(msg_p));
                break;
        }
        result = itti_free(ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
        AssertFatal(result == EXIT_SUCCESS, "Failed to free memory: itti_free error code (%d)!\n", result);
    }

    //*
    const int MAX_EVENTS = 5;
    struct epoll_event events[MAX_EVENTS];
    int numEvents = itti_get_events(TASK_INTF_GNB, events, MAX_EVENTS);
    if(numEvents > 0){
        LOG_I(INTF, "INTF gNB received %d events from epoll\n", numEvents);
        for(int i=0; i<numEvents; i++){
            LOG_I(INTF, "event %d: fd=%d, events=0x%x\n", i, events[i].data.fd, events[i].events);
            if(events[i].data.fd == udp_socket_fd){
                // Data available on UDP socket
                char buffer[1024];
                ssize_t len = intf_gnb_network_api_id.recv(intf_gnb_network_api_id.endpoint, buffer, sizeof(buffer)-1);
                process_udp_pkg(&intf_gnb_network_api_id, buffer, len);
            } else {
                LOG_W(INTF, "INTF gNB received event on unknown fd %d\n", events[i].data.fd);
            }
        }
    }
    //*/

    return NULL;
}