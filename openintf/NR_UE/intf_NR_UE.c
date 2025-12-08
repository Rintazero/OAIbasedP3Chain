#define INTF_NR_UE
#define INTF_NR_UE_C
#include "common/utils/system.h"

#include "intertask_interface.h"
#include "intf_NR_UE.h"

// ASN1
#include "NR_INTF-TEST-Message.h"

// MSG
#include "intf_asn1_msg.h"

// #include "nr-uesoftmodem.h"
#include "common/utils/LOG/log.h"
#include "common/utils/LOG/vcd_signal_dumper.h"

// intf network api include
#include "openintf/COMMON/intf_network_api.h"

// rlc api
#include "LAYER2/nr_rlc/nr_rlc_oai_api.h"

// pdcp api
#include "nr_pdcp/nr_pdcp_oai_api.h"

// net and socket includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>
#include <stdio.h>


char intf_NR_UE_test_str[] = "OpenAirInterface NR_UE Interface Test String";

int intf_NR_UE_test_func() {
    return 2025;
}

// Global network API ID for INTF NR-UE
INTF_NETWORK_API_ID intf_nrue_network_api_id;
static int udp_socket_fd = -1;

// udp processer(test) triggered by receiving udp pkg
int process_udp_pkg(INTF_NETWORK_API_ID* api_id, const char* data, size_t len){
    if(len > 0){
        LOG_I(INTF, "INTF NR-UE processing UDP pkg: %.*s\n", (int)len, data);
        // LOG_I(INTF, "INTF NR-UE replying to UDP pkg\n");
        char reply[] = "INTF NR-UE: copy that";
        ssize_t reply_len = strlen(reply);
        api_id->send(api_id->endpoint, reply, reply_len);
        
        char data_buf[256];
        size_t copy_len = (len < sizeof(data_buf) - 1) ? len : (sizeof(data_buf) - 1);
        memcpy(data_buf, data, copy_len);
        data_buf[copy_len] = '\0';

        if(strcmp(data_buf,"rrc_test") == 0){
            LOG_I(INTF, "INTF NR-UE received rrc_test command via UDP\n");
            // Here you can add code to trigger RRC test functions
            MessageDef *msg = itti_alloc_new_message(TASK_INTF_NRUE, 0, INTF_HELLOWORLD_MSG);
            INTF_HELLOWORLD_MSG(msg).num_data = 2025;
            itti_send_msg_to_task(TASK_RRC_NRUE, 0, msg);
        } else if(strcmp(data_buf,"send_msg_via_rlc") == 0){
            LOG_I(INTF, "INTF NR-UE received send_msg_via_rlc command via UDP\n");
            // Here you can add code to trigger RLC message sending
            uint8_t buf[1024];
            long msg_data = 125; // as it defined in ASN.1, val must be in (0...255)
            int len = do_INTF_MSG_TEST(buf, sizeof(buf), msg_data);
            // do_INTF_MSG_DECODE_TEST(buf, len);
            // LOG_I(INTF, "ASN1 buf: %.*s\n", len, buf);
            nr_rlc_srb_recv_sdu(0, 1, buf, len);
            // nr_pdcp_data_req_srb(0, 1, 0, len, buf, deliver_pdu_srb_rlc, NULL);
        } else if(strcmp(data_buf,"send_msg_via_pdcp") == 0){
            LOG_I(INTF, "INTF NR-UE received send_msg_via_pdcp command via UDP\n");
            // Here you can add code to trigger PDCP message sending
            uint8_t buf[1024];
            long msg_data = 10;
            int len = do_INTF_MSG_TEST(buf, sizeof(buf), msg_data);
            // nr_rlc_srb_recv_sdu(0, 1, buf, len);
            nr_pdcp_data_req_srb(0, 1, 0, len, buf, deliver_pdu_srb_rlc, NULL);
        }
    } else {
        LOG_W(INTF, "INTF NR-UE received empty UDP pkg\n");
    }
    return 0;
}

// nr-ue function interface task
void *intf_nrue_task(void *args_p){
    itti_mark_task_ready(TASK_INTF_NRUE);

    //* test: Send a hello world message
    MessageDef *msg = itti_alloc_new_message(TASK_INTF_NRUE, 0, INTF_HELLOWORLD_MSG);
    INTF_HELLOWORLD_MSG(msg).num_data = 2025;
    itti_send_msg_to_task(TASK_INTF_NRUE, 0, msg);
    //*/

    LOG_I(INTF, "INTF NR-UE task started\n");
    // Initialize UDP communication
    // You can get these from a config file

    //*
    const char* udp_host = "127.0.0.1";
    const char* udp_port = "21212";
    if (intf_network_api_initialize(&intf_nrue_network_api_id, udp_host, udp_port) != RETURNok) {
        LOG_E(INTF, "Failed to initialize network API for INTF NR-UE task\n");
        return NULL;
    }
    udp_socket_fd = intf_network_api_get_fd(&intf_nrue_network_api_id);
    if(udp_socket_fd == -1){
        LOG_E(INTF, "Failed to get UDP socket fd for INTF NR-UE task\n");
        return NULL;
    } else {
        LOG_I(INTF, "INTF NR-UE UDP socket fd: %d\n", udp_socket_fd);
    }
    LOG_I(INTF,"Subscribing UDP socket fd %d to ITTI epoll events\n", udp_socket_fd);
    itti_subscribe_event_fd(TASK_INTF_NRUE, udp_socket_fd);
    LOG_I(INTF,"Finished subscribing UDP socket fd to ITTI epoll events\n");
    //*/

    while(1){
        intf_nrue(NULL);
    }
}

void *intf_nrue(void *notUsed){
    int result;
    MessageDef *msg_p = NULL;
    itti_receive_msg(TASK_INTF_NRUE, &msg_p);
    // 
    // LOG_D(INTF, "[UE %ld] Received %s\n", instance, ITTI_MSG_NAME(msg_p));
    
    if(msg_p!=NULL){
        instance_t instance = ITTI_MSG_DESTINATION_INSTANCE(msg_p);
        LOG_I(INTF, "[UE %ld] Received %s\n", instance, ITTI_MSG_NAME(msg_p));
        switch(ITTI_MSG_ID(msg_p)){
            case TERMINATE_MESSAGE:
                LOG_W(INTF, " *** Exiting INTF thread\n");
                intf_nrue_network_api_id.close(intf_nrue_network_api_id.endpoint);
                itti_exit_task();
                break;

            default:
                LOG_W(INTF, "[UE %ld] Unhandled INTF message %s\n", instance, ITTI_MSG_NAME(msg_p));
                break;
        }
        result = itti_free(ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
        AssertFatal(result == EXIT_SUCCESS, "Failed to free memory: itti_free error code (%d)!\n", result);
    }
    //*
    const int MAX_EVENTS = 5;
    struct epoll_event events[MAX_EVENTS];
    int numEvents = itti_get_events(TASK_INTF_NRUE, events, MAX_EVENTS);
    if(numEvents > 0){
        LOG_I(INTF, "INTF NR-UE received %d events from epoll\n", numEvents);
        for(int i=0; i<numEvents; i++){
            LOG_I(INTF, "event %d: fd=%d, events=0x%x\n", i, events[i].data.fd, events[i].events);
            if(events[i].data.fd == udp_socket_fd){
                // Data available on UDP socket
                char buffer[1024];
                ssize_t len = intf_nrue_network_api_id.recv(intf_nrue_network_api_id.endpoint, buffer, sizeof(buffer)-1);
                process_udp_pkg(&intf_nrue_network_api_id, buffer, len);
            } else {
                LOG_W(INTF, "INTF NR-UE received event on unknown fd %d\n", events[i].data.fd);
            }
        }
    }
    //*/

    return NULL;
}