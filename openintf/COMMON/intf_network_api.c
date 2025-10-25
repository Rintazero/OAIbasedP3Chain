#include "intf_network_api.h"
#include "common/utils/LOG/log.h"
#include "socket.h"
//#include "openair3/NAS/COMMON/UTIL/socket.h"
#include "common/platform_constants.h"

#include <string.h> // strerror, memset
#include <netdb.h>  // gai_strerror
#include <errno.h>  // errno
#include <unistd.h> // gethostname

/****************************************************************************/

#define NETWORK_API_RECV_BUFFER_SIZE 4096
#define NETWORK_API_SEND_BUFFER_SIZE NETWORK_API_RECV_BUFFER_SIZE

/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:  intf_network_api_initialize()                                   **
 **                                                                        **
 ** Description: Initializes the network API from which the INTF layer     **
 **    will send/receive messages to/from the network sublayer             **
 **                                                                        **
 ** Inputs:                                                                **  
 **     intf_network_api_id: Pointer to the network API ID structure       **
 **     host:    The name of the host from which the net-                  **
 **       work sublayer will connect                                       **
 **      port:    The local port number                                    **
 **      Others:  None                                                     **
 **                                                                        **
 ** Outputs:   Return:  RETURNerror, RETURNok                              **
 **      Others:  _network_api_id                                          **
 **                                                                        **
 ***************************************************************************/

int intf_network_api_initialize(INTF_NETWORK_API_ID* intf_network_api_id,const char* host, const char* port){
   LOG_I(INTF,"network API init\n");

   intf_network_api_id->open  = socket_udp_open;
   intf_network_api_id->getfd = socket_get_fd;
   intf_network_api_id->recv  = socket_recv;
   intf_network_api_id->send  = socket_send;
   intf_network_api_id->close = socket_close;

   intf_network_api_id->endpoint = intf_network_api_id->open(SOCKET_CLIENT, host, port);
   if (intf_network_api_id->endpoint == NULL) {
       LOG_E(INTF, "INTF - Failed to open connection endpoint, %s\n",
              ((errno < 0) ? gai_strerror(errno) : strerror(errno)));
       return RETURNerror;
   }
   char intf_network_api_send_buffer[NETWORK_API_SEND_BUFFER_SIZE];
   gethostname(intf_network_api_send_buffer, sizeof(intf_network_api_send_buffer));
   LOG_I(INTF, "INTF - Network's UDP socket %d is BOUND to %s/%s\n",
            intf_network_api_id->getfd(intf_network_api_id->endpoint), intf_network_api_send_buffer, port);
   return RETURNok;
}

int intf_network_api_get_fd(INTF_NETWORK_API_ID* intf_network_api_id){
    // LOG_I(INTF,"network API get fd\n");
    return intf_network_api_id->getfd(intf_network_api_id->endpoint);
}